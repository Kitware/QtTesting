// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqDoubleSpinBoxEventTranslator.h"

#include <QDoubleSpinBox>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QStyle>
#include <QStyleOptionSpinBox>

// ----------------------------------------------------------------------------
pqDoubleSpinBoxEventTranslator::pqDoubleSpinBoxEventTranslator(QObject* p)
  : pqWidgetEventTranslator(p)
  , CurrentObject(0)
{
}

// ----------------------------------------------------------------------------
bool pqDoubleSpinBoxEventTranslator::translateEvent(QObject* Object, QEvent* Event, bool& Error)
{
  QDoubleSpinBox* const object = qobject_cast<QDoubleSpinBox*>(Object);

  // consume line edit events if part of spin box
  if (!object && qobject_cast<QDoubleSpinBox*>(Object->parent()))
  {
    return true;
  }

  if (!object)
    return false;

  if (Event->type() == QEvent::Enter && Object == object)
  {
    if (this->CurrentObject != Object)
    {
      if (this->CurrentObject)
      {
        disconnect(this->CurrentObject, 0, this, 0);
      }

      this->CurrentObject = Object;
      this->Value = object->value();
      connect(object, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
      connect(object, SIGNAL(destroyed(QObject*)), this, SLOT(onDestroyed(QObject*)));
    }
    return true;
  }

  if (Event->type() == QEvent::KeyRelease && Object == object)
  {
    QKeyEvent* ke = static_cast<QKeyEvent*>(Event);
    QString keyText = ke->text();
    if (keyText.length() && keyText.at(0).isPrint())
    {
      Q_EMIT recordEvent(object, "set_double", QString("%1").arg(object->value()));
    }
    else
    {
      Q_EMIT recordEvent(object, "key", QString("%1").arg(ke->key()));
    }
    return true;
  }
  return this->Superclass::translateEvent(Object, Event, Error);
}

// ----------------------------------------------------------------------------
void pqDoubleSpinBoxEventTranslator::onDestroyed(QObject* /*Object*/)
{
  this->CurrentObject = 0;
}

// ----------------------------------------------------------------------------
void pqDoubleSpinBoxEventTranslator::onValueChanged(double number)
{
  Q_EMIT recordEvent(this->CurrentObject, "set_double", QString("%1").arg(number));
}
