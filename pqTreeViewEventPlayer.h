// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#ifndef __pqTreeViewEventPlayer_h
#define __pqTreeViewEventPlayer_h

#include "pqAbstractItemViewEventPlayerBase.h"

/// pqTreeViewEventPlayer is a player for QTreeWidget. Plays back the state
/// recorded using pqTreeViewEventTranslator.
class QTTESTING_EXPORT pqTreeViewEventPlayer : public pqAbstractItemViewEventPlayerBase
{
  Q_OBJECT
  typedef pqAbstractItemViewEventPlayerBase Superclass;

public:
  pqTreeViewEventPlayer(QObject* parent = 0);
  ~pqTreeViewEventPlayer() override;

  using Superclass::playEvent;
  bool playEvent(QObject* object, const QString& command, const QString& arguments, int eventType,
    bool& error) override;

private:
  pqTreeViewEventPlayer(const pqTreeViewEventPlayer&); // Not implemented.
  void operator=(const pqTreeViewEventPlayer&);        // Not implemented.
};

#endif
