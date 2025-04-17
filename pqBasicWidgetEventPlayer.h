// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqBasicWidgetEventPlayer_h
#define _pqBasicWidgetEventPlayer_h

#include "pqWidgetEventPlayer.h"

/**
 * EventPlayer for QWidget to handles mouse and keyboard inputs.
 */
class pqBasicWidgetEventPlayer : public pqWidgetEventPlayer
{
  Q_OBJECT
  typedef pqWidgetEventPlayer Superclass;

public:
  pqBasicWidgetEventPlayer(QObject* parent = nullptr);

  using Superclass::playEvent;

  bool playEvent(
    QObject* object, const QString& command, const QString& arguments, bool& error) override;

private:
  pqBasicWidgetEventPlayer(const pqBasicWidgetEventPlayer&);
  pqBasicWidgetEventPlayer& operator=(const pqBasicWidgetEventPlayer&);
};

#endif // !_pqBasicWidgetEventPlayer_h
