// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#include "pq3DViewEventPlayer.h"

#include <QApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QRegularExpression>
#include <QWidget>
#include <QtDebug>

pq3DViewEventPlayer::pq3DViewEventPlayer(const QByteArray& classname, QObject* p)
  : pqWidgetEventPlayer(p)
  , mClassType(classname)
{
}

bool pq3DViewEventPlayer::playEvent(
  QObject* Object, const QString& Command, const QString& Arguments, bool& Error)
{
  QWidget* widget = qobject_cast<QWidget*>(Object);
  if (widget && Object->inherits(mClassType.data()))
  {
    if (Command == "mousePress" || Command == "mouseRelease" || Command == "mouseMove")
    {
      QRegularExpression mouseRegExp("\\(([^,]*),([^,]*),([^,]),([^,]),([^,]*)\\)");
      QRegularExpressionMatch match = mouseRegExp.match(Arguments);
      if (match.hasMatch())
      {
        QVariant v = match.captured(1);
        int x = static_cast<int>(v.toDouble() * widget->size().width());
        v = match.captured(2);
        int y = static_cast<int>(v.toDouble() * widget->size().height());
        v = match.captured(3);
        Qt::MouseButton button = static_cast<Qt::MouseButton>(v.toInt());
        v = match.captured(4);
        Qt::MouseButtons buttons = static_cast<Qt::MouseButton>(v.toInt());
        v = match.captured(5);
        Qt::KeyboardModifiers keym = static_cast<Qt::KeyboardModifier>(v.toInt());
        QEvent::Type type = (Command == "mousePress")
          ? QEvent::MouseButtonPress
          : ((Command == "mouseMove") ? QEvent::MouseMove : QEvent::MouseButtonRelease);
        QPoint pos(x, y);
        QMouseEvent e(type, pos, widget->mapToGlobal(pos), button, buttons, keym);
        qApp->notify(Object, &e);
      }
      return true;
    }
  }
  return this->Superclass::playEvent(Object, Command, Arguments, Error);
}
