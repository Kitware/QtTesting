// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#ifndef __pqTableViewEventPlayer_h
#define __pqTableViewEventPlayer_h

#include "pqAbstractItemViewEventPlayerBase.h"

/// pqTableViewEventPlayer is a player for QTableWidget. Plays back the state
/// recorded using pqTableViewEventTranslator.
class QTTESTING_EXPORT pqTableViewEventPlayer : public pqAbstractItemViewEventPlayerBase
{
  Q_OBJECT
  typedef pqAbstractItemViewEventPlayerBase Superclass;

public:
  pqTableViewEventPlayer(QObject* parent = 0);
  ~pqTableViewEventPlayer() override;

  using Superclass::playEvent;
  bool playEvent(QObject* object, const QString& command, const QString& arguments, int eventType,
    bool& error) override;

private:
  pqTableViewEventPlayer(const pqTableViewEventPlayer&); // Not implemented.
  void operator=(const pqTableViewEventPlayer&);         // Not implemented.
};

#endif
