// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqAbstractStringEventPlayer_h
#define _pqAbstractStringEventPlayer_h

#include "QtTestingExport.h"
#include "pqWidgetEventPlayer.h"

/**
Concrete implementation of pqWidgetEventPlayer that translates high-level ParaView events into
low-level Qt events.

\sa pqEventPlayer
*/

class QTTESTING_EXPORT pqAbstractStringEventPlayer : public pqWidgetEventPlayer
{
  Q_OBJECT
  typedef pqWidgetEventPlayer Superclass;

public:
  pqAbstractStringEventPlayer(QObject* p = 0);

  using Superclass::playEvent;
  bool playEvent(
    QObject* Object, const QString& Command, const QString& Arguments, bool& Error) override;

private:
  pqAbstractStringEventPlayer(const pqAbstractStringEventPlayer&);
  pqAbstractStringEventPlayer& operator=(const pqAbstractStringEventPlayer&);
};

#endif // !_pqAbstractStringEventPlayer_h
