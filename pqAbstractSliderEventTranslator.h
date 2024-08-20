// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqAbstractSliderEventTranslator_h
#define _pqAbstractSliderEventTranslator_h

#include "pqWidgetEventTranslator.h"

/**
Translates low-level Qt events into high-level ParaView events that can be recorded as test cases.

\sa pqEventTranslator
*/

class QTTESTING_EXPORT pqAbstractSliderEventTranslator : public pqWidgetEventTranslator
{
  Q_OBJECT
  typedef pqWidgetEventTranslator Superclass;

public:
  pqAbstractSliderEventTranslator(QObject* p = 0);

  using Superclass::translateEvent;
  bool translateEvent(QObject* Object, QEvent* Event, bool& Error) override;

private:
  pqAbstractSliderEventTranslator(const pqAbstractSliderEventTranslator&);
  pqAbstractSliderEventTranslator& operator=(const pqAbstractSliderEventTranslator&);

  QObject* CurrentObject;

private Q_SLOTS:
  void onValueChanged(int);
};

#endif // !_pqAbstractSliderEventTranslator_h
