// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqThreadedEventSource_h
#define _pqThreadedEventSource_h

#include "QtTestingExport.h"
#include "pqEventSource.h"

class QString;
class QMutex;

/// Abstract interface for objects that can supply high-level testing events
/// on a separte thread.  This class is derived from, and run() is
/// implemented.
class QTTESTING_EXPORT pqThreadedEventSource : public pqEventSource
{
  Q_OBJECT
public:
  pqThreadedEventSource(QObject* p);
  ~pqThreadedEventSource() override;

  using pqEventSource::getNextEvent;
  /** Called by the dispatcher on the GUI thread.
    Retrieves the next available event.  Returns true if an event was
    returned, false if there are no more events.
    In the case of a threaded event source, this function is called by the GUI
    thread and waits for the other thread to pos and event. */
  virtual int getNextEvent(QString& object, QString& command, QString& arguments);

  /** The testing thread may post an event for the GUI to process.
      This function blocks until there are no previously queued
      events to play.
      If the event plays successfully, true is returned. */
  bool postNextEvent(const QString& object, const QString& command, const QString& argument);

  /** tell this source to stop */
  void stop() override;

  /** Wait for the GUI thread to acknowledge an event.
      A previously locked mutex must be passed in.
      For use by the testing thread.
      If return value is false, an error occurred and
      the testing thread should terminate. */
  bool waitForGUI();

  /** Give the testing thread an acknowledgement.
      For use by the GUI thread */
  void guiAcknowledge();

  // helper method to sleep.
  static void msleep(int msecs);

private Q_SLOTS:

  void relayEvent(QString object, QString command, QString arguments);

protected:
  // start the thread
  virtual void start();

  // called by the testing thread to signify it is done
  // pass in zero for success, non-zero for failure
  void done(int);

  // run the thread, return
  virtual void run() = 0;

private:
  class pqInternal;
  pqInternal* Internal;
  friend class pqInternal;
};

#endif // !_pqThreadedEventSource_h
