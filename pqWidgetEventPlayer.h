// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqWidgetEventPlayer_h
#define _pqWidgetEventPlayer_h

#include "QtTestingExport.h"

#include "pqObjectPlayer.h"

class QString;

/**
 * Implements pqObjectPlayer for QWidgets.
 *
 * \sa pqEventPlayer
 */

class QTTESTING_EXPORT pqWidgetEventPlayer : public pqObjectPlayer
{
  Q_OBJECT
  typedef pqObjectPlayer Superclass;

public:
  pqWidgetEventPlayer(QObject* parent);
  ~pqWidgetEventPlayer() override = default;

  using Superclass::playEvent;

  /**
   * Handle generics command action, like resize and context menu trigger.
   */
  bool playEvent(
    QObject* object, const QString& command, const QString& arguments, bool& error) override;
};

#endif // !_pqWidgetEventPlayer_h
