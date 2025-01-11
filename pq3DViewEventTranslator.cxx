// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#include "pq3DViewEventTranslator.h"

#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWidget>

pq3DViewEventTranslator::pq3DViewEventTranslator(const QByteArray& classname, QObject* p)
  : pqWidgetEventTranslator(p)
  , mClassType(classname)
  , lastMoveEvent(QEvent::MouseButtonPress, QPoint(), QPoint(), Qt::MouseButton(),
      Qt::MouseButtons(), Qt::KeyboardModifiers())
{
}

pq3DViewEventTranslator::~pq3DViewEventTranslator() {}

bool pq3DViewEventTranslator::translateEvent(QObject* Object, QEvent* Event, bool& Error)
{
  QWidget* widget = qobject_cast<QWidget*>(Object);
  if (!widget || !Object->inherits(mClassType.data()))
  {
    return false;
  }

  switch (Event->type())
  {
    case QEvent::ContextMenu:
    {
      return true;
      break;
    }

    case QEvent::MouseButtonPress:
    {
      QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(Event);
      if (mouseEvent)
      {
        QSize size = widget->size();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        auto pos = mouseEvent->pos();
#else
        auto pos = mouseEvent->position().toPoint();
#endif
        double normalized_x = pos.x() / static_cast<double>(size.width());
        double normalized_y = pos.y() / static_cast<double>(size.height());
        int button = mouseEvent->button();
        int buttons = mouseEvent->buttons();
        int modifiers = mouseEvent->modifiers();
        Q_EMIT recordEvent(Object, "mousePress",
          QString("(%1,%2,%3,%4,%5)")
            .arg(normalized_x)
            .arg(normalized_y)
            .arg(button)
            .arg(buttons)
            .arg(modifiers));
      }

      // reset lastMoveEvent
      QMouseEvent e(QEvent::MouseButtonPress, QPoint(), QPoint(), Qt::MouseButton(),
        Qt::MouseButtons(), Qt::KeyboardModifiers());

#if QT_VERSION < 0x060000
      // FIXME: QMouseEvent copy ctor is private in Qt6
      lastMoveEvent = e;
#endif
      return true;
      break;
    }

    case QEvent::MouseMove:
    {
      QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(Event);
      if (mouseEvent)
      {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        auto pos = mouseEvent->pos();
#else
        auto pos = mouseEvent->position().toPoint();
#endif
        QMouseEvent e(QEvent::MouseMove, pos, widget->mapToGlobal(pos), mouseEvent->button(),
          mouseEvent->buttons(), mouseEvent->modifiers());

#if QT_VERSION < 0x060000
        // FIXME: QMouseEvent copy ctor is private in Qt6
        lastMoveEvent = e;
#endif
      }
      return true;
      break;
    }

    case QEvent::MouseButtonRelease:
    {
      QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(Event);
      if (mouseEvent)
      {
        QSize size = widget->size();

        // record last move event if it is valid
        if (lastMoveEvent.type() == QEvent::MouseMove)
        {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
          auto pos = lastMoveEvent.pos();
#else
          auto pos = lastMoveEvent.position().toPoint();
#endif
          double normalized_x = pos.x() / static_cast<double>(size.width());
          double normalized_y = pos.y() / static_cast<double>(size.height());
          int button = lastMoveEvent.button();
          int buttons = lastMoveEvent.buttons();
          int modifiers = lastMoveEvent.modifiers();

          Q_EMIT recordEvent(Object, "mouseMove",
            QString("(%1,%2,%3,%4,%5)")
              .arg(normalized_x)
              .arg(normalized_y)
              .arg(button)
              .arg(buttons)
              .arg(modifiers));
        }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        auto pos = mouseEvent->pos();
#else
        auto pos = mouseEvent->position().toPoint();
#endif
        double normalized_x = pos.x() / static_cast<double>(size.width());
        double normalized_y = pos.y() / static_cast<double>(size.height());
        int button = mouseEvent->button();
        int buttons = mouseEvent->buttons();
        int modifiers = mouseEvent->modifiers();

        Q_EMIT recordEvent(Object, "mouseRelease",
          QString("(%1,%2,%3,%4,%5)")
            .arg(normalized_x)
            .arg(normalized_y)
            .arg(button)
            .arg(buttons)
            .arg(modifiers));
      }
      return true;
      break;
    }

    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    {
      QKeyEvent* ke = static_cast<QKeyEvent*>(Event);
      QString data = QString("%1:%2:%3:%4:%5:%6")
                       .arg(ke->type())
                       .arg(ke->key())
                       .arg(static_cast<int>(ke->modifiers()))
                       .arg(ke->text())
                       .arg(ke->isAutoRepeat())
                       .arg(ke->count());
      Q_EMIT recordEvent(Object, "keyEvent", data);
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
