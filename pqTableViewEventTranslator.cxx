// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#include "pqTableViewEventTranslator.h"

#include <QHeaderView>
#include <QTableView>
//-----------------------------------------------------------------------------
pqTableViewEventTranslator::pqTableViewEventTranslator(QObject* parentObject)
  : Superclass(parentObject)
{
}

//-----------------------------------------------------------------------------
pqTableViewEventTranslator::~pqTableViewEventTranslator() {}

//-----------------------------------------------------------------------------
void pqTableViewEventTranslator::onEnteredCheck(const QModelIndex& item)
{
  // Recover Table View
  QTableView* tableView = qobject_cast<QTableView*>(this->AbstractItemView);

  // Get a visual rectangle of the item
  QRect visualRect = tableView->visualRect(item);

  // Translate the rect of margins and headers
  visualRect.translate(tableView->contentsMargins().left(), tableView->contentsMargins().top());
  visualRect.translate(
    tableView->verticalHeader()->width(), tableView->horizontalHeader()->height());

  // Stor item and signal that a specific overlay is ready to be drawn
  this->ModelItemCheck = &item;
  Q_EMIT this->specificOverlay(visualRect);
}

//-----------------------------------------------------------------------------
QAbstractItemView* pqTableViewEventTranslator::findCorrectedAbstractItemView(QObject* object) const
{
  // Ignore QHeaderView event specifically
  if (qobject_cast<QHeaderView*>(object))
  {
    return NULL;
  }

  QAbstractItemView* abstractItemView = qobject_cast<QTableView*>(object);
  if (!abstractItemView)
  {
    // mouse events go to the viewport widget
    abstractItemView = qobject_cast<QTableView*>(object->parent());
  }
  return abstractItemView;
}
