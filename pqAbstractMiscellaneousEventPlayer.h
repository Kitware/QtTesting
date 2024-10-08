// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqAbstractMiscellaneousEventPlayer_h
#define _pqAbstractMiscellaneousEventPlayer_h

#include "pqWidgetEventPlayer.h"

class pqTestUtility;

/// Event playback handler for a collection of miscellaneous commands.
/// For these events, the "object" on which the event is triggered is generally
/// immaterial.
///
/// Supported commands are:
/// \li \c pause : pause the application for a fixed time. Time is specified as
///                the arguments in milliseconds to pause the application for.
///                This is a sleep i.e. no events will be processed and the
///                application will appear frozen.
/// \li \c process_events: process pending events (including timers, etc.).
///                Optional time (in milliseconds) may be specified to also pause
///                the test playback. Unlike "pause" however, this will continue
///                to process all events arising in the application e.g.
///                responding to timer events.
/// \li \c dashboard_mode: trigger a call to pqTestUtility::setDashboardMode with the
///                provided argument.

class QTTESTING_EXPORT pqAbstractMiscellaneousEventPlayer : public pqWidgetEventPlayer
{
  Q_OBJECT
  typedef pqWidgetEventPlayer Superclass;

public:
  pqAbstractMiscellaneousEventPlayer(pqTestUtility* util, QObject* p = 0);

  using Superclass::playEvent;
  bool playEvent(
    QObject* Object, const QString& Command, const QString& Arguments, bool& Error) override;

private:
  pqAbstractMiscellaneousEventPlayer(const pqAbstractMiscellaneousEventPlayer&);
  pqAbstractMiscellaneousEventPlayer& operator=(const pqAbstractMiscellaneousEventPlayer&);

  pqTestUtility* TestUtility = nullptr;
};

#endif // !_pqAbstractMiscellaneousEventPlayer_h
