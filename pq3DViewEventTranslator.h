// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#ifndef _pq3DViewEventTranslator_h
#define _pq3DViewEventTranslator_h

#include "pqWidgetEventTranslator.h"
#include <QMouseEvent>

/**
Translates low-level Qt events into high-level ParaView events that can be recorded as test cases.

\sa pqEventTranslator
*/

class QTTESTING_EXPORT pq3DViewEventTranslator : public pqWidgetEventTranslator
{
  Q_OBJECT
  typedef pqWidgetEventTranslator Superclass;

public:
  pq3DViewEventTranslator(const QByteArray& classname, QObject* p = 0);
  ~pq3DViewEventTranslator() override;

  using Superclass::translateEvent;
  bool translateEvent(QObject* Object, QEvent* Event, bool& Error) override;

protected:
  QByteArray mClassType;
  QMouseEvent lastMoveEvent;

private:
  pq3DViewEventTranslator(const pq3DViewEventTranslator&);
  pq3DViewEventTranslator& operator=(const pq3DViewEventTranslator&);
};

#endif // !_pq3DViewEventTranslator_h
