/*=========================================================================

   Program: ParaView
   Module:    pqEventObserver.h

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

#ifndef _pqEventObserver_h
#define _pqEventObserver_h

#include <QObject>
#include <QString>
#include "QtTestingExport.h"
class QTextStream;

/**
Observes high-level ParaView events, and serializes them to a stream 
for possible playback (as a test-case, demo, tutorial, etc).  To use,
connect the onRecordEvent() slot to the pqEventTranslator::recordEvent()
signal.

\sa pqEventTranslator, pqStdoutEventObserver
*/

class QTTESTING_EXPORT pqEventObserver :
  public QObject
{
  Q_OBJECT
  
public:
  pqEventObserver(QObject* p);
  ~pqEventObserver();

public:
  virtual void setStream(QTextStream* stream);

public slots:
  // Slot called when recording an event
  // Event Type can be pqEventTypes::ACTION_EVENT or pqEventType::CHECK_EVENT
  // Widget, Command/Property and Arguments are QString
  // THis method has to be redefined by subclasses
  virtual void onRecordEvent(
    const QString& Widget,
    const QString& Command,
    const QString& Arguments,
    const int& eventType)=0;


signals:
  void eventRecorded(const QString& Widget,
                     const QString& Command,
                     const QString& Arguments);

protected:
  QTextStream* Stream;
};

#endif // !_pqEventObserver_h

