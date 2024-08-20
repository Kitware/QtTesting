// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqMenuEventTranslator_h
#define _pqMenuEventTranslator_h

#include "pqWidgetEventTranslator.h"

#include <queue>

class QMenu;
class QAction;

/**
Translates low-level Qt events into high-level ParaView events that can be recorded as test cases.

\sa pqEventTranslator
*/

class QTTESTING_EXPORT pqMenuEventTranslator : public pqWidgetEventTranslator
{
  Q_OBJECT
  typedef pqWidgetEventTranslator Superclass;

public:
  pqMenuEventTranslator(QObject* p = 0);
  ~pqMenuEventTranslator() override;

  using Superclass::translateEvent;
  bool translateEvent(QObject* Object, QEvent* Event, bool& Error) override;

private:
  pqMenuEventTranslator(const pqMenuEventTranslator&);
  pqMenuEventTranslator& operator=(const pqMenuEventTranslator&);

  // A sub-menu action -> parent map
  std::map<QAction*, QMenu*> SubMenuParent;
};

#endif // !_pqMenuEventTranslator_h
