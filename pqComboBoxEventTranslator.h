// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqComboBoxEventTranslator_h
#define _pqComboBoxEventTranslator_h

#include "pqWidgetEventTranslator.h"

/**
Translates low-level Qt events into high-level ParaView events that can be recorded as test cases.

\sa pqEventTranslator
*/

class QTTESTING_EXPORT pqComboBoxEventTranslator : public pqWidgetEventTranslator
{
  Q_OBJECT
  typedef pqWidgetEventTranslator Superclass;

public:
  pqComboBoxEventTranslator(QObject* p = 0);

  using Superclass::translateEvent;
  bool translateEvent(QObject* Object, QEvent* Event, bool& Error) override;

private:
  pqComboBoxEventTranslator(const pqComboBoxEventTranslator&);
  pqComboBoxEventTranslator& operator=(const pqComboBoxEventTranslator&);

  QObject* CurrentObject;

private Q_SLOTS:
  void onDestroyed(QObject*);
  void onActivated(const QString&);
  void onEditTextChanged(const QString&);
};

#endif // !_pqComboBoxEventTranslator_h
