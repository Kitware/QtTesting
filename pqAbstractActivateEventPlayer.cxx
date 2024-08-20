// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqAbstractActivateEventPlayer.h"

#include <QAction>
#include <QApplication>
#include <QKeyEvent>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QToolButton>
#include <QtDebug>

#include "pqEventDispatcher.h"

pqAbstractActivateEventPlayer::pqAbstractActivateEventPlayer(QObject* p)
  : pqWidgetEventPlayer(p)
{
}

bool pqAbstractActivateEventPlayer::playEvent(
  QObject* Object, const QString& Command, const QString& Arguments, bool& Error)
{
  if (Command != "activate" && Command != "longActivate")
    return false;

  if (QMenuBar* const menu_bar = qobject_cast<QMenuBar*>(Object))
  {
    QAction* action = findAction(menu_bar, Arguments);
    if (action)
    {
      menu_bar->setActiveAction(action);
      return true;
    }

    qCritical() << "couldn't find action " << Arguments;
    Error = true;
    return true;
  }

  if (QMenu* qmenu = qobject_cast<QMenu*>(Object))
  {
    QAction* action = findAction(qmenu, Arguments);

    if (!action)
    {
      Error = true;
      return true;
    }

    // get a list of menus that must be navigated to
    // click on the action
    QObjectList menus;
    for (QObject* p = qmenu; qobject_cast<QMenu*>(p) || qobject_cast<QMenuBar*>(p); p = p->parent())
    {
      menus.push_front(p);
    }

    // unfold menus to make action visible
    int i;
    int numMenus = menus.size() - 1;
    for (i = 0; i < numMenus; ++i)
    {
      QObject* p = menus[i];
      QMenu* next = qobject_cast<QMenu*>(menus[i + 1]);
      if (QMenuBar* menu_bar = qobject_cast<QMenuBar*>(p))
      {
        menu_bar->setActiveAction(next->menuAction());
        int max_wait = 0;
        while (!next->isVisible() && (++max_wait) <= 2)
        {
          pqEventDispatcher::processEventsAndWait(100);
        }
      }
      else if (QMenu* menu = qobject_cast<QMenu*>(p))
      {
        menu->setActiveAction(next->menuAction());
        int max_wait = 0;
        while (!next->isVisible() && (++max_wait) <= 2)
        {
          pqEventDispatcher::processEventsAndWait(100);
        }
      }
    }

    // set active action, will cause scrollable menus to scroll
    // to make action visible
    qmenu->setActiveAction(action);

    // activate the action item
    QKeyEvent keyDown(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
    QKeyEvent keyUp(QEvent::KeyRelease, Qt::Key_Enter, Qt::NoModifier);

    qApp->notify(qmenu, &keyDown);
    qApp->notify(qmenu, &keyUp);

    // QApplication::processEvents();
    return true;
  }

  if (QAbstractButton* const object = qobject_cast<QAbstractButton*>(Object))
  {
    if (Command == "activate")
    {
      object->click();
      // QApplication::processEvents();
      return true;
    }
    if (Command == "longActivate")
    {
      QToolButton* tButton = qobject_cast<QToolButton*>(Object);
      if (tButton)
      {
        tButton->showMenu();
        return true;
      }
    }
  }

  if (QAction* const action = qobject_cast<QAction*>(Object))
  {
    action->activate(QAction::Trigger);
    // QApplication::processEvents();
    return true;
  }

  qCritical() << "calling activate on unhandled type " << Object;
  Error = true;
  return true;
}

QAction* pqAbstractActivateEventPlayer::findAction(QMenuBar* p, const QString& name)
{
  QList<QAction*> actions = p->actions();
  QAction* action = NULL;
  Q_FOREACH (QAction* a, actions)
  {
    if (a->menu()->objectName() == name)
    {
      action = a;
      break;
    }
  }

  if (!action)
  {
    Q_FOREACH (QAction* a, actions)
    {
      if (a->text() == name)
      {
        action = a;
        break;
      }
    }
  }

  return action;
}

QAction* pqAbstractActivateEventPlayer::findAction(QMenu* p, const QString& name)
{
  QStringList checked;
  QList<QAction*> actions = p->actions();
  QAction* action = NULL;
  Q_FOREACH (QAction* a, actions)
  {
    checked.append(a->objectName());
    if (checked.back() == name)
    {
      action = a;
      break;
    }
  }

  if (!action)
  {
    Q_FOREACH (QAction* a, actions)
    {
      checked.append(a->text());
      if (checked.back() == name)
      {
        action = a;
        break;
      }
    }
  }

  if (!action)
  {
    checked.removeDuplicates();
    qCritical() << "Unable to find action " << name << " in " << p->objectName()
                << ".  Available actions: " << checked.join(", ");
  }

  return action;
}
