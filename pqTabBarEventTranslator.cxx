// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqTabBarEventTranslator.h"

#include <QEvent>
#include <QMainWindow>
#include <QTabBar>

pqTabBarEventTranslator::pqTabBarEventTranslator(QObject* p)
  : pqWidgetEventTranslator(p)
  , CurrentObject(0)
{
}

bool pqTabBarEventTranslator::translateEvent(QObject* Object, QEvent* Event, bool& Error)
{
  QTabBar* const object = qobject_cast<QTabBar*>(Object);
  if (!object)
  {
    return false;
  }

  switch (Event->type())
  {
    case QEvent::Enter:
    {
      if (this->CurrentObject != Object)
      {
        if (this->CurrentObject)
        {
          disconnect(this->CurrentObject, 0, this, 0);
        }

        this->CurrentObject = object;
        connect(object, SIGNAL(currentChanged(int)), this, SLOT(indexChanged(int)));
      }
      return true;
      break;
    }
    default:
      break;
  }
  return this->Superclass::translateEvent(Object, Event, Error);
}

void pqTabBarEventTranslator::indexChanged(int which)
{
  QObject* recordedObject = this->CurrentObject;
  QObject* parent = this->CurrentObject->parent();
  if (parent && this->CurrentObject->objectName().isEmpty())
  {
    QMainWindow* mainWindow = qobject_cast<QMainWindow*>(this->CurrentObject->parent());
    if (mainWindow)
    {
      recordedObject = mainWindow;
    }
  }

  Q_EMIT recordEvent(recordedObject, "set_tab_with_text", this->CurrentObject->tabText(which));
}
