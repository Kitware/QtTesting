// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#include "pqComboBoxEventPlayer.h"
#include "pqEventTypes.h"
#include "pqObjectNaming.h"

#include <QComboBox>
#include <QDebug>

//-----------------------------------------------------------------------------
pqComboBoxEventPlayer::pqComboBoxEventPlayer(QObject* parentObject)
  : Superclass(parentObject)
{
}

//-----------------------------------------------------------------------------
pqComboBoxEventPlayer::~pqComboBoxEventPlayer() {}

//-----------------------------------------------------------------------------
bool pqComboBoxEventPlayer::playEvent(
  QObject* object, const QString& command, const QString& arguments, int eventType, bool& error)
{
  QComboBox* comboBox = qobject_cast<QComboBox*>(object);
  if (!comboBox)
  {
    return false;
  }

  if (eventType == pqEventTypes::ACTION_EVENT)
  {
    if (command == "activated" || command == "editTextChanged")
    {
      int index = comboBox->findText(arguments);
      if (index != -1)
      {
        comboBox->setCurrentIndex(index);
        if (command == "activated")
        {
          Q_EMIT comboBox->activated(index);
        }
      }
      else if (comboBox->isEditable() && command == "editTextChanged")
      {
        comboBox->setCurrentText(arguments);
      }
      else
      {
        QString possibles;
        for (int i = 0; i < comboBox->count(); i++)
        {
          possibles += QString("\t") + comboBox->itemText(i) + QString("\n");
        }
        qCritical() << "Unable to find " << arguments
                    << " in combo box: " << pqObjectNaming::GetName(*comboBox)
                    << "\nPossible values are:\n"
                    << possibles;
        error = true;
      }
    }
    return true;
  }
  return this->Superclass::playEvent(object, command, arguments, eventType, error);
}
