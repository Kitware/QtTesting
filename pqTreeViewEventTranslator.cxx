// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqTreeViewEventTranslator.h"

#include <QHeaderView>
#include <QTreeView>

//-----------------------------------------------------------------------------
pqTreeViewEventTranslator::pqTreeViewEventTranslator(QObject* parentObject)
  : Superclass(parentObject)
{
}

//-----------------------------------------------------------------------------
pqTreeViewEventTranslator::~pqTreeViewEventTranslator() {}

//-----------------------------------------------------------------------------
void pqTreeViewEventTranslator::monitorSignalsInternal(QAbstractItemView* abstractItemView)
{
  QObject::connect(abstractItemView, SIGNAL(expanded(const QModelIndex&)), this,
    SLOT(onExpanded(const QModelIndex&)));
  QObject::connect(abstractItemView, SIGNAL(collapsed(const QModelIndex&)), this,
    SLOT(onCollapsed(const QModelIndex&)));
}

//-----------------------------------------------------------------------------
void pqTreeViewEventTranslator::onExpanded(const QModelIndex& index)
{
  QTreeView* treeView = qobject_cast<QTreeView*>(this->sender());

  // record the check state change if the item is user-checkable.
  Q_EMIT this->recordEvent(treeView, "expand", this->getIndexAsString(index));
}

//-----------------------------------------------------------------------------
void pqTreeViewEventTranslator::onCollapsed(const QModelIndex& index)
{
  QTreeView* treeView = qobject_cast<QTreeView*>(this->sender());

  // record the check state change if the item is user-checkable.
  Q_EMIT this->recordEvent(treeView, "collapse", this->getIndexAsString(index));
}

//-----------------------------------------------------------------------------
void pqTreeViewEventTranslator::onEnteredCheck(const QModelIndex& item)
{
  // Recover treeview
  QTreeView* treeView = qobject_cast<QTreeView*>(this->AbstractItemView);

  // Recover a visual rectangle corresponding to the item
  QRect visualRect = treeView->visualRect(item);

  // Translating it of margins, as they are missing from visual rect
  visualRect.translate(treeView->contentsMargins().left(), treeView->contentsMargins().top());

  // As well as header
  visualRect.translate(0, treeView->header()->height());

  // store item and signal that a specific overlay is ready to be drawn
  this->ModelItemCheck = &item;
  Q_EMIT this->specificOverlay(visualRect);
}

//-----------------------------------------------------------------------------
QAbstractItemView* pqTreeViewEventTranslator::findCorrectedAbstractItemView(QObject* object) const
{
  // Ignore QHeaderView event specifically
  if (qobject_cast<QHeaderView*>(object))
  {
    return NULL;
  }

  QAbstractItemView* abstractItemView = qobject_cast<QTreeView*>(object);
  if (!abstractItemView)
  {
    // mouse events go to the viewport widget
    abstractItemView = qobject_cast<QTreeView*>(object->parent());
  }
  return abstractItemView;
}
