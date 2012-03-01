/*=========================================================================

   Program: ParaView
   Module:    pqPlayBackEventsDialog.cxx

   Copyright (c) 2005-2008 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 

   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#include "pqEventDispatcher.h"
#include "pqEventPlayer.h"
#include "pqPlayBackEventsDialog.h"
#include "pqTestUtility.h"

#include "ui_pqPlayBackEventsDialog.h"

#include <QCheckBox>
#include <QFile>
#include <QFileDialog>
#include <QTableWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QStringListModel>
#include <QTextStream>
#include <QTimer>

#include <QDebug>

//////////////////////////////////////////////////////////////////////////////////
// pqImplementation

struct pqPlayBackEventsDialog::pqImplementation
{
public:
  pqImplementation(pqEventPlayer& player,
                   pqEventDispatcher& dispatcher,
                   pqTestUtility* testUtility);
  ~pqImplementation();
  void init(pqPlayBackEventsDialog* dialog);
  void setProgressBarsValue(int value);
  void setProgressBarValue(int row, int value);

  Ui::pqPlayBackEventsDialog Ui;

  pqEventPlayer&      Player;
  pqEventDispatcher&  Dispatcher;
  pqTestUtility*      TestUtility;

  int           CurrentLine;
  int           MaxLines;
  int           CurrentFile;
  QStringList   Filenames;
  QStringList   CurrentEvent;
};

// ----------------------------------------------------------------------------
pqPlayBackEventsDialog::pqImplementation::pqImplementation(pqEventPlayer& player,
                                                           pqEventDispatcher& dispatcher,
                                                           pqTestUtility* testUtility)
  : Player(player)
  , Dispatcher(dispatcher)
  , TestUtility(testUtility)
{
  this->CurrentLine = 0;
  this->MaxLines = 0;
  this->CurrentFile = 0;
  this->Filenames = QStringList();
  this->CurrentEvent = QStringList();
}

// ----------------------------------------------------------------------------
pqPlayBackEventsDialog::pqImplementation::~pqImplementation()
{
}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::pqImplementation::init(pqPlayBackEventsDialog* dialog)
{
  this->Ui.setupUi(dialog);

  QObject::connect(&this->Player, SIGNAL(eventAboutToBePlayed(QString, QString, QString)),
                   dialog, SLOT(onEventAboutToBePlayed(QString, QString, QString)));

  QObject::connect(this->Ui.timeStepSpinBox, SIGNAL(valueChanged(int)),
                   &this->Dispatcher, SLOT(changeTimeStep(int)));

  QObject::connect(this->Ui.loadFileButton, SIGNAL(clicked()),
                   dialog, SLOT(onLoadFiles()));
  QObject::connect(this->Ui.playPauseButton, SIGNAL(clicked(bool)),
                   dialog, SLOT(onPlayOrPause(bool)));
  QObject::connect(this->Ui.stopButton, SIGNAL(clicked()),
                   this->TestUtility, SLOT(stop()));
  QObject::connect(this->Ui.stepButton, SIGNAL(clicked()),
                   &this->Dispatcher, SLOT(oneStep()));

  QObject::connect(this->TestUtility, SIGNAL(started(QString)),
                   dialog, SLOT(onStarted(QString)));

  QObject::connect(this->TestUtility, SIGNAL(stopped()),
                   dialog, SLOT(updateUi()));
  QObject::connect(&this->Dispatcher, SIGNAL(paused()),
                   dialog, SLOT(updateUi()));
  QObject::connect(&this->Dispatcher, SIGNAL(started()),
                   dialog, SLOT(updateUi()));

}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::pqImplementation::setProgressBarsValue(int value)
{
  for(int i = 0; i < this->Ui.tableWidget->rowCount(); ++i)
    {
    this->setProgressBarValue(i, value);
    }
}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::pqImplementation::setProgressBarValue(int row, int value)
{
  QWidget* widget = this->Ui.tableWidget->cellWidget(row, 2);
  QProgressBar* progressBar = qobject_cast<QProgressBar*>(widget);
  progressBar->setValue(value);
}

///////////////////////////////////////////////////////////////////////////////////
// pqPlayBackEventsDialog

// ----------------------------------------------------------------------------
pqPlayBackEventsDialog::pqPlayBackEventsDialog(pqEventPlayer& Player,
                                               pqEventDispatcher& Dispatcher,
                                               pqTestUtility* TestUtility,
                                               QWidget* Parent)
  : QDialog(Parent)
  , Implementation(new pqImplementation(Player, Dispatcher, TestUtility))
{
  this->Implementation->init(this);

  this->onLoadFiles();
}

// ----------------------------------------------------------------------------
pqPlayBackEventsDialog::~pqPlayBackEventsDialog()
{
  delete Implementation;
}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::done(const int& value)
{
  this->Implementation->TestUtility->stopTests();
  QDialog::done(value);
  delete this;
}

// ----------------------------------------------------------------------------
QStringList pqPlayBackEventsDialog::fileNamesSelected()
{
  QStringList list;
  for(int i = 0; i < this->Implementation->Ui.tableWidget->rowCount(); ++i)
    {
    QCheckBox* box = qobject_cast<QCheckBox*>(
        this->Implementation->Ui.tableWidget->cellWidget(i, 0));
    if (box->isChecked())
      {
      list << this->Implementation->Filenames[i];
      }
    }
  return list;
}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::onEventAboutToBePlayed(const QString& Object,
                                                    const QString& Command,
                                                    const QString& Arguments)
{
  ++this->Implementation->CurrentLine;
  QStringList newEvent;
  newEvent << Object << Command << Arguments;
  this->Implementation->CurrentEvent = newEvent;
  this->updateUi();
}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::onLoadFiles()
{
  QStringList filenames = QFileDialog::getOpenFileNames(this, "Macro File Name",
    QString(), "XML Files (*.xml)");
  if (!filenames.isEmpty())
    {
    this->Implementation->Filenames = filenames;

    this->loadFiles(this->Implementation->Filenames);

    this->updateUi();
    }
}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::loadFiles(const QStringList& filenames)
{
  int rowCount = this->Implementation->Ui.tableWidget->rowCount();
  for(int i = 0 ; i < rowCount ; i++)
    {
    this->Implementation->Ui.tableWidget->removeRow(0);
    }
  for(int i = 0 ; i < filenames.count() ; i++)
    {
    QFileInfo info(filenames[i]);
    this->Implementation->Ui.tableWidget->insertRow(i);
    this->Implementation->Ui.tableWidget->setItem(
        i, 1, new QTableWidgetItem(info.fileName()));
    this->Implementation->Ui.tableWidget->setCellWidget(
        i, 2, new QProgressBar(this->Implementation->Ui.tableWidget));
    this->Implementation->setProgressBarsValue(0);
    QCheckBox* check = new QCheckBox(this->Implementation->Ui.tableWidget);
    check->setChecked(true);
    this->Implementation->Ui.tableWidget->setCellWidget(i, 0, check);
    this->Implementation->Ui.tableWidget->resizeColumnToContents(0);
    }
}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::onPlayOrPause(bool playOrPause)
{
  if(playOrPause)
    {
    if(!this->Implementation->TestUtility->playingTest())
      {
      QStringList newList = this->fileNamesSelected();
      this->Implementation->TestUtility->playTests(newList);
      }
    else
      {
      this->Implementation->Dispatcher.restart();
      }
    }
  else
    {
    this->Implementation->Dispatcher.pause();
    }
}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::onStarted(const QString& filename)
{
  this->Implementation->CurrentFile = this->Implementation->Filenames.indexOf(filename);
  this->Implementation->Ui.tableWidget->setCurrentCell(
      this->Implementation->CurrentFile, 1, QItemSelectionModel::Rows | QItemSelectionModel::SelectCurrent);

  this->Implementation->MaxLines = 0;
  this->Implementation->CurrentLine = 0;

  QFile file(filename);
  QFileInfo infoFile(file);
  file.open(QIODevice::ReadOnly);
  QTextStream stream(&file);
  this->Implementation->Ui.currentFileLabel->setText(infoFile.fileName());
  while(!stream.atEnd())
    {
    QString line = stream.readLine();
    if(line.replace(" ", "").startsWith("<event"))
      {
      ++this->Implementation->MaxLines;
      }
    }
}

// ----------------------------------------------------------------------------
void pqPlayBackEventsDialog::updateUi()
{
  // Update player buttons
  this->Implementation->Ui.playPauseButton->setEnabled(
      !this->Implementation->Filenames.isEmpty());
  this->Implementation->Ui.stepButton->setEnabled(
      this->Implementation->TestUtility->playingTest() && this->Implementation->Dispatcher.isPaused());
  this->Implementation->Ui.stopButton->setEnabled(
      this->Implementation->TestUtility->playingTest());

  // Play or pause
  this->Implementation->Ui.playPauseButton->setChecked(
      this->Implementation->TestUtility->playingTest() && !this->Implementation->Dispatcher.isPaused());

  // loadFile button
  this->Implementation->Ui.loadFileButton->setEnabled(
      !this->Implementation->TestUtility->playingTest());

  // Time step
  this->Implementation->Ui.timeStepSpinBox->setEnabled(
      !this->Implementation->Filenames.isEmpty());

  QString command = tr("Command : ");
  QString argument = tr("Argument(s) : ");
  QString object = tr("Object : ");
  if(this->Implementation->TestUtility->playingTest() &&
     !this->Implementation->CurrentEvent.isEmpty())
    {
    command += this->Implementation->CurrentEvent[1];
    argument += this->Implementation->CurrentEvent[2].right(25);
    object += this->Implementation->CurrentEvent[0].right(20);
    this->Implementation->setProgressBarValue(this->Implementation->CurrentFile,
      static_cast<int>((static_cast<double>(this->Implementation->CurrentLine)/static_cast<double>(this->Implementation->MaxLines-1))*100));
    }
  else
    {
    this->Implementation->setProgressBarsValue(0);
    }
  this->Implementation->Ui.commandLabel->setText(command);
  this->Implementation->Ui.argumentsLabel->setText(argument);
  this->Implementation->Ui.objectLabel->setText(object);
}
