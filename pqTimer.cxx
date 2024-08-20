// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#include "pqTimer.h"

#include "pqEventDispatcher.h"

//-----------------------------------------------------------------------------
pqTimer::pqTimer(QObject* parentObject)
  : Superclass(parentObject)
{
  pqEventDispatcher::registerTimer(this);
}

//-----------------------------------------------------------------------------
pqTimer::~pqTimer() {}

//-----------------------------------------------------------------------------
void pqTimer::timerEvent(QTimerEvent* evt)
{
  // here we can consume the timer event and reschedule it for another time if
  // timers are blocked.
  this->Superclass::timerEvent(evt);
}

//-----------------------------------------------------------------------------
void pqTimer::singleShot(int msec, QObject* receiver, const char* member)
{
  if (receiver && member)
  {
    pqTimer* timer = new pqTimer(receiver);
    QObject::connect(timer, SIGNAL(timeout()), receiver, member);
    QObject::connect(timer, SIGNAL(timeout()), timer, SLOT(deleteLater()));
    timer->setSingleShot(true);
    timer->start(msec);
  }
}
