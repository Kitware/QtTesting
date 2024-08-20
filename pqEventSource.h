// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqEventSource_h
#define _pqEventSource_h

#include "QtTestingExport.h"
#include "pqEventTypes.h"
#include <QObject>
class QString;

/// Abstract interface for objects that can supply high-level testing events
class QTTESTING_EXPORT pqEventSource : public QObject
{
  Q_OBJECT
public:
  pqEventSource(QObject* p)
    : QObject(p)
  {
  }
  ~pqEventSource() override {}

  enum eventReturnFlag
  {
    SUCCESS,
    FAILURE,
    DONE
  };

  /** Retrieves the next available event.
    Returns SUCCESS if an event was returned and can be processed,
    FAILURE if there was a problem,
    DONE, if there are no more events. */
  virtual int getNextEvent(
    QString& object, QString& command, QString& arguments, int& eventType) = 0;

  /** Set the filename for contents.
      Returns true for valid file, false for invalid file */
  virtual void setContent(const QString& filename) = 0;

  /** tell the source to stop feeding in events */
  virtual void stop() {}
};

#endif // !_pqEventSource_h
