// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqWidgetEventPlayer.h"

#include <QContextMenuEvent>
#include <QCoreApplication>
#include <QPoint>
#include <QWidget>
#include <QtDebug>
#include <cmath> // for std::abs

// ----------------------------------------------------------------------------
pqWidgetEventPlayer::pqWidgetEventPlayer(QObject* parent)
  : Superclass(parent)
{
}

// ----------------------------------------------------------------------------
bool pqWidgetEventPlayer::playEvent(
  QObject* object, const QString& command, const QString& arguments, bool& error)
{
  QWidget* widget = qobject_cast<QWidget*>(object);
  if (widget)
  {
    if (command == "contextMenu")
    {
      QPoint pt(widget->x(), widget->y());
      QPoint globalPt = widget->mapToGlobal(pt);
      QContextMenuEvent e(QContextMenuEvent::Other, pt, globalPt);
      qApp->notify(widget, &e);
      return true;
    }
    else if (command == "size")
    {
      QStringList entries = arguments.split(',');
      if (entries.size() == 2)
      {
        QSize sz = widget->size();
        error = (sz.width() != entries[0].toInt() || sz.height() != entries[1].toInt());
        if (error)
        {
          qCritical() << "Size mismatch: (" << arguments << ") != " << sz;
        }
        return true;
      }
    }
    else if (command == "sizeGreaterOrEqual")
    {
      QStringList entries = arguments.split(',');
      if (entries.size() == 2)
      {
        QSize sz = widget->size();
        error = (sz.width() < entries[0].toInt() || sz.height() < entries[1].toInt());
        if (error)
        {
          qCritical() << "Size mismatch: (" << arguments << ") > " << sz;
        }
        return true;
      }
    }
    else if (command == "aspectRatio")
    {
      double targetAspectRatio = arguments.toDouble();
      QSize sz = widget->size();
      double aspectRatio = static_cast<double>(sz.width()) / sz.height();
      error = (std::abs(targetAspectRatio - aspectRatio) > 1e-2);
      if (error)
      {
        qCritical() << "Ratio mismatch: (" << targetAspectRatio << ") != " << aspectRatio;
      }
      return true;
    }
  }
  return false;
}
