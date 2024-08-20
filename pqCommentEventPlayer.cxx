// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqCommentEventPlayer.h"

#include <QtGlobal>

// ----------------------------------------------------------------------------
pqCommentEventPlayer::pqCommentEventPlayer(pqTestUtility* testUtility, QObject* parent)
  : pqWidgetEventPlayer(parent)
{
  this->TestUtility = testUtility;
}

// ----------------------------------------------------------------------------
pqCommentEventPlayer::~pqCommentEventPlayer()
{
  this->TestUtility = 0;
}

// ----------------------------------------------------------------------------
bool pqCommentEventPlayer::playEvent(
  QObject* Object, const QString& Command, const QString& Arguments, bool& Error)
{
  Q_UNUSED(Object);
  Q_UNUSED(Error);
  if (!Command.startsWith("comment"))
  {
    return false;
  }

  if (!Arguments.isEmpty())
  {
    Q_EMIT this->comment(Arguments);
  }

  if (Command.split("-").contains("block"))
  {
    this->TestUtility->dispatcher()->run(false);
  }

  return true;
}
