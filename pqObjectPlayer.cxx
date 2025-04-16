// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "pqObjectPlayer.h"

#include "pqEventTypes.h"

#include <QVariant>
#include <QtDebug>

// ----------------------------------------------------------------------------
pqObjectPlayer::pqObjectPlayer(QObject* parent)
  : Superclass(parent)
{
}

// ----------------------------------------------------------------------------
bool pqObjectPlayer::playEvent(
  QObject* object, const QString& command, const QString& arguments, int eventType, bool& error)
{
  if (!object)
  {
    return false;
  }

  if (eventType == pqEventTypes::CHECK_EVENT)
  {
    QVariant propertyValue = object->property(command.toUtf8().data());
    if (!propertyValue.isValid())
    {
      return false;
    }

    if (propertyValue.toString().replace("\t", " ") != arguments)
    {
      QString errorMessage = object->objectName() +
        " property value is: " + propertyValue.toString() + ". Expecting: " + arguments + ".";
      qCritical() << errorMessage.toUtf8().data();
      error = true;
    }
    return true;
  }

  if (eventType == pqEventTypes::ACTION_EVENT)
  {
    return this->playEvent(object, command, arguments, error);
  }

  return false;
}

// ----------------------------------------------------------------------------
bool pqObjectPlayer::playEvent(QObject*, const QString&, const QString&, bool&)
{
  return false;
}
