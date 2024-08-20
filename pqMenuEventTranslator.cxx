// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqMenuEventTranslator.h"

#include <QAction>
#include <QEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QMenuBar>
#include <QMouseEvent>

pqMenuEventTranslator::pqMenuEventTranslator(QObject* p)
  : pqWidgetEventTranslator(p)
{
}

pqMenuEventTranslator::~pqMenuEventTranslator() {}

bool pqMenuEventTranslator::translateEvent(QObject* Object, QEvent* Event, bool& Error)
{
  QMenu* const menu = qobject_cast<QMenu*>(Object);
  QMenuBar* const menubar = qobject_cast<QMenuBar*>(Object);
  if (!menu && !menubar)
  {
    return false;
  }

  if (menubar)
  {
    QMouseEvent* e = dynamic_cast<QMouseEvent*>(Event);
    if (e && e->button() == Qt::LeftButton)
    {
      QAction* action = menubar->actionAt(e->pos());
      if (action && action->menu())
      {
        QString which = action->menu()->objectName();
        if (which.isEmpty())
        {
          which = action->text();
        }
        Q_EMIT recordEvent(menubar, "activate", which);
      }
    }
    return true;
  }

  const auto actionArgument = [](QAction* action) -> QString {
    QString argument = action->objectName();
    if (argument.isNull())
    {
      argument = action->text();
    }
    return argument;
  };

  if (Event->type() == QEvent::Show)
  {
    // If this menu has any sub-menu's record this menu as the parent for subsequent enter event.
    for (auto action : menu->actions())
    {
      if (action->menu())
      {
        this->SubMenuParent[action] = menu;
      }
    }

    return true;
  }
  else if (Event->type() == QEvent::Enter)
  {
    // Note: Using QEvent::Enter instead of QEvent::FocusIn here because the menu
    // can be activated without first getting focus.
    if (this->SubMenuParent.find(menu->menuAction()) != this->SubMenuParent.end())
    { // Then a previous menu has recorded this action as a sub-menu
      Q_EMIT recordEvent(
        this->SubMenuParent[menu->menuAction()], "activate", actionArgument(menu->menuAction()));
    }
  }
  else if (Event->type() == QEvent::Close || Event->type() == QEvent::Hide)
  {
    // Clean up sub-menu mapping
    for (auto it = this->SubMenuParent.begin(); it != this->SubMenuParent.end();)
    {
      if (it->second == menu)
      {
        it = this->SubMenuParent.erase(it);
      }
      else
      {
        ++it;
      }
    }
    return true;
  }
  else if (Event->type() == QEvent::KeyPress)
  {

    QKeyEvent* e = static_cast<QKeyEvent*>(Event);
    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
    {
      QAction* action = menu->activeAction();
      if (action)
      {
        Q_EMIT recordEvent(menu, "activate", actionArgument(action));
      }
    }
    else if (e->key() == Qt::Key_Right)
    {
      QAction* action = menu->activeAction();
      if (action && action->menu())
      {
        Q_EMIT recordEvent(menu, "activate", actionArgument(action));
      }
    }
    else
    {
      for (auto action : menu->actions())
      {
        const QKeySequence mnemonic = QKeySequence::mnemonic(action->text());
        if (!mnemonic.isEmpty())
        { // Then the action has a keyboard accelerator
#if QT_VERSION >= 0x060000
          if (mnemonic ==
            QKeySequence(QKeyCombination(e->modifiers(), static_cast<Qt::Key>(e->key()))))
#else
          if (mnemonic == QKeySequence(e->modifiers() + e->key()))
#endif
          {
            Q_EMIT recordEvent(menu, "activate", actionArgument(action));
          }
        }
      }
    }
    return true;
  }
  else if (Event->type() == QEvent::MouseButtonRelease)
  {
    QMouseEvent* e = static_cast<QMouseEvent*>(Event);
    if (e->button() == Qt::LeftButton)
    {
      QAction* action = menu->actionAt(e->pos());
      if (action && !action->menu())
      {
        Q_EMIT recordEvent(menu, "activate", actionArgument(action));
      }
    }
    return true;
  }
  return this->Superclass::translateEvent(Object, Event, Error);
}
