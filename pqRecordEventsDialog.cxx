/*=========================================================================

   Program: ParaView
   Module:    pqRecordEventsDialog.cxx

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

// Qt includes
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QTimer>
#include <QPushButton>

// QtTesting includes
#include "pqEventRecorder.h"
#include "pqRecordEventsDialog.h"
#include "pqTestUtility.h"

#include "ui_pqRecordEventsDialog.h"

//////////////////////////////////////////////////////////////////////////////////
// pqImplementation

struct pqRecordEventsDialog::pqImplementation
{
public:
  pqImplementation(pqEventRecorder* recorder,
                   pqTestUtility* testUtility)
    : Recorder(recorder), TestUtility(testUtility)
  {
  }
  
  ~pqImplementation()
  {
    this->Recorder = 0;
    this->TestUtility = 0;
  }

  Ui::pqRecordEventsDialog Ui;

  pqEventRecorder*    Recorder;
  pqTestUtility*      TestUtility;
};

///////////////////////////////////////////////////////////////////////////////////
// pqRecordEventsDialog

// ----------------------------------------------------------------------------
pqRecordEventsDialog::pqRecordEventsDialog(pqEventRecorder* recorder,
                                           pqTestUtility* testUtility,
                                           QWidget* Parent)
  : QDialog(Parent),
    Implementation(new pqImplementation(recorder, testUtility))
{
  this->Implementation->Ui.setupUi(this);

  this->Implementation->TestUtility->eventTranslator()->ignoreObject(this->Implementation->Ui.stopButton);
  this->Implementation->TestUtility->eventTranslator()->ignoreObject(this);

  this->setWindowTitle(tr("Recording User Input"));
  this->setObjectName("");

}

// ----------------------------------------------------------------------------
pqRecordEventsDialog::~pqRecordEventsDialog()
{
  delete Implementation;
}

// ----------------------------------------------------------------------------
void pqRecordEventsDialog::done(int value)
{
  this->Implementation->TestUtility->stopRecords();
  QDialog::done(value);
}
