// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqStdoutEventObserver_h
#define _pqStdoutEventObserver_h

#include "QtTestingExport.h"
#include <QObject>

/**
Observes high-level ParaView "events" and writes them to stdout,
mainly for debugging purposes. To use, connect the onRecordEvent()
slot to the pqEventTranslator::recordEvent() signal.

\sa pqEventTranslator, pqEventObserverXML
*/

class QTTESTING_EXPORT pqStdoutEventObserver : public QObject
{
  Q_OBJECT

public Q_SLOTS:
  void onRecordEvent(
    const QString& Widget, const QString& Command, const QString& Arguments, const int& eventType);
};

#endif // !_pqStdoutEventObserver_h
