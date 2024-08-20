// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqTabBarEventPlayer.h"

#include <QLineEdit>
#include <QMainWindow>
#include <QTabBar>
#include <QtDebug>

#include "pqObjectNaming.h"

pqTabBarEventPlayer::pqTabBarEventPlayer(QObject* p)
  : pqWidgetEventPlayer(p)
{
}

bool pqTabBarEventPlayer::playEvent(
  QObject* target, const QString& command, const QString& arguments, bool& error_flag)
{
  const QString value = arguments;

  QTabBar* const tab_bar = qobject_cast<QTabBar*>(target);
  QMainWindow* const main_window = qobject_cast<QMainWindow*>(target);
  if (!(tab_bar || main_window))
  {
    return false;
  }

  if (command == "set_tab")
  {
    if (!tab_bar)
    {
      qCritical() << "calling set_tab on unhandled type " << target;
      error_flag = true;
    }

    // "set_tab" saves tab index. This was done in the past. Newly recorded
    // tests will use set_tab_with_text for more reliable playback.
    bool ok = false;
    int which = value.toInt(&ok);
    if (!ok)
    {
      qCritical() << "calling set_tab with invalid argument on " << target;
      error_flag = true;
    }
    else if (tab_bar->count() < which)
    {
      qCritical() << "calling set_tab with out of bounds index on " << target;
      error_flag = true;
    }
    else
    {
      tab_bar->setCurrentIndex(which);
    }
    return true;
  }
  else if (command == "set_tab_with_text")
  {
    if (tab_bar)
    {
      for (int cc = 0; cc < tab_bar->count(); cc++)
      {
        if (tab_bar->tabText(cc) == value)
        {
          tab_bar->setCurrentIndex(cc);
          return true;
        }
      }

      QString error_message;
      QTextStream errorStream(&error_message);
      errorStream << "calling set_tab_with_text with unknown tab " << value
                  << "Available tabs are (count=" << tab_bar->count() << ")";
      for (int tab_index = 0; tab_index < tab_bar->count(); tab_index++)
      {
        errorStream << '\t' << tab_bar->tabText(tab_index);
      }
      qCritical() << error_message;
      error_flag = true;
    }
    else if (main_window)
    {
      // Special code for DockWidget tabs (and other unnamaed QMainWindow tabs). See comment in
      // pqTabBarEventTranslator for full explanation.
      QList<QTabBar*> main_window_tab_bars =
        main_window->findChildren<QTabBar*>(QString(), Qt::FindDirectChildrenOnly);
      for (QTabBar* possible_dock_widget_tab : main_window_tab_bars)
      {
        if (possible_dock_widget_tab->objectName().isEmpty())
        {
          for (int tab_index = 0; tab_index < possible_dock_widget_tab->count(); ++tab_index)
          {
            if (possible_dock_widget_tab->tabText(tab_index) == value)
            {
              possible_dock_widget_tab->setCurrentIndex(tab_index);
              return true;
            }
          }
        }
      }
      qCritical() << "Could not find any tabs named " << value << " in the main window";
      error_flag = true;
    }
    else
    {
      qCritical() << "calling set_tab_with_text on unhandled type " << target;
      error_flag = true;
    }
    return true;
  }

  return false;
}
