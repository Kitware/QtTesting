// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqTabBarEventTranslator_h
#define _pqTabBarEventTranslator_h

#include "pqWidgetEventTranslator.h"
#include <QPointer>
class QTabBar;

/**
\brief Records selection of a tab in a QTabBar

\note A QMainMainWindow has hidden QTabBar's that it uses to manage
QDockWidgets and other widgets that can be tabified.  These hidden QTabBars
are not named, can be numerous, and are instantiated in an unspecified order.
This makes finding the correct QTabBar difficult and sometimes impossible.
Therefore when recording selection of one of these hidden QTabBar's, the
QMainWindow is recorded as the object instead of the QTabBar itself. During
playback, if a QMainWindow was recorded then all hidden QTabBar direct
children will be searched for the tab with the recorded name. The risk with
this approach is that when multiple DockWidgets have the same windowTitle()
(which determines the tab name) then it is possible to select the wrong one.
This can be mediated in client code by ensuring that each DockWidget has a
unique windowTitle()

\sa pqEventTranslator
*/

class QTTESTING_EXPORT pqTabBarEventTranslator : public pqWidgetEventTranslator
{
  Q_OBJECT
  typedef pqWidgetEventTranslator Superclass;

public:
  pqTabBarEventTranslator(QObject* p = 0);

  using Superclass::translateEvent;
  bool translateEvent(QObject* Object, QEvent* Event, bool& Error) override;

protected Q_SLOTS:
  void indexChanged(int);

private:
  pqTabBarEventTranslator(const pqTabBarEventTranslator&);
  pqTabBarEventTranslator& operator=(const pqTabBarEventTranslator&);

  QPointer<QTabBar> CurrentObject;
};

#endif // !_pqTabBarEventTranslator_h
