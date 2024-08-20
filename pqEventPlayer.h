// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqEventPlayer_h
#define _pqEventPlayer_h

#include "QtTestingExport.h"

#include <QList>
#include <QObject>
#include <QString>

class pqWidgetEventPlayer;
class pqTestUtility;

/**
Manages playback of test-cases, demos, tutorials, etc.
pqEventPlayer converts high-level ParaView events
(button click, row selection, etc) into low-level Qt events
that drive the user interface.

The high-level events created by pqEventTranslator are fed to
pqEventPlayer::playEvent() one-by-one, which passes each event
through a collection of pqWidgetEventPlayer objects.  Each
pqWidgetEventPlayer has the chance to ignore the event or
accept and update the program state using low-level Qt events.

pqEventDispatcher is an example of an object that can retrieve
high-level events from a pqEventSource object, feeding them
to pqEventPlayer.

For specialized Qt widgets it may be necessary to create "custom"
pqWidgetEventPlayer derivatives, which can be added to pqEventPlayer
using the addWidgetEventPlayer() method.
pqWidgetEventPlayer objects are searched in order for
event playback, so you can also use this mechanism to
"override" the builtin event players.

\sa pqWidgetEventPlayer, pqEventTranslator, pqEventDispatcher,
pqEventSource, pqXMLEventSource
*/

class QTTESTING_EXPORT pqEventPlayer : public QObject
{
  Q_OBJECT

public:
  pqEventPlayer();
  ~pqEventPlayer() override;

  /** Adds the default set of widget players to the current working set.
  Players are executed in-order, so you can call addWidgetEventPlayer()
  before this function to override default players. */
  void addDefaultWidgetEventPlayers(pqTestUtility* util);
  /** Adds a new player to the current working set of widget players.
  pqEventPlayer assumes control of the lifetime of the supplied object. */
  void addWidgetEventPlayer(pqWidgetEventPlayer*);
  /** Method to get a specific player */
  bool removeWidgetEventPlayer(const QString& className);
  /** Method to get a specific player */
  pqWidgetEventPlayer* getWidgetEventPlayer(const QString& className);

  /** Method to return the list of widget event players added previously*/
  QList<pqWidgetEventPlayer*> players() const;

  /** This method is called with each high-level ParaView event, which
  will invoke the corresponding low-level Qt functionality in-turn.
  If there was an error playing the event, Error argument will be set
  to "true".  Note: Currently there is no guarantee that playEvent()
  will return immediately, since the functionality it invokes may enter
  a separate event loop (a modal dialog or context menu, for example).
  Check event will not modify ui but check a widget value*/
  void playEvent(const QString& object, const QString& command, const QString& arguments,
    int eventType, bool& Error);
  void playEvent(
    const QString& object, const QString& command, const QString& arguments, bool& Error);

Q_SIGNALS:
  void eventAboutToBePlayed(
    const QString& Object, const QString& Command, const QString& Arguments);
  void eventPlayed(const QString& Object, const QString& Command, const QString& Arguments);
  void errorMessage(const QString&);

private:
  int getWidgetEventPlayerIndex(const QString& className);

  pqEventPlayer(const pqEventPlayer&);
  pqEventPlayer& operator=(const pqEventPlayer&);

  /// Stores the working set of widget players
  QList<pqWidgetEventPlayer*> Players;
};

#endif // !_pqEventPlayer_h
