// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqThreadedEventSource.h"

#include <QMutex>
#include <QString>
#include <QThread>
#include <QWaitCondition>

#include "pqEventDispatcher.h"

class pqThreadedEventSource::pqInternal : public QThread
{
  friend class pqThreadedEventSource;

public:
  pqInternal(pqThreadedEventSource& source)
    : Source(source)
    , ShouldStop(0)
    , GotEvent(0)
  {
  }

  void run() override { Source.run(); }

  pqThreadedEventSource& Source;

  QWaitCondition WaitCondition;
  int Waiting;

  int ShouldStop;
  int GotEvent;
  QString CurrentObject;
  QString CurrentCommand;
  QString CurrentArgument;

  class ThreadHelper : public QThread
  {
  public:
    static void msleep(int msecs) { QThread::msleep(msecs); }
  };
};

pqThreadedEventSource::pqThreadedEventSource(QObject* p)
  : pqEventSource(p)
{
  this->Internal = new pqInternal(*this);
}

pqThreadedEventSource::~pqThreadedEventSource()
{
  // wait a second for this thread to finish, if it hasn't yet
  this->Internal->wait(1000);
  delete this->Internal;
}

int pqThreadedEventSource::getNextEvent(QString& object, QString& command, QString& arguments)
{

  while (!this->Internal->GotEvent)
  {
    // wait for the other thread to post an event, while
    // we keep the GUI alive.
    pqEventDispatcher::processEventsAndWait(100);
  }

  object = this->Internal->CurrentObject;
  command = this->Internal->CurrentCommand;
  arguments = this->Internal->CurrentArgument;
  this->Internal->GotEvent = 0;
  this->guiAcknowledge();

  if (object.isEmpty())
  {
    if (arguments == "failure")
    {
      return FAILURE;
    }
    return DONE;
  }

  return SUCCESS;
}

void pqThreadedEventSource::relayEvent(QString object, QString command, QString arguments)
{
  this->Internal->CurrentObject = object;
  this->Internal->CurrentCommand = command;
  this->Internal->CurrentArgument = arguments;
  this->Internal->GotEvent = 1;
}

bool pqThreadedEventSource::postNextEvent(
  const QString& object, const QString& command, const QString& argument)
{
  QMetaObject::invokeMethod(this, "relayEvent", Qt::QueuedConnection, Q_ARG(QString, object),
    Q_ARG(QString, command), Q_ARG(QString, argument));

  return this->waitForGUI();
}

void pqThreadedEventSource::start()
{
  this->Internal->ShouldStop = 0;
  this->Internal->start(QThread::LowestPriority);
}

void pqThreadedEventSource::stop()
{
  this->Internal->ShouldStop = 1;
  this->Internal->wait();
}

bool pqThreadedEventSource::waitForGUI()
{
  this->Internal->Waiting = 1;

  while (this->Internal->Waiting == 1 && this->Internal->ShouldStop == 0)
  {
    pqInternal::ThreadHelper::msleep(50);
  }

  this->Internal->Waiting = 0;

  return !this->Internal->ShouldStop;
}

void pqThreadedEventSource::guiAcknowledge()
{
  while (this->Internal->Waiting == 0)
  {
    pqInternal::ThreadHelper::msleep(50);
  }

  this->Internal->Waiting = 0;
}

void pqThreadedEventSource::msleep(int msec)
{
  pqInternal::ThreadHelper::msleep(msec);
}

void pqThreadedEventSource::done(int success)
{
  if (success == 0)
  {
    this->postNextEvent(QString(), QString(), QString());
    return;
  }
  this->postNextEvent(QString(), QString(), "failure");
}
