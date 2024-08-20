// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqAbstractItemViewEventPlayer_h
#define _pqAbstractItemViewEventPlayer_h

#include "pqWidgetEventPlayer.h"

/**
Concrete implementation of pqWidgetEventPlayer that translates high-level ParaView events into
low-level Qt events.

\sa pqEventPlayer
*/

class QTTESTING_EXPORT pqAbstractItemViewEventPlayer : public pqWidgetEventPlayer
{
  Q_OBJECT
  typedef pqWidgetEventPlayer Superclass;

public:
  pqAbstractItemViewEventPlayer(QObject* p = 0);

  using Superclass::playEvent;
  bool playEvent(
    QObject* Object, const QString& Command, const QString& Arguments, bool& Error) override;

private:
  pqAbstractItemViewEventPlayer(const pqAbstractItemViewEventPlayer&);
  pqAbstractItemViewEventPlayer& operator=(const pqAbstractItemViewEventPlayer&);
};

#endif // !_pqAbstractItemViewEventPlayer_h
