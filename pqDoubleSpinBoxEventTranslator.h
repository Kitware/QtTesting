// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqDoubleSpinBoxEventTranslator_h
#define _pqDoubleSpinBoxEventTranslator_h

#include "pqWidgetEventTranslator.h"

/**
Translates low-level Qt events into high-level ParaView events that can be recorded as test cases.

\sa pqEventTranslator
*/

class QTTESTING_EXPORT pqDoubleSpinBoxEventTranslator : public pqWidgetEventTranslator
{
  Q_OBJECT
  typedef pqWidgetEventTranslator Superclass;

public:
  pqDoubleSpinBoxEventTranslator(QObject* p = 0);

  using Superclass::translateEvent;
  bool translateEvent(QObject* Object, QEvent* Event, bool& Error) override;

private:
  pqDoubleSpinBoxEventTranslator(const pqDoubleSpinBoxEventTranslator&);
  pqDoubleSpinBoxEventTranslator& operator=(const pqDoubleSpinBoxEventTranslator&);

  int Value;
  QObject* CurrentObject;

private Q_SLOTS:
  void onDestroyed(QObject*);
  void onValueChanged(double number);
};

#endif // !_pqDoubleSpinBoxEventTranslator_h
