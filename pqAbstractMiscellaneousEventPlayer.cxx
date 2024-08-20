// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqAbstractMiscellaneousEventPlayer.h"

#include <QAbstractButton>
#include <QFile>
#include <QThread>
#include <QtDebug>

#include "pqEventDispatcher.h"
#include "pqTestUtility.h"

// Class that encapsulates the protected function QThread::msleep
class SleeperThread : public QThread
{
public:
  // Allows for cross platform sleep function
  static bool msleep(unsigned long msecs)
  {
    QThread::msleep(msecs);
    return true;
  }
};

pqAbstractMiscellaneousEventPlayer::pqAbstractMiscellaneousEventPlayer(
  pqTestUtility* util, QObject* p)
  : pqWidgetEventPlayer(p)
  , TestUtility(util)
{
}

// Allows for execution of testing commands that don't merit their own class
bool pqAbstractMiscellaneousEventPlayer::playEvent(
  QObject* Object, const QString& Command, const QString& Arguments, bool& Error)
{
  if (Command == "pause")
  {
    const int value = Arguments.toInt();
    if (SleeperThread::msleep(value))
    {
      return true;
    }
    Error = true;
    qCritical() << "calling pause on unhandled type " << Object;
  }
  if (Command == "process_events")
  {
    bool valid = false;
    const int ms = Arguments.toInt(&valid);
    if (valid)
    {
      pqEventDispatcher::processEventsAndWait(ms);
    }
    else
    {
      pqEventDispatcher::processEvents();
    }
    return true;
  }
  if (Command == "dashboard_mode")
  {
    bool toggle = QVariant(Arguments).toBool();
    this->TestUtility->setDashboardMode(toggle);
    return true;
  }
  return false;
}
