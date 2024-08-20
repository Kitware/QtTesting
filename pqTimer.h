// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#ifndef __pqTimer_h
#define __pqTimer_h

#include "QtTestingExport.h"
#include <QTimer>

/// pqTimer is a extension for QTimer which ensures that the timer is registered
/// with the pqEventDispatcher. Register timers with pqEventDispatcher ensures
/// that when tests are being played back, the timer will be ensured to have
/// timed out if active after every step in the playback. This provides a means
/// to reproduce the timer behaviour in the real world, during test playback.
class QTTESTING_EXPORT pqTimer : public QTimer
{
  Q_OBJECT
  typedef QTimer Superclass;

public:
  pqTimer(QObject* parent = 0);
  ~pqTimer() override;

  /// This static function calls a slot after a given time interval.
  static void singleShot(int msec, QObject* receiver, const char* member);

protected:
  /// overridden to support blocking timer events in future (current
  /// implementation merely forwards to superclass).
  void timerEvent(QTimerEvent* evt) override;

private:
  Q_DISABLE_COPY(pqTimer)
};

#endif
