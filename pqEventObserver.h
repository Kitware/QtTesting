// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqEventObserver_h
#define _pqEventObserver_h

#include "QtTestingExport.h"
#include <QObject>
#include <QString>
class QTextStream;

/**
Observes high-level ParaView events, and serializes them to a stream
for possible playback (as a test-case, demo, tutorial, etc).  To use,
connect the onRecordEvent() slot to the pqEventTranslator::recordEvent()
signal.

\sa pqEventTranslator, pqStdoutEventObserver
*/

class QTTESTING_EXPORT pqEventObserver : public QObject
{
  Q_OBJECT

public:
  pqEventObserver(QObject* p);
  ~pqEventObserver() override;

public:
  virtual void setStream(QTextStream* stream);

public Q_SLOTS:
  // Slot called when recording an event
  // Event Type can be pqEventTypes::ACTION_EVENT or pqEventType::CHECK_EVENT
  // Widget, Command/Property and Arguments are QString
  // THis method has to be redefined by subclasses
  virtual void onRecordEvent(const QString& Widget, const QString& Command,
    const QString& Arguments, const int& eventType) = 0;

Q_SIGNALS:
  void eventRecorded(
    const QString& Widget, const QString& Command, const QString& Arguments, const int& eventType);

protected:
  QTextStream* Stream;
};

#endif // !_pqEventObserver_h
