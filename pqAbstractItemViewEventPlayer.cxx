// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqAbstractItemViewEventPlayer.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QHeaderView>
#include <QKeyEvent>
#include <QList>
#include <QListWidget>
#include <QMenu>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QtDebug>

#include "pqEventDispatcher.h"

/// Converts a string representation of a model index into the real thing
static QModelIndex OldGetIndex(QAbstractItemView& View, const QString& Name)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
  QStringList rows = Name.split('/', Qt::SkipEmptyParts);
#else
  QStringList rows = Name.split('/', QString::SkipEmptyParts);
#endif
  QString column;

  if (rows.size())
  {
    column = rows.back().split('|').at(1);
    rows.back() = rows.back().split('|').at(0);
  }

  QModelIndex index;
  for (int i = 0; i != rows.size(); ++i)
  {
    index = View.model()->index(rows[i].toInt(), column.toInt(), index);
  }

  return index;
}

static QModelIndex GetIndexByItemName(QAbstractItemView& View, const QString& Name)
{
  QListWidget* const listWidget = qobject_cast<QListWidget*>(&View);

  QModelIndex index;
  if (!listWidget)
  {
    return index;
  }
  QList<QListWidgetItem*> findResult = listWidget->findItems(Name, Qt::MatchExactly);
  if (findResult.count() > 0)
  {
    // in theory more than one item could match? Only return the index for
    // the first instance.
    index = View.model()->index(listWidget->row(findResult.first()), 0, index);
  }

  return index;
}

static QModelIndex GetIndex(QAbstractItemView* View, const QString& Name)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
  QStringList idxs = Name.split('/', Qt::SkipEmptyParts);
#else
  QStringList idxs = Name.split('/', QString::SkipEmptyParts);
#endif

  QModelIndex index;
  for (int i = 0; i != idxs.size(); ++i)
  {
    QStringList rowCol = idxs[i].split(':');
    index = View->model()->index(rowCol[0].toInt(), rowCol[1].toInt(), index);
  }

  return index;
}

///////////////////////////////////////////////////////////////////////////////
// pqAbstractItemViewEventPlayer

pqAbstractItemViewEventPlayer::pqAbstractItemViewEventPlayer(QObject* p)
  : pqWidgetEventPlayer(p)
{
}

bool pqAbstractItemViewEventPlayer::playEvent(
  QObject* Object, const QString& Command, const QString& Arguments, bool& Error)
{
  QAbstractItemView* object = qobject_cast<QAbstractItemView*>(Object);
  QMenu* contextMenu = qobject_cast<QMenu*>(Object);
  // if this a QMenu (potentially a context menu of the view),
  // we should not move onto parent
  if (!object && !contextMenu)
  {
    // mouse events go to the viewport widget
    object = qobject_cast<QAbstractItemView*>(Object->parent());
  }
  if (!object)
  {
    return false;
  }

  if (Command == "key" || Command == "editCancel" || Command == "editAccepted")
  {
    // We let the pqBasicWidgetEventPlayer do it ...
    return false;
  }

  if (Command == "currentChanged") // left to support old recordings
  {
    const QModelIndex index = OldGetIndex(*object, Arguments);
    if (!index.isValid())
      return false;

    object->setCurrentIndex(index);
    return true;
  }
  else if (Command == "currentChangedbyItemName")
  {
    const QModelIndex index = GetIndexByItemName(*object, Arguments);
    if (!index.isValid())
      return false;

    object->setCurrentIndex(index);
    return true;
  }
  else if (Command == "keyEvent")
  {
    QStringList data = Arguments.split(',');
    if (data.size() == 6)
    {
      QKeyEvent ke(static_cast<QEvent::Type>(data[0].toInt()), data[1].toInt(),
        static_cast<Qt::KeyboardModifiers>(data[2].toInt()), data[3], !!data[4].toInt(),
        data[5].toInt());
      qApp->notify(object, &ke);
      return true;
    }
  }
  else if (Command.startsWith("mouse"))
  {
    QStringList args = Arguments.split(',');
    if (args.size() == 5)
    {
      // Sometime, mouseWheel event does not contain the index, in that case,
      // they should be played by the basic widget event player
      return false;
    }
    if (args.size() == 6)
    {
      Qt::MouseButtons buttons = static_cast<Qt::MouseButton>(args[1].toInt());
      Qt::KeyboardModifiers keym = static_cast<Qt::KeyboardModifier>(args[2].toInt());
      int x = args[3].toInt();
      int y = args[4].toInt();
      QPoint pt;
      QHeaderView* header = qobject_cast<QHeaderView*>(object);
      if (header)
      {
        int idx = args[5].toInt();
        if (header->orientation() == Qt::Horizontal)
        {
          pt = QPoint(header->sectionPosition(idx) + 4, 4);
        }
        else
        {
          pt = QPoint(4, header->sectionPosition(idx) + 4);
        }
      }
      else
      {
        QModelIndex idx = GetIndex(object, args[5]);
        object->scrollTo(idx);
        QRect r = object->visualRect(idx);
        pt = r.topLeft() + QPoint(x, y);
      }
      if (Command == "mouseWheel")
      {
        int delta = args[0].toInt();
        QWheelEvent we(QPointF(x, y), QPointF(x, y), QPoint(0, 0), QPoint(0, delta), buttons, keym,
          Qt::NoScrollPhase, false);
        QCoreApplication::sendEvent(Object, &we);
        return true;
      }
      Qt::MouseButton button = static_cast<Qt::MouseButton>(args[0].toInt());
      QEvent::Type type = QEvent::MouseButtonPress;
      type = Command == "mouseMove" ? QEvent::MouseMove : type;
      type = Command == "mouseRelease" ? QEvent::MouseButtonRelease : type;
      type = Command == "mouseDblClick" ? QEvent::MouseButtonDblClick : type;
      QMouseEvent e(type, pt, object->mapToGlobal(pt), button, buttons, keym);
      qApp->notify(object->viewport(), &e);
      return true;
    }
  }

  if (!this->Superclass::playEvent(Object, Command, Arguments, Error))
  {
    qCritical() << "Unknown abstract item command: " << Command << "\n";
    Error = true;
  }
  return true;
}
