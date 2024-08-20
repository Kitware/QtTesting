// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqAbstractButtonEventTranslator.h"

#include <QAbstractButton>
#include <QAction>
#include <QDebug>
#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPushButton>
#include <QToolButton>

pqAbstractButtonEventTranslator::pqAbstractButtonEventTranslator(QObject* p)
  : pqWidgetEventTranslator(p)
{
  this->LastMouseEventType = QEvent::None;
}

bool pqAbstractButtonEventTranslator::translateEvent(QObject* Object, QEvent* Event, bool& Error)
{
  QAbstractButton* const object = qobject_cast<QAbstractButton*>(Object);
  if (!object)
  {
    return false;
  }
  switch (Event->type())
  {
    case QEvent::KeyPress:
    {
      QKeyEvent* const e = dynamic_cast<QKeyEvent*>(Event);
      if (e->key() == Qt::Key_Space)
      {
        onActivate(object);
      }
      return true;
      break;
    }
    case QEvent::MouseButtonPress:
    {
      QMouseEvent* const e = dynamic_cast<QMouseEvent*>(Event);
      this->LastMouseEventType = Event->type();
      // Menus are opened on mouse press
      if (e->button() == Qt::LeftButton && object->rect().contains(e->pos()) &&
        this->hasMenu(object))
      {
        onActivate(object);
      }
      return true;
      break;
    }
    case QEvent::Timer:
    {
      if (this->LastMouseEventType == QEvent::MouseButtonPress)
      {
        QToolButton* tButton = qobject_cast<QToolButton*>(object);
        if (tButton && tButton->popupMode() == QToolButton::DelayedPopup)
        {
          Q_EMIT recordEvent(object, "longActivate", "");
          // Tell comming mouse button release to not record activate.
          this->LastMouseEventType = QEvent::None;
        }
      }
      return true;
      break;
    }
    case QEvent::MouseButtonRelease:
    {
      bool lastEventWasMouseButtonPress = this->LastMouseEventType == QEvent::MouseButtonPress;
      QMouseEvent* const e = dynamic_cast<QMouseEvent*>(Event);
      this->LastMouseEventType = Event->type();
      // Buttons are activated on mouse release. Menus should already be recorded.
      if (e->button() == Qt::LeftButton && object->rect().contains(e->pos()) &&
        !this->hasMenu(object) && lastEventWasMouseButtonPress)
      {
        onActivate(object);
      }
      return true;
      break;
    }
    default:
      break;
  }
  return this->Superclass::translateEvent(Object, Event, Error);
}

void pqAbstractButtonEventTranslator::onActivate(QAbstractButton* actualObject)
{
  QObject* object = actualObject;
  QToolButton* tb = qobject_cast<QToolButton*>(object);
  if (tb && tb->defaultAction())
  {
    object = tb->defaultAction();
  }
  if (actualObject->isCheckable())
  {
    const bool new_value = !actualObject->isChecked();
    Q_EMIT recordEvent(object, "set_boolean", new_value ? "true" : "false");
  }
  else
  {
    Q_EMIT recordEvent(object, "activate", "");
  }
}

bool pqAbstractButtonEventTranslator::hasMenu(QAbstractButton* button) const
{
  bool hasMenu = false;
  QPushButton* pushButton = qobject_cast<QPushButton*>(button);
  if (pushButton)
  {
    hasMenu = pushButton->menu() != 0;
  }
  QToolButton* toolButton = qobject_cast<QToolButton*>(button);
  if (toolButton)
  {
    hasMenu = toolButton->menu() != 0 ||
      (toolButton->defaultAction() && toolButton->defaultAction()->menu() != 0);
    hasMenu = hasMenu && toolButton->popupMode() != QToolButton::DelayedPopup;
  }
  return hasMenu;
}
