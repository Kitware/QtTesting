// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqAbstractIntEventPlayer.h"

#include <QAbstractSlider>
#include <QSpinBox>
#include <QtDebug>

pqAbstractIntEventPlayer::pqAbstractIntEventPlayer(QObject* p)
  : pqWidgetEventPlayer(p)
{
}

bool pqAbstractIntEventPlayer::playEvent(
  QObject* Object, const QString& Command, const QString& Arguments, bool& Error)
{
  if (Command != "set_int" && Command != "spin")
    return false;

  const int value = Arguments.toInt();

  if (QAbstractSlider* const object = qobject_cast<QAbstractSlider*>(Object))
  {
    object->setValue(value);
    return true;
  }

  if (QSpinBox* const object = qobject_cast<QSpinBox*>(Object))
  {
    if (Command == "set_int")
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

  if (Command == "spin")
  {
    // let pqAbstractEventPlayer handle it if possible.
    return false;
  }

  qCritical() << "calling set_int on unhandled type " << Object;
  Error = true;
  return true;
}
