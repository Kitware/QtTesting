// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqAbstractActivateEventPlayer_h
#define _pqAbstractActivateEventPlayer_h

#include "pqWidgetEventPlayer.h"
class QAction;
class QMenuBar;
class QMenu;

/**
Concrete implementation of pqWidgetEventPlayer that handles playback of "activate" events for
buttons and menus.

\sa pqEventPlayer
*/
class QTTESTING_EXPORT pqAbstractActivateEventPlayer : public pqWidgetEventPlayer
{
  Q_OBJECT
  typedef pqWidgetEventPlayer Superclass;

public:
  pqAbstractActivateEventPlayer(QObject* p = 0);

  using Superclass::playEvent;
  bool playEvent(
    QObject* Object, const QString& Command, const QString& Arguments, bool& Error) override;

private:
  QAction* findAction(QMenu* p, const QString& name);
  QAction* findAction(QMenuBar* p, const QString& name);

  pqAbstractActivateEventPlayer(const pqAbstractActivateEventPlayer&);
  pqAbstractActivateEventPlayer& operator=(const pqAbstractActivateEventPlayer&);
};

#endif // !_pqAbstractActivateEventPlayer_h
