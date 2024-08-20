// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqAbstractBooleanEventPlayer_h
#define _pqAbstractBooleanEventPlayer_h

#include "pqWidgetEventPlayer.h"

/**
Concrete implementation of pqWidgetEventPlayer that translates high-level ParaView events into
low-level Qt events.

\sa pqEventPlayer
*/
class QTTESTING_EXPORT pqAbstractBooleanEventPlayer : public pqWidgetEventPlayer
{
  Q_OBJECT
  typedef pqWidgetEventPlayer Superclass;

public:
  pqAbstractBooleanEventPlayer(QObject* p = 0);

  using Superclass::playEvent;
  bool playEvent(
    QObject* Object, const QString& Command, const QString& Arguments, bool& Error) override;

private:
  pqAbstractBooleanEventPlayer(const pqAbstractBooleanEventPlayer&);
  pqAbstractBooleanEventPlayer& operator=(const pqAbstractBooleanEventPlayer&);
};

#endif // !_pqAbstractBooleanEventPlayer_h
