// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#include "pqListViewEventTranslator.h"

#include <QHeaderView>
#include <QListView>
//-----------------------------------------------------------------------------
pqListViewEventTranslator::pqListViewEventTranslator(QObject* parentObject)
  : Superclass(parentObject)
{
}

//-----------------------------------------------------------------------------
pqListViewEventTranslator::~pqListViewEventTranslator() {}

//-----------------------------------------------------------------------------
void pqListViewEventTranslator::onEnteredCheck(const QModelIndex& item)
{
  // Recover list View
  QListView* listView = qobject_cast<QListView*>(this->AbstractItemView);

  // Get a visual rectangle of the item
  QRect visualRect = listView->visualRect(item);

  // Translate the rect of margins and headers
  visualRect.translate(listView->contentsMargins().left(), listView->contentsMargins().top());

  // Stor item and signal that a specific overlay is ready to be drawn
  this->ModelItemCheck = &item;
  Q_EMIT this->specificOverlay(visualRect);
}

//-----------------------------------------------------------------------------
QAbstractItemView* pqListViewEventTranslator::findCorrectedAbstractItemView(QObject* object) const
{
  // Ignore QHeaderView event specifically
  if (qobject_cast<QHeaderView*>(object))
  {
    return NULL;
  }

  QAbstractItemView* abstractItemView = qobject_cast<QListView*>(object);
  if (!abstractItemView)
  {
    // mouse events go to the viewport widget
    abstractItemView = qobject_cast<QListView*>(object->parent());
  }
  return abstractItemView;
}
