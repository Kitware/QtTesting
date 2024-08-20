// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqAbstractSliderEventTranslator.h"

#include <QAbstractSlider>
#include <QEvent>
#include <QScrollBar>

pqAbstractSliderEventTranslator::pqAbstractSliderEventTranslator(QObject* p)
  : pqWidgetEventTranslator(p)
  , CurrentObject(0)
{
}

bool pqAbstractSliderEventTranslator::translateEvent(QObject* Object, QEvent* Event, bool& Error)
{
  QAbstractSlider* const object = qobject_cast<QAbstractSlider*>(Object);
  if (!object || qobject_cast<QScrollBar*>(object))
    return false;

  switch (Event->type())
  {
    case QEvent::Enter:
      this->CurrentObject = Object;
      connect(object, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
      return true;
      break;
    case QEvent::Leave:
      disconnect(Object, 0, this, 0);
      this->CurrentObject = 0;
      return true;
      break;
    default:
      break;
  }
  return this->Superclass::translateEvent(Object, Event, Error);
}

void pqAbstractSliderEventTranslator::onValueChanged(int Value)
{
  Q_EMIT recordEvent(this->CurrentObject, "set_int", QString().setNum(Value));
}
