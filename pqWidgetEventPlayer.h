// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqWidgetEventPlayer_h
#define _pqWidgetEventPlayer_h

#include "QtTestingExport.h"
#include <QObject>

class QString;

/**
Abstract interface for an object that can playback high-level
ParaView events by translating them into low-level Qt events,
for test-cases, demos, tutorials, etc.

\sa pqEventPlayer
*/

class QTTESTING_EXPORT pqWidgetEventPlayer : public QObject
{
  Q_OBJECT

public:
  pqWidgetEventPlayer(QObject* p);
  ~pqWidgetEventPlayer() override;

  /** Derivatives should implement this and play-back the given command,
  returning "true" if they handled the command, and setting Error
  to "true" if there were any problems. */
  virtual bool playEvent(
    QObject* object, const QString& command, const QString& arguments, bool& error);
  virtual bool playEvent(
    QObject* object, const QString& command, const QString& arguments, int eventType, bool& error);
};

#endif // !_pqWidgetEventPlayer_h
