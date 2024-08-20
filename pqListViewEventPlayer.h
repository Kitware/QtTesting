// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#ifndef __pqListViewEventPlayer_h
#define __pqListViewEventPlayer_h

#include "pqAbstractItemViewEventPlayerBase.h"

/// pqListViewEventPlayer is a player for QTableWidget. Plays back the state
/// recorded using pqListViewEventTranslator.
class QTTESTING_EXPORT pqListViewEventPlayer : public pqAbstractItemViewEventPlayerBase
{
  Q_OBJECT
  typedef pqAbstractItemViewEventPlayerBase Superclass;

public:
  pqListViewEventPlayer(QObject* parent = 0);
  ~pqListViewEventPlayer() override;

  using Superclass::playEvent;
  bool playEvent(QObject* object, const QString& command, const QString& arguments, int eventType,
    bool& error) override;

private:
  pqListViewEventPlayer(const pqListViewEventPlayer&); // Not implemented.
  void operator=(const pqListViewEventPlayer&);        // Not implemented.
};

#endif
