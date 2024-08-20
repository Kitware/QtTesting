// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqAbstractDoubleEventPlayer.h"

#include <QDoubleSpinBox>
#include <QtDebug>

pqAbstractDoubleEventPlayer::pqAbstractDoubleEventPlayer(QObject* p)
  : pqWidgetEventPlayer(p)
{
}

bool pqAbstractDoubleEventPlayer::playEvent(
  QObject* Object, const QString& Command, const QString& Arguments, bool& Error)
{
  if (Command != "set_double" && Command != "spin")
    return false;

  const double value = Arguments.toDouble();

  if (QDoubleSpinBox* const object = qobject_cast<QDoubleSpinBox*>(Object))
  {
    if (Command == "set_double")
    {
      object->setValue(value);
      return true;
    }
    else if (Command == "spin" && Arguments == "up")
    {
      object->stepUp();
      return true;
    }
    else if (Command == "spin" && Arguments == "down")
    {
      object->stepDown();
      return true;
    }
  }

  qCritical() << "calling set_double on unhandled type " << Object;
  Error = true;
  return true;
}
