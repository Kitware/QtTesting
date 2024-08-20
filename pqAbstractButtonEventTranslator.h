// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqAbstractButtonEventTranslator_h
#define _pqAbstractButtonEventTranslator_h

// Qt includes
#include <QMouseEvent>

#include "pqWidgetEventTranslator.h"

class QAbstractButton;

/**
Translates low-level Qt events into high-level ParaView events that can be recorded as test cases.

\sa pqEventTranslator
*/

class QTTESTING_EXPORT pqAbstractButtonEventTranslator : public pqWidgetEventTranslator
{
  Q_OBJECT
  typedef pqWidgetEventTranslator Superclass;

public:
  pqAbstractButtonEventTranslator(QObject* p = 0);

  using Superclass::translateEvent;
  bool translateEvent(QObject* Object, QEvent* Event, bool& Error) override;

protected:
  virtual void onActivate(QAbstractButton*);
  virtual bool hasMenu(QAbstractButton* actualObject) const;

  QEvent::Type LastMouseEventType;

private:
  pqAbstractButtonEventTranslator(const pqAbstractButtonEventTranslator&);
  pqAbstractButtonEventTranslator& operator=(const pqAbstractButtonEventTranslator&);
};

#endif // !_pqAbstractButtonEventTranslator_h
