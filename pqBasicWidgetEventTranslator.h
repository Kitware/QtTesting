// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqBasicWidgetEventTranslator_h
#define _pqBasicWidgetEventTranslator_h

#include "pqWidgetEventTranslator.h"
#include <QPointer>
#include <QWidget>

/**
Translates low-level Qt events into high-level ParaView events that can be recorded as test cases.

\sa pqEventTranslator
*/

class QTTESTING_EXPORT pqBasicWidgetEventTranslator : public pqWidgetEventTranslator
{
  Q_OBJECT
  typedef pqWidgetEventTranslator Superclass;

public:
  pqBasicWidgetEventTranslator(QObject* p = 0);
  ~pqBasicWidgetEventTranslator() override;

  using Superclass::translateEvent;
  bool translateEvent(QObject* object, QEvent* event, int eventType, bool& error) override;

protected:
  QPoint LastPos;

private:
  pqBasicWidgetEventTranslator(const pqBasicWidgetEventTranslator&);
  pqBasicWidgetEventTranslator& operator=(const pqBasicWidgetEventTranslator&);
};

#endif // !_pqBasicWidgetEventTranslator_h
