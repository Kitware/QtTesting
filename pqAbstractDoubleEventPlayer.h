// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqAbstractDoubleEventPlayer_h
#define _pqAbstractDoubleEventPlayer_h

#include "pqWidgetEventPlayer.h"

/**
Concrete implementation of pqWidgetEventPlayer that translates high-level ParaView events into
low-level Qt events.

\sa pqEventPlayer
*/

class QTTESTING_EXPORT pqAbstractDoubleEventPlayer : public pqWidgetEventPlayer
{
  Q_OBJECT
  typedef pqWidgetEventPlayer Superclass;

public:
  pqAbstractDoubleEventPlayer(QObject* p = 0);

  using Superclass::playEvent;
  bool playEvent(
    QObject* Object, const QString& Command, const QString& Arguments, bool& Error) override;

private:
  pqAbstractDoubleEventPlayer(const pqAbstractDoubleEventPlayer&);
  pqAbstractDoubleEventPlayer& operator=(const pqAbstractDoubleEventPlayer&);
};

#endif // !_pqAbstractDoubleEventPlayer_h
