// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef __pqCommentEventPlayer_h
#define __pqCommentEventPlayer_h

// QtTesting includes
#include "pqTestUtility.h"
#include "pqWidgetEventPlayer.h"

/// This class is a comment class.
/// Do no action on Object.
/// Emit comment wrote in the xml, which can be catch by someone else.

class QTTESTING_EXPORT pqCommentEventPlayer : public pqWidgetEventPlayer
{
  Q_OBJECT
  typedef pqWidgetEventPlayer Superclass;

public:
  pqCommentEventPlayer(pqTestUtility* testUtility, QObject* p = 0);
  ~pqCommentEventPlayer() override;

  using Superclass::playEvent;
  bool playEvent(
    QObject* Object, const QString& Command, const QString& Arguments, bool& Error) override;

Q_SIGNALS:
  void comment(const QString&);

private:
  pqCommentEventPlayer(const pqCommentEventPlayer&);            // Not implemented
  pqCommentEventPlayer& operator=(const pqCommentEventPlayer&); // Not implemented

  pqTestUtility* TestUtility;
};

#endif // __pqCommentEventPlayer_h
