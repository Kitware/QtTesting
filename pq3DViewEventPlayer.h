// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#ifndef _pq3DViewEventPlayer_h
#define _pq3DViewEventPlayer_h

#include "pqWidgetEventPlayer.h"

/**
Concrete implementation of pqWidgetEventPlayer that handles playback of "activate" events for 3d
views.
It is not registered by default, and user can register with their own 3d view type.

\sa pqEventPlayer
*/
class QTTESTING_EXPORT pq3DViewEventPlayer : public pqWidgetEventPlayer
{
  Q_OBJECT
  typedef pqWidgetEventPlayer Superclass;

public:
  pq3DViewEventPlayer(const QByteArray& classname, QObject* p = 0);

  using Superclass::playEvent;
  bool playEvent(
    QObject* Object, const QString& Command, const QString& Arguments, bool& Error) override;

protected:
  QByteArray mClassType;

private:
  pq3DViewEventPlayer(const pq3DViewEventPlayer&);
  pq3DViewEventPlayer& operator=(const pq3DViewEventPlayer&);
};

#endif // !_pq3DViewEventPlayer_h
