// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqTabBarEventPlayer_h
#define _pqTabBarEventPlayer_h

#include "pqWidgetEventPlayer.h"

/**
Concrete implementation of pqWidgetEventPlayer that translates high-level ParaView events into
low-level Qt events.

\sa pqEventPlayer
*/

class QTTESTING_EXPORT pqTabBarEventPlayer : public pqWidgetEventPlayer
{
  Q_OBJECT
  typedef pqWidgetEventPlayer Superclass;

public:
  pqTabBarEventPlayer(QObject* p = 0);

  using Superclass::playEvent;
  bool playEvent(
    QObject* Object, const QString& Command, const QString& Arguments, bool& Error) override;

private:
  pqTabBarEventPlayer(const pqTabBarEventPlayer&);
  pqTabBarEventPlayer& operator=(const pqTabBarEventPlayer&);
};

#endif // !_pqTabBarEventPlayer_h
