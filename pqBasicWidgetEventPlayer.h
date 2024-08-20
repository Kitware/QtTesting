// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqBasicWidgetEventPlayer_h
#define _pqBasicWidgetEventPlayer_h

#include "pqWidgetEventPlayer.h"

/**
Concrete implementation of pqWidgetEventPlayer that handles playback of "activate" events for
buttons and menus.

\sa pqEventPlayer
*/
class pqBasicWidgetEventPlayer : public pqWidgetEventPlayer
{
  Q_OBJECT
  typedef pqWidgetEventPlayer Superclass;

public:
  pqBasicWidgetEventPlayer(QObject* p = 0);

  using Superclass::playEvent;
  bool playEvent(QObject* object, const QString& command, const QString& arguments, int eventType,
    bool& error) override;

private:
  pqBasicWidgetEventPlayer(const pqBasicWidgetEventPlayer&);
  pqBasicWidgetEventPlayer& operator=(const pqBasicWidgetEventPlayer&);
};

#endif // !_pqBasicWidgetEventPlayer_h
