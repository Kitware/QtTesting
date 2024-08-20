// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#ifndef __pqComboBoxEventPlayer_h
#define __pqComboBoxEventPlayer_h

#include "pqWidgetEventPlayer.h"

/// pqComboBoxEventPlayer is a player for QComboBoxWidget. Plays back the state
/// recorded using pqComboBoxEventTranslator.
class QTTESTING_EXPORT pqComboBoxEventPlayer : public pqWidgetEventPlayer
{
  Q_OBJECT
  typedef pqWidgetEventPlayer Superclass;

public:
  pqComboBoxEventPlayer(QObject* parent = 0);
  ~pqComboBoxEventPlayer() override;

  /// Play an event on a QComboBox
  using Superclass::playEvent;
  bool playEvent(QObject* object, const QString& command, const QString& arguments, int eventType,
    bool& error) override;

Q_SIGNALS:
  // Transition signal to call combo box activated signal
  void activated(int index);

private:
  pqComboBoxEventPlayer(const pqComboBoxEventPlayer&); // Not implemented.
  void operator=(const pqComboBoxEventPlayer&);        // Not implemented.
};

#endif
