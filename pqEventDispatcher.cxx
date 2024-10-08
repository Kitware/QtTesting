// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqEventDispatcher.h"

#include "pqEventPlayer.h"
#include "pqEventSource.h"

#include <QAbstractEventDispatcher>
#include <QApplication>
#include <QDialog>
#include <QEventLoop>
#include <QList>
#include <QMainWindow>
#include <QPointer>
#include <QThread>
#include <QTimer>
#include <QtDebug>

#include <iostream>
using namespace std;

//-----------------------------------------------------------------------------
namespace
{
static QList<QPointer<QTimer>> RegisteredTimers;

void processTimers()
{
  Q_FOREACH (QTimer* timer, RegisteredTimers)
  {
    if (timer && timer->isActive())
    {
      QTimerEvent event(timer->timerId());
      qApp->notify(timer, &event);
    }
  }
}

static int EventPlaybackDelay = QT_TESTING_EVENT_PLAYBACK_DELAY;
};

bool pqEventDispatcher::DeferMenuTimeouts = false;
bool pqEventDispatcher::DeferEventsIfBlocked = false;
bool pqEventDispatcher::PlayingBlockingEvent = false;

//-----------------------------------------------------------------------------
pqEventDispatcher::pqEventDispatcher(QObject* parentObject)
  : Superclass(parentObject)
{
  this->ActiveSource = NULL;
  this->ActivePlayer = NULL;

  this->PlayBackStatus = true;
  this->PlayBackFinished = false;
  this->PlayBackPaused = false;
  this->PlayBackOneStep = false;
  this->PlayBackStoped = false;

#ifdef __APPLE__
  this->BlockTimer.setInterval(1000);
#else
  this->BlockTimer.setInterval(100);
#endif
  this->BlockTimer.setSingleShot(true);
  QObject::connect(&this->BlockTimer, SIGNAL(timeout()), this, SLOT(playEventOnBlocking()));
}

//-----------------------------------------------------------------------------
pqEventDispatcher::~pqEventDispatcher() {}

//-----------------------------------------------------------------------------
void pqEventDispatcher::setEventPlaybackDelay(int milliseconds)
{
  EventPlaybackDelay = (milliseconds <= 0) ? 0 : milliseconds;
}

//-----------------------------------------------------------------------------
int pqEventDispatcher::eventPlaybackDelay()
{
  return EventPlaybackDelay;
}

//-----------------------------------------------------------------------------
void pqEventDispatcher::registerTimer(QTimer* timer)
{
  if (timer)
  {
    RegisteredTimers.push_back(timer);
  }
}

//-----------------------------------------------------------------------------
void pqEventDispatcher::deferEventsIfBlocked(bool enable)
{
  pqEventDispatcher::DeferEventsIfBlocked = enable;
}

//-----------------------------------------------------------------------------
void pqEventDispatcher::aboutToBlock()
{
  // if (!pqEventDispatcher::DeferMenuTimeouts)
  {
    if (!this->BlockTimer.isActive())
    {
      // cout << "aboutToBlock" << endl;
      // Request a delayed playback for an event.
      this->BlockTimer.start();
    }
  }
}

//-----------------------------------------------------------------------------
void pqEventDispatcher::awake()
{
  // if (!pqEventDispatcher::DeferMenuTimeouts)
  //  {
  //  // cout << "awake" << endl;
  //  // this->BlockTimer.stop();
  //  }
}

//-----------------------------------------------------------------------------
void pqEventDispatcher::setTimeStep(int value)
{
  EventPlaybackDelay = value;
}

//-----------------------------------------------------------------------------
void pqEventDispatcher::run(bool value)
{
  this->PlayBackPaused = !value;
  if (value)
  {
    Q_EMIT this->restarted();
  }
  else
  {
    Q_EMIT this->paused();
  }
}

//-----------------------------------------------------------------------------
void pqEventDispatcher::stop()
{
  this->PlayBackPaused = false;
  this->PlayBackFinished = true;
}

//-----------------------------------------------------------------------------
bool pqEventDispatcher::isPaused() const
{
  return this->PlayBackPaused;
}

//-----------------------------------------------------------------------------
bool pqEventDispatcher::status() const
{
  return this->PlayBackStatus;
}

//-----------------------------------------------------------------------------
void pqEventDispatcher::oneStep()
{
  this->PlayBackOneStep = true;
}

//-----------------------------------------------------------------------------
bool pqEventDispatcher::playEvents(pqEventSource& source, pqEventPlayer& player)
{
  if (this->ActiveSource || this->ActivePlayer)
  {
    qCritical() << "Event dispatcher is already playing";
    return false;
  }

  this->ActiveSource = &source;
  this->ActivePlayer = &player;

  QApplication::setEffectEnabled(Qt::UI_General, false);
  QApplication::setEffectEnabled(Qt::UI_AnimateMenu, false);    // Show animated menus.
  QApplication::setEffectEnabled(Qt::UI_FadeMenu, false);       //	Show faded menus.
  QApplication::setEffectEnabled(Qt::UI_AnimateCombo, false);   // Show animated comboboxes.
  QApplication::setEffectEnabled(Qt::UI_AnimateTooltip, false); //	Show tooltip animations.
  QApplication::setEffectEnabled(Qt::UI_FadeTooltip, false);    // Show tooltip fading effects.

  QObject::connect(
    QAbstractEventDispatcher::instance(), SIGNAL(aboutToBlock()), this, SLOT(aboutToBlock()));
  QObject::connect(QAbstractEventDispatcher::instance(), SIGNAL(awake()), this, SLOT(awake()));

  // This is how the playback logic works:
  // * In here, we continuously keep on playing one event after another until
  //   we are done processing all the events.
  // * If a modal dialog pops up, then this->aboutToBlock() gets called. To me
  //   accurate, aboutToBlock() is called everytime the sub-event loop is entered
  //   and more modal dialogs that loop is entered after processing of each event
  //   (not merely when the dialog pops up).
  // * In this->aboutToBlock() we start a timer which on timeout processes just 1 event.
  // * After executing that event, if the dialog still is up, them aboutToBlock() will
  //   be called again and the cycle continues. If not, the control returns to this main
  //   playback loop, and it continues.
  this->PlayBackStatus = true; // success.
  this->PlayBackFinished = false;
  while (!this->PlayBackFinished)
  {
    if (!this->PlayBackPaused)
    {
      // cerr << "=== playEvent(1) ===" << endl;
      this->playEvent();
    }
    else
    {
      if (this->PlayBackOneStep)
      {
        this->PlayBackOneStep = false;
        this->playEvent();
      }
      else
      {
        this->processEventsAndWait(100);
      }
    }
  }
  this->ActiveSource = NULL;
  this->ActivePlayer = NULL;

  QObject::disconnect(
    QAbstractEventDispatcher::instance(), SIGNAL(aboutToBlock()), this, SLOT(aboutToBlock()));
  QObject::disconnect(QAbstractEventDispatcher::instance(), SIGNAL(awake()), this, SLOT(awake()));

  return this->PlayBackStatus;
}

//-----------------------------------------------------------------------------
void pqEventDispatcher::playEventOnBlocking()
{
  //  if(this->PlayingBlockingEvent)
  //    {
  //    qDebug() << "Event blocking already playing ....";
  //    return;
  //    }

  if (pqEventDispatcher::DeferMenuTimeouts || pqEventDispatcher::DeferEventsIfBlocked)
  {
    // cerr << "=== playEventOnBlocking ===" << endl;
    this->BlockTimer.start();
    return;
  }

  pqEventDispatcher::PlayingBlockingEvent = true;
  // cout << "---blocked event: " << endl;
  // if needed for debugging, I can print blocking annotation here.
  // cerr << "=== playEvent(2) ===" << endl;
  this->playEvent(1);

  // if (!this->BlockTimer.isActive())
  //  {
  //  this->BlockTimer.start();
  //  }
  pqEventDispatcher::PlayingBlockingEvent = false;
}

//-----------------------------------------------------------------------------
void pqEventDispatcher::playEvent(int indent)
{
  this->BlockTimer.stop();
  if (this->PlayBackFinished)
  {
    return;
  }

  if (!this->ActiveSource)
  {
    this->PlayBackFinished = true;
    this->PlayBackStatus = false; // failure.
    qCritical("Internal error: playEvent called without a valid event source.");
    return;
  }

  QString object;
  QString command;
  QString arguments;
  int eventType;

  int result = this->ActiveSource->getNextEvent(object, command, arguments, eventType);
  if (result == pqEventSource::DONE)
  {
    this->PlayBackFinished = true;
    return;
  }
  else if (result == pqEventSource::FAILURE)
  {
    this->PlayBackFinished = true;
    this->PlayBackStatus = false; // failure.
    return;
  }

  static unsigned long counter = 0;
  unsigned long local_counter = counter++;
  QString pretty_name = object.mid(object.lastIndexOf('/'));
  bool print_debug = getenv("PV_DEBUG_TEST") != NULL;
  if (print_debug)
  {
    QString eventString = "Event";
    if (eventType == pqEventTypes::CHECK_EVENT)
    {
      eventString = "Check Event";
    }

    cout << QTime::currentTime().toString("hh:mm:ss").toStdString().c_str() << " : "
         << QString().fill(' ', 4 * indent).toStdString().c_str() << local_counter << ": Test ("
         << indent << "): " << eventString.toUtf8().data() << ": "
         << pretty_name.toStdString().c_str() << ": " << command.toStdString().c_str() << " : "
         << arguments.toStdString().c_str() << endl;
  }

  bool error = false;
  this->ActivePlayer->playEvent(object, command, arguments, eventType, error);
  this->BlockTimer.stop();

  // process any posted events. We call processEvents() so that any slots
  // connected using QueuedConnection also get handled.
  this->processEventsAndWait(EventPlaybackDelay);

  // We explicitly timeout timers that have been registered with us.
  processTimers();

  this->BlockTimer.stop();

  if (print_debug)
  {
    cout << QTime::currentTime().toString("hh:mm:ss").toStdString().c_str() << " : "
         << QString().fill(' ', 4 * indent).toStdString().c_str() << local_counter << ": Done"
         << endl;
  }

  if (error)
  {
    this->PlayBackStatus = false;
    this->PlayBackFinished = true;
    return;
  }
}

//-----------------------------------------------------------------------------
void pqEventDispatcher::processEventsAndWait(int ms)
{
  bool prev = pqEventDispatcher::DeferMenuTimeouts;
  pqEventDispatcher::DeferMenuTimeouts = true;
  if (ms > 0)
  {
    QApplication::sendPostedEvents();
    QEventLoop loop;
    QTimer::singleShot(ms, &loop, SLOT(quit()));
    loop.exec();
  }
  // When this gets called during playback from a blocking event loop (e.g. a modal dialog)
  // calling `QApplication::processEvents()` has a sideeffect on Qt 5 + OsX where it does
  // not quit the eventloop for the modal dialog until a mouse event (for example) is
  // received by the application. Avoiding calling QApplication::processEvents when already
  // processing a event loop other the apps main event loop avoids that problem.
  if (!pqEventDispatcher::PlayingBlockingEvent)
  {
    QApplication::processEvents();
  }
  QApplication::sendPostedEvents();
  if (!pqEventDispatcher::PlayingBlockingEvent)
  {
    QApplication::processEvents();
  }
  pqEventDispatcher::DeferMenuTimeouts = prev;
}

//-----------------------------------------------------------------------------
void pqEventDispatcher::processEvents(QEventLoop::ProcessEventsFlags flags)
{
  bool prev = pqEventDispatcher::DeferMenuTimeouts;
  pqEventDispatcher::DeferMenuTimeouts = true;
  QApplication::processEvents(flags);
  pqEventDispatcher::DeferMenuTimeouts = prev;
}
