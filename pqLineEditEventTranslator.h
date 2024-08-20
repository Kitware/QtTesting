// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqLineEditEventTranslator_h
#define _pqLineEditEventTranslator_h

#include "pqWidgetEventTranslator.h"

/**
Translates low-level Qt events into high-level ParaView events that can be recorded as test cases.

\sa pqEventTranslator
*/

class QTTESTING_EXPORT pqLineEditEventTranslator : public pqWidgetEventTranslator
{
  Q_OBJECT
  typedef pqWidgetEventTranslator Superclass;

public:
  pqLineEditEventTranslator(QObject* p = 0);

  using Superclass::translateEvent;
  bool translateEvent(QObject* object, QEvent* event, int eventType, bool& error) override;

private:
  pqLineEditEventTranslator(const pqLineEditEventTranslator&);
  pqLineEditEventTranslator& operator=(const pqLineEditEventTranslator&);
};

#endif // !_pqLineEditEventTranslator_h
