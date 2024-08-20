// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqSpinBoxEventTranslator_h
#define _pqSpinBoxEventTranslator_h

#include "pqWidgetEventTranslator.h"

/**
Translates low-level Qt events into high-level ParaView events that can be recorded as test cases.

\sa pqEventTranslator
*/

class QTTESTING_EXPORT pqSpinBoxEventTranslator : public pqWidgetEventTranslator
{
  Q_OBJECT
  typedef pqWidgetEventTranslator Superclass;

public:
  pqSpinBoxEventTranslator(QObject* p = 0);

  using Superclass::translateEvent;
  bool translateEvent(QObject* Object, QEvent* Event, bool& Error) override;

private:
  pqSpinBoxEventTranslator(const pqSpinBoxEventTranslator&);
  pqSpinBoxEventTranslator& operator=(const pqSpinBoxEventTranslator&);

  QObject* CurrentObject;

private Q_SLOTS:
  void onDestroyed(QObject*);
  void onValueChanged(int number);
};

#endif // !_pqSpinBoxEventTranslator_h
