// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqAbstractStringEventPlayer.h"

#include <QComboBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QTextDocument>
#include <QTextEdit>
#include <QtDebug>

#include "pqObjectNaming.h"

pqAbstractStringEventPlayer::pqAbstractStringEventPlayer(QObject* p)
  : pqWidgetEventPlayer(p)
{
}

bool pqAbstractStringEventPlayer::playEvent(
  QObject* Object, const QString& Command, const QString& Arguments, bool& Error)
{
  if (Command != "set_string")
    return false;

  const QString value = Arguments;

  if (QComboBox* const comboBox = qobject_cast<QComboBox*>(Object))
  {
    int index = comboBox->findText(value);
    if (index != -1)
    {
      comboBox->setCurrentIndex(index);
    }
    else
    {
      QString possibles;
      for (int i = 0; i < comboBox->count(); i++)
      {
        possibles += QString("\t") + comboBox->itemText(i) + QString("\n");
      }
      qCritical() << "Unable to find " << value
                  << " in combo box: " << pqObjectNaming::GetName(*Object)
                  << "\nPossible values are:\n"
                  << possibles;
      Error = true;
    }
    return true;
  }

  if (QLineEdit* const lineEdit = qobject_cast<QLineEdit*>(Object))
  {
    lineEdit->setText(value);
    return true;
  }

  if (QTextEdit* const textEdit = qobject_cast<QTextEdit*>(Object))
  {
    textEdit->setFocus(Qt::OtherFocusReason);
    textEdit->document()->setPlainText(value);
    textEdit->clearFocus();
    return true;
  }

  if (QPlainTextEdit* const plainTextEdit = qobject_cast<QPlainTextEdit*>(Object))
  {
    plainTextEdit->setFocus(Qt::OtherFocusReason);
    plainTextEdit->document()->setPlainText(value);
    plainTextEdit->clearFocus();
    return true;
  }

  qCritical() << "calling set_string on unhandled type " << Object;

  Error = true;
  return true;
}
