// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#include "pqTableViewEventPlayer.h"
#include <QTableView>

//-----------------------------------------------------------------------------
pqTableViewEventPlayer::pqTableViewEventPlayer(QObject* parentObject)
  : Superclass(parentObject)
{
}

//-----------------------------------------------------------------------------
pqTableViewEventPlayer::~pqTableViewEventPlayer() {}

//-----------------------------------------------------------------------------0000000
bool pqTableViewEventPlayer::playEvent(
  QObject* object, const QString& command, const QString& arguments, int eventType, bool& error)
{
  QTableView* tableView = qobject_cast<QTableView*>(object);
  if (!tableView)
  {
    // mouse events go to the viewport widget
    tableView = qobject_cast<QTableView*>(object->parent());
  }
  if (!tableView)
  {
    return false;
  }
  return this->Superclass::playEvent(object, command, arguments, eventType, error);
}
