// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqNativeFileDialogEventTranslator_h
#define _pqNativeFileDialogEventTranslator_h

#include "pqWidgetEventTranslator.h"
#include <QMouseEvent>

class pqTestUtility;

/**
Records usage of native file dialogs in test cases.

\sa pqEventTranslator
*/

class QTTESTING_EXPORT pqNativeFileDialogEventTranslator : public pqWidgetEventTranslator
{
  Q_OBJECT
  typedef pqWidgetEventTranslator Superclass;

public:
  pqNativeFileDialogEventTranslator(pqTestUtility* util, QObject* p = 0);
  ~pqNativeFileDialogEventTranslator() override;

  using Superclass::translateEvent;
  bool translateEvent(QObject* Object, QEvent* Event, bool& Error) override;

  void record(const QString& command, const QString& args);

protected Q_SLOTS:
  void start();
  void stop();

protected:
  pqTestUtility* mUtil;

private:
  pqNativeFileDialogEventTranslator(const pqNativeFileDialogEventTranslator&);
  pqNativeFileDialogEventTranslator& operator=(const pqNativeFileDialogEventTranslator&);
};

#endif // !_pqNativeFileDialogEventTranslator_h
