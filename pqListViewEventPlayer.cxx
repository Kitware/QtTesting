// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#include "pqListViewEventPlayer.h"
#include <QListView>

//-----------------------------------------------------------------------------
pqListViewEventPlayer::pqListViewEventPlayer(QObject* parentObject)
  : Superclass(parentObject)
{
}

//-----------------------------------------------------------------------------
pqListViewEventPlayer::~pqListViewEventPlayer() {}

//-----------------------------------------------------------------------------0000000
bool pqListViewEventPlayer::playEvent(
  QObject* object, const QString& command, const QString& arguments, int eventType, bool& error)
{
  QListView* listView = qobject_cast<QListView*>(object);
  if (!listView)
  {
    // mouse events go to the viewport widget
    listView = qobject_cast<QListView*>(object->parent());
  }
  if (!listView)
  {
    return false;
  }
  return this->Superclass::playEvent(object, command, arguments, eventType, error);
}
