// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqPythonEventObserver_h
#define _pqPythonEventObserver_h

#include "pqEventObserver.h"
#include <QHash>
#include <QString>

/**
Observes high-level ParaView events, and serializes them to a stream as Python
for possible playback (as a test-case, demo, tutorial, etc).  To use,
connect the onRecordEvent() slot to the pqEventTranslator::recordEvent()
signal.

\note Output is sent to the stream from this object's destructor, so you
must ensure that it goes out of scope before trying to playback the stream.

\sa pqEventTranslator, pqStdoutEventObserver, pqPythonEventSource.
*/

class QTTESTING_EXPORT pqPythonEventObserver : public pqEventObserver
{
  Q_OBJECT

public:
  pqPythonEventObserver(QObject* p = 0);
  ~pqPythonEventObserver();

  void onRecordEvent(
    const QString& Widget, const QString& Command, const QString& Arguments, const int& eventType);

  void setStream(QTextStream*);

private:
  /// Stores a stream that will be used to store the Python output
  QHash<QString, QString> Names;
};

#endif // !_pqPythonEventObserver_h
