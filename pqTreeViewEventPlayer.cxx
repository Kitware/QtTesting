// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#include "pqTreeViewEventPlayer.h"
#include <QDebug>
#include <QMenu>
#include <QTreeWidget>

//-----------------------------------------------------------------------------
pqTreeViewEventPlayer::pqTreeViewEventPlayer(QObject* parentObject)
  : Superclass(parentObject)
{
}

//-----------------------------------------------------------------------------
pqTreeViewEventPlayer::~pqTreeViewEventPlayer() {}

//-----------------------------------------------------------------------------
bool pqTreeViewEventPlayer::playEvent(
  QObject* object, const QString& command, const QString& arguments, int eventType, bool& error)
{
  QTreeView* treeView = qobject_cast<QTreeView*>(object);
  QMenu* contextMenu = qobject_cast<QMenu*>(object);
  // if this a QMenu (potentially a context menu of the view),
  // we should not move onto parent
  if (!treeView && !contextMenu)
  {
    // mouse events go to the viewport widget
    treeView = qobject_cast<QTreeView*>(object->parent());
  }
  if (!treeView)
  {
    return false;
  }

  QRegularExpression regExp0("^([\\d\\.]+),(\\d+),(\\d+)$");
  QRegularExpressionMatch match = regExp0.match(arguments);
  if (command == "setTreeItemCheckState" && match.hasMatch())
  {
    // legacy command recorded from tree widgets.
    QTreeWidget* treeWidget = qobject_cast<QTreeWidget*>(object);
    if (!treeWidget)
    {
      return false;
    }
    QString str_index = match.captured(1);
    int column = match.captured(2).toInt();
    int check_state = match.captured(3).toInt();
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    QStringList indices = str_index.split(".", Qt::SkipEmptyParts);
#else
    QStringList indices = str_index.split(".", QString::SkipEmptyParts);
#endif
    QTreeWidgetItem* cur_item = NULL;
    Q_FOREACH (QString cur_index, indices)
    {
      int index = cur_index.toInt();
      if (!cur_item)
      {
        cur_item = treeWidget->topLevelItem(index);
      }
      else
      {
        cur_item = cur_item->child(index);
      }
      if (!cur_item)
      {
        error = true;
        qCritical() << "ERROR: Tree widget must have changed. "
                    << "Indices recorded in the test are no longer valid. Cannot playback.";
        return true;
      }
    }
    cur_item->setCheckState(column, static_cast<Qt::CheckState>(check_state));
    return true;
  }

  if (command == "expand" || command == "collapse")
  {
    QString str_index = arguments;
    QModelIndex index = pqTreeViewEventPlayer::GetIndex(str_index, treeView, error);
    if (error)
    {
      return true;
    }
    treeView->setExpanded(index, (command == "expand"));
    return true;
  }
  return this->Superclass::playEvent(object, command, arguments, eventType, error);
}
