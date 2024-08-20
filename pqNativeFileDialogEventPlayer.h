// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqNativeFileDialogEventPlayer_h
#define _pqNativeFileDialogEventPlayer_h

#include "pqWidgetEventPlayer.h"
#include <QMouseEvent>

class pqTestUtility;

/**
Records usage of native file dialogs in test cases.

\sa pqEventPlayer
*/

class QTTESTING_EXPORT pqNativeFileDialogEventPlayer : public pqWidgetEventPlayer
{
  Q_OBJECT
  typedef pqWidgetEventPlayer Superclass;

public:
  pqNativeFileDialogEventPlayer(pqTestUtility* util, QObject* p = 0);
  ~pqNativeFileDialogEventPlayer() override;

  using Superclass::playEvent;
  bool playEvent(
    QObject* Object, const QString& Command, const QString& Arguments, bool& Error) override;

protected Q_SLOTS:
  void start();
  void stop();

protected:
  pqTestUtility* mUtil;

private:
  pqNativeFileDialogEventPlayer(const pqNativeFileDialogEventPlayer&);
  pqNativeFileDialogEventPlayer& operator=(const pqNativeFileDialogEventPlayer&);
};

#endif // !_pqNativeFileDialogEventPlayer_h
