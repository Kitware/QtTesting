// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqAbstractBooleanEventPlayer.h"

#include <QAbstractButton>
#include <QAction>
#include <QGroupBox>
#include <QtDebug>

pqAbstractBooleanEventPlayer::pqAbstractBooleanEventPlayer(QObject* p)
  : pqWidgetEventPlayer(p)
{
}

bool pqAbstractBooleanEventPlayer::playEvent(
  QObject* Object, const QString& Command, const QString& Arguments, bool& Error)
{
  if (Command != "set_boolean")
    return false;

  const bool value = Arguments == "true" ? true : false;

  if (QAbstractButton* const object = qobject_cast<QAbstractButton*>(Object))
  {
    if (value != object->isChecked())
      object->click();
    return true;
  }

  if (QAction* const action = qobject_cast<QAction*>(Object))
  {
    if (action->isChecked() != value)
    {
      action->trigger();
    }
    return true;
  }

  if (QGroupBox* const object = qobject_cast<QGroupBox*>(Object))
  {
    if (value != object->isChecked())
    {
      object->setChecked(value);
    }
    return true;
  }
  qCritical() << "calling set_boolean on unhandled type " << Object;
  Error = true;
  return true;
}
