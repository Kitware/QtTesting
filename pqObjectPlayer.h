// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqObjectPlayer_h
#define _pqObjectPlayer_h

#include "QtTestingExport.h"
#include <QObject>

class QString;

/**
 * Base Player class.
 * It defines the API to play events.
 * It handles the playback of "check" events for QObject, looking for QObject::property.
 */
class QTTESTING_EXPORT pqObjectPlayer : public QObject
{
  Q_OBJECT
  typedef QObject Superclass;

public:
  pqObjectPlayer(QObject* parent = nullptr);
  ~pqObjectPlayer() override = default;

  /**
   * Entry point to handle events.
   *
   * If type is pqEventTypes::CHECK_EVENT, look for a QObject::property
   * named as the "command" and test its value against "arguments".
   *
   * pqEventTypes::ACTION_EVENT is forwarded to playEvent(QObject*, QString, QString, bool&)
   *
   * Returns true if event was handled.
   * Returns false if object is nullptr.
   */
  virtual bool playEvent(
    QObject* object, const QString& command, const QString& arguments, int eventType, bool& error);

  /**
   * Entry point for the pqEventTypes::ACTION_EVENT type only.
   * Default implementation is no op and return false.
   */
  virtual bool playEvent(QObject*, const QString&, const QString&, bool&);

private:
  pqObjectPlayer(const pqObjectPlayer&);
  pqObjectPlayer& operator=(const pqObjectPlayer&);
};

#endif // _pqBasicObjectPlayer_h
