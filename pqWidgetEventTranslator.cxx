// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#include "pqWidgetEventTranslator.h"
#include "pqEventTypes.h"
#include <QEvent>
#include <QWidget>
#include <QtGlobal>

//-----------------------------------------------------------------------------
pqWidgetEventTranslator::pqWidgetEventTranslator(QObject* parentObject)
  : Superclass(parentObject)
{
}

//-----------------------------------------------------------------------------
pqWidgetEventTranslator::~pqWidgetEventTranslator() {}

bool pqWidgetEventTranslator::translateEvent(QObject* object, QEvent* event, bool& error)
{
  Q_UNUSED(error);
  QWidget* widget = qobject_cast<QWidget*>(object);
  if (!widget)
  {
    return false;
  }

  switch (event->type())
  {
    case QEvent::ContextMenu:
    {
      Q_EMIT recordEvent(widget, "contextMenu", "");
      break;
    }
    default:
    {
      break;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
bool pqWidgetEventTranslator::translateEvent(
  QObject* object, QEvent* event, int eventType, bool& error)
{
  if (eventType == pqEventTypes::ACTION_EVENT)
  {
    return this->translateEvent(object, event, error);
  }
  return false;
}
