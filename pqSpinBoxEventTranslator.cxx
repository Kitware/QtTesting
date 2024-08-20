// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqSpinBoxEventTranslator.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QSpinBox>
#include <QStyle>
#include <QStyleOptionSpinBox>

// ----------------------------------------------------------------------------
pqSpinBoxEventTranslator::pqSpinBoxEventTranslator(QObject* p)
  : pqWidgetEventTranslator(p)
{
  this->CurrentObject = 0;
}

// ----------------------------------------------------------------------------
bool pqSpinBoxEventTranslator::translateEvent(QObject* Object, QEvent* Event, bool& Error)
{
  QSpinBox* object = qobject_cast<QSpinBox*>(Object);

  // consume line edit events if part of spin box
  if (!object && qobject_cast<QSpinBox*>(Object->parent()))
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
      connect(object, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
      connect(object, SIGNAL(destroyed(QObject*)), this, SLOT(onDestroyed(QObject*)));
    }
    return true;
  }

  if (Event->type() == QEvent::Leave && Object == object)
  {
    disconnect(this->CurrentObject, 0, this, 0);
    this->CurrentObject = 0;
    return true;
  }

  if (Event->type() == QEvent::KeyRelease && Object == object)
  {
    QKeyEvent* ke = static_cast<QKeyEvent*>(Event);
    QString keyText = ke->text();
    if (keyText.length() && keyText.at(0).isPrint())
    {
      Q_EMIT recordEvent(object, "set_int", QString("%1").arg(object->value()));
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
void pqSpinBoxEventTranslator::onDestroyed(QObject* /*Object*/)
{
  this->CurrentObject = 0;
}

// ----------------------------------------------------------------------------
void pqSpinBoxEventTranslator::onValueChanged(int number)
{
  Q_EMIT recordEvent(this->CurrentObject, "set_int", QString("%1").arg(number));
}
