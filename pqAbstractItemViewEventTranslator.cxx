// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqAbstractItemViewEventTranslator.h"

#include <QAbstractItemView>
#include <QEvent>
#include <QHeaderView>
#include <QKeyEvent>

static QString toIndexStr(QModelIndex index)
{
  QString result;
  for (QModelIndex i = index; i.isValid(); i = i.parent())
  {
    result = "/" + QString("%1:%2").arg(i.row()).arg(i.column()) + result;
  }
  return result;
}

pqAbstractItemViewEventTranslator::pqAbstractItemViewEventTranslator(QObject* p)
  : pqWidgetEventTranslator(p)
{
}

bool pqAbstractItemViewEventTranslator::translateEvent(QObject* Object, QEvent* Event, bool& Error)
{
  QAbstractItemView* object = qobject_cast<QAbstractItemView*>(Object);
  if (!object)
  {
    // mouse events go to the viewport widget
    object = qobject_cast<QAbstractItemView*>(Object->parent());
  }
  if (!object)
  {
    return false;
  }

  // Don't try to record events for QComboBox implementation details
  if (object->inherits("QComboBoxListView"))
  {
    return false;
  }

  switch (Event->type())
  {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    {
      QKeyEvent* ke = static_cast<QKeyEvent*>(Event);
      QString data = QString("%1,%2,%3,%4,%5,%6")
                       .arg(ke->type())
                       .arg(ke->key())
                       .arg(static_cast<int>(ke->modifiers()))
                       .arg(ke->text())
                       .arg(ke->isAutoRepeat())
                       .arg(ke->count());
      Q_EMIT recordEvent(object, "keyEvent", data);
      return true;
    }
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonRelease:
    {
      if (Object == object)
      {
        return false;
      }
      QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(Event);
      if (Event->type() != QEvent::MouseButtonRelease)
      {
        this->LastPos = mouseEvent->pos();
      }
      QString idxStr;
      QPoint relPt = QPoint(0, 0);
      QHeaderView* header = qobject_cast<QHeaderView*>(object);
      if (header)
      {
        int idx = header->logicalIndexAt(mouseEvent->pos());
        idxStr = QString("%1").arg(idx);
      }
      else
      {
        QModelIndex idx = object->indexAt(mouseEvent->pos());
        idxStr = toIndexStr(idx);
        QRect r = object->visualRect(idx);
        relPt = mouseEvent->pos() - r.topLeft();
      }

      QString info = QString("%1,%2,%3,%4,%5,%6")
                       .arg(mouseEvent->button())
                       .arg(mouseEvent->buttons())
                       .arg(mouseEvent->modifiers())
                       .arg(relPt.x())
                       .arg(relPt.y())
                       .arg(idxStr);
      if (Event->type() == QEvent::MouseButtonPress)
      {
        Q_EMIT recordEvent(object, "mousePress", info);
      }
      else if (Event->type() == QEvent::MouseButtonDblClick)
      {
        Q_EMIT recordEvent(object, "mouseDblClick", info);
      }
      else if (Event->type() == QEvent::MouseButtonRelease)
      {
        if (this->LastPos != mouseEvent->pos())
        {
          Q_EMIT recordEvent(object, "mouseMove", info);
        }
        Q_EMIT recordEvent(object, "mouseRelease", info);
      }
      return true;
      break;
    }
    case QEvent::Wheel:
    {
      if (Object == object)
      {
        return false;
      }
      QPoint relPt = QPoint(0, 0);
      QWheelEvent* wheelEvent = dynamic_cast<QWheelEvent*>(Event);
      if (wheelEvent)
      {
        QString idxStr;
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        QModelIndex idx = object->indexAt(wheelEvent->position().toPoint());
#else
        QModelIndex idx = object->indexAt(wheelEvent->pos());
#endif
        idxStr = toIndexStr(idx);
        QRect r = object->visualRect(idx);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        relPt = wheelEvent->position().toPoint() - r.topLeft();
#else
        relPt = wheelEvent->pos() - r.topLeft();
#endif
        int numStep = wheelEvent->angleDelta().y() > 0 ? 120 : -120;
        int buttons = wheelEvent->buttons();
        int modifiers = wheelEvent->modifiers();
        Q_EMIT recordEvent(Object, "mouseWheel",
          QString("%1,%2,%3,%4,%5")
            .arg(numStep)
            .arg(buttons)
            .arg(modifiers)
            .arg(relPt.x())
            .arg(relPt.y())
            .arg(idxStr));
      }
      return true;
      break;
    }
    default:
    {
      break;
    }
  }
  return this->Superclass::translateEvent(Object, Event, Error);
}
