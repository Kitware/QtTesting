// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

// Qt includes
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMainWindow>
#include <QPushButton>
#include <QTextStream>
#include <QTimer>

// QtTesting includes
#include "pqEventComment.h"
#include "pqEventRecorder.h"
#include "pqEventTypes.h"
#include "pqRecordEventsDialog.h"
#include "pqTestUtility.h"

#include "ui_pqRecordEventsDialog.h"

//////////////////////////////////////////////////////////////////////////////////
// pqImplementation

struct pqRecordEventsDialog::pqImplementation
{
public:
  pqImplementation(pqEventRecorder* recorder, pqTestUtility* testUtility)
    : Recorder(recorder)
    , TestUtility(testUtility)
  {
  }

  ~pqImplementation()
  {
    this->Recorder = 0;
    this->TestUtility = 0;
  }

  Ui::pqRecordEventsDialog Ui;

  pqEventRecorder* Recorder;
  pqTestUtility* TestUtility;
};

///////////////////////////////////////////////////////////////////////////////////
// pqRecordEventsDialog

// ----------------------------------------------------------------------------
pqRecordEventsDialog::pqRecordEventsDialog(
  pqEventRecorder* recorder, pqTestUtility* testUtility, QWidget* Parent)
  : QDialog(Parent)
  , Implementation(new pqImplementation(recorder, testUtility))
{
  this->Implementation->Ui.setupUi(this);
  this->setWindowFlags(this->windowFlags().setFlag(Qt::WindowContextHelpButtonHint, false));
  this->setWindowFlags(this->windowFlags().setFlag(Qt::WindowStaysOnTopHint, true));

  this->ignoreObject(this);

  this->setWindowTitle(tr("Recording User Input"));
  this->setObjectName("");

  QObject::connect(this->Implementation->TestUtility->eventTranslator(),
    SIGNAL(recordEvent(QString, QString, QString, int)), this,
    SLOT(onEventRecorded(QString, QString, QString, int)));

  QObject::connect(
    this->Implementation->Ui.commentAddButton, SIGNAL(clicked()), this, SLOT(addComment()));

  QObject::connect(this->Implementation->Ui.checkButton, SIGNAL(toggled(bool)),
    this->Implementation->Recorder, SLOT(check(bool)));

  QObject::connect(this->Implementation->Ui.recordPauseButton, SIGNAL(toggled(bool)),
    this->Implementation->Recorder, SLOT(unpause(bool)));

  QObject::connect(this->Implementation->Ui.continuousFlush, SIGNAL(toggled(bool)),
    this->Implementation->Recorder, SLOT(setContinuousFlush(bool)));

  if (this->Implementation->TestUtility->supportsDashboardMode())
  {
    QObject::connect(this->Implementation->Ui.dashboardMode, SIGNAL(toggled(bool)), this,
      SLOT(onDashboardModeToggled(bool)));
  }
  else
  {
    this->Implementation->Ui.dashboardMode->setVisible(false);
  }

  QObject::connect(this->Implementation->Ui.recordInteractionTimings, SIGNAL(toggled(bool)),
    this->Implementation->Recorder, SLOT(setRecordInteractionTimings(bool)));

  QObject::connect(this->Implementation->Recorder, SIGNAL(started()), this, SLOT(updateUi()));
  QObject::connect(this->Implementation->Recorder, SIGNAL(stopped()), this, SLOT(updateUi()));
}

// ----------------------------------------------------------------------------
pqRecordEventsDialog::~pqRecordEventsDialog()
{
  delete Implementation;
}

// ----------------------------------------------------------------------------
void pqRecordEventsDialog::ignoreObject(QObject* object)
{
  this->Implementation->TestUtility->eventTranslator()->ignoreObject(object);
  Q_FOREACH (QObject* child, object->children())
  {
    this->ignoreObject(child);
  }
}

// ----------------------------------------------------------------------------
void pqRecordEventsDialog::done(int value)
{
  this->Implementation->TestUtility->stopRecords(value);
  QDialog::done(value);
}

// ----------------------------------------------------------------------------
void pqRecordEventsDialog::onEventRecorded(
  const QString& widget, const QString& command, const QString& argument, int /*eventType*/)
{
  if (!this->Implementation->Recorder->isRecording())
  {
    return;
  }

  this->Implementation->Ui.eventWidgetEdit->setText(widget);
  this->Implementation->Ui.eventCommandEdit->setText(command);
  this->Implementation->Ui.eventArgumentEdit->setText(argument);
  int newValue = this->Implementation->Ui.nbEvents->value() + 1;
  this->Implementation->Ui.nbEvents->display(newValue);
}

// ----------------------------------------------------------------------------
void pqRecordEventsDialog::addComment()
{
  if (this->Implementation->Ui.blockingCheckBox->isChecked())
  {
    this->Implementation->Recorder->translator()->eventComment()->recordCommentBlock(
      this->Implementation->Ui.commentTextEdit->toPlainText());
  }
  else
  {
    this->Implementation->Recorder->translator()->eventComment()->recordComment(
      this->Implementation->Ui.commentTextEdit->toPlainText());
  }
  this->Implementation->Ui.commentTextEdit->clear();
}

// ----------------------------------------------------------------------------
void pqRecordEventsDialog::updateUi()
{
  bool recording = this->Implementation->Recorder->isRecording();
  this->Implementation->Ui.recordPauseButton->setChecked(recording);
  this->Implementation->Ui.dashboardMode->setEnabled(recording);
}

// ----------------------------------------------------------------------------
void pqRecordEventsDialog::onDashboardModeToggled(bool toggle)
{
  this->Implementation->TestUtility->setDashboardMode(toggle);

  const QWidgetList& list = QApplication::topLevelWidgets();
  QMainWindow* mainWindow = nullptr;
  for (QWidget* widg : list)
  {
    mainWindow = qobject_cast<QMainWindow*>(widg);
    if (mainWindow)
    {
      break;
    }
  }
  this->Implementation->Recorder->translator()->recordDashboardModeToggle(mainWindow, toggle);
}
