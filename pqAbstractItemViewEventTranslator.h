// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqAbstractItemViewEventTranslator_h
#define _pqAbstractItemViewEventTranslator_h

#include "pqWidgetEventTranslator.h"
#include <QPoint>

/**
Translates low-level Qt events into high-level ParaView events that can be recorded as test cases.

\sa pqEventTranslator
*/

class QTTESTING_EXPORT pqAbstractItemViewEventTranslator : public pqWidgetEventTranslator
{
  Q_OBJECT
  typedef pqWidgetEventTranslator Superclass;

public:
  pqAbstractItemViewEventTranslator(QObject* p = 0);

  using Superclass::translateEvent;
  bool translateEvent(QObject* Object, QEvent* Event, bool& Error) override;

protected:
  QPoint LastPos;

private:
  pqAbstractItemViewEventTranslator(const pqAbstractItemViewEventTranslator&);
  pqAbstractItemViewEventTranslator& operator=(const pqAbstractItemViewEventTranslator&);
};

#endif // !_pqAbstractItemViewEventTranslator_h
