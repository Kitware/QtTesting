// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqComboBoxEventTranslator.h"

#include <QComboBox>
#include <QEvent>

pqComboBoxEventTranslator::pqComboBoxEventTranslator(QObject* p)
  : pqWidgetEventTranslator(p)
  , CurrentObject(0)
{
}

bool pqComboBoxEventTranslator::translateEvent(QObject* Object, QEvent* Event, bool& Error)
{
  QComboBox* combo = NULL;
  for (QObject* test = Object; combo == NULL && test != NULL; test = test->parent())
  {
    combo = qobject_cast<QComboBox*>(test);
  }

  if (!combo)
  {
    // not for me
    return false;
  }

  if (Event->type() == QEvent::Enter && Object == combo)
  {
    if (this->CurrentObject != Object)
    {
      if (this->CurrentObject)
      {
        disconnect(this->CurrentObject, 0, this, 0);
      }

      this->CurrentObject = Object;
      connect(combo, SIGNAL(destroyed(QObject*)), this, SLOT(onDestroyed(QObject*)));
      connect(combo, SIGNAL(activated(const QString&)), this, SLOT(onActivated(const QString&)));
      connect(combo, SIGNAL(editTextChanged(const QString&)), this,
        SLOT(onEditTextChanged(const QString&)));
    }
    return true;
  }
  return this->Superclass::translateEvent(Object, Event, Error);
}

void pqComboBoxEventTranslator::onDestroyed(QObject* /*Object*/)
{
  this->CurrentObject = 0;
}

void pqComboBoxEventTranslator::onActivated(const QString& text)
{
  Q_EMIT recordEvent(this->CurrentObject, "activated", text);
}

void pqComboBoxEventTranslator::onEditTextChanged(const QString& text)
{
  Q_EMIT recordEvent(this->CurrentObject, "editTextChanged", text);
}
