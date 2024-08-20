// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqEventRecorder_h
#define _pqEventRecorder_h

// Qt includes
#include <QIODevice>
#include <QObject>
#include <QTextStream>

// QtTesting includes
#include "QtTestingExport.h"

class pqEventObserver;
class pqEventTranslator;

/// Class to provide the recording.
/// Two behaviors :
///   - Either Continuous Flush is true, the file is written after each new actions
/// Indeed if the QtTesting/Application crashs, part of the script is written.
///   - Or the file is written only when we stop the record.

/// A simple call to the function "recordEvent" with good parameters, will start
/// the record.

class QTTESTING_EXPORT pqEventRecorder : public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool continuousFlush READ continuousFlush WRITE setContinuousFlush)
  typedef QObject Superclass;

public:
  explicit pqEventRecorder(QObject* parent = 0);
  ~pqEventRecorder() override;

  bool continuousFlush() const;
  bool recordInteractionTimings() const;

  void setFile(QIODevice* file);
  QIODevice* file() const;

  void setObserver(pqEventObserver* observer);
  pqEventObserver* observer() const;

  void setTranslator(pqEventTranslator* translator);
  pqEventTranslator* translator() const;

  bool isRecording() const;

  void recordEvents(pqEventTranslator* translator, pqEventObserver* observer, QIODevice* file,
    bool continuousFlush);

Q_SIGNALS:
  void started();
  void stopped();
  void paused(bool);

public Q_SLOTS:
  void flush();

  void start();
  void stop(int value);

  // Pause the recording if value is False
  // Keep recording if value is True
  void unpause(bool value);

  // Pause the recording if value is True
  // Keep recording if value is False
  void pause(bool value);

  // Set/Unset the recorder into Check mode
  // Connection signal into the correct slots
  void check(bool value);

  // Set the continuous flush value
  // and connect/disconnect the current observer with
  // this recorder
  // If true, events are written on file as soon
  // as they are recorded.
  // False by default
  void setContinuousFlush(bool value);

  // Set the record interaction timings value
  // and pass it to current active translator
  // if any.
  // When set to true user's pause between interaction events
  // are recorded and thus will be reproduced during playback.
  // When enabled, timing starts to be recorded only after the
  // next recorded event.
  // False by default
  void setRecordInteractionTimings(bool value);

protected:
  pqEventObserver* ActiveObserver;
  pqEventTranslator* ActiveTranslator;
  QIODevice* File;

  bool ContinuousFlush;
  bool RecordInteractionTimings;
  QTextStream Stream;
};

#endif // !_pqEventRecorder_h
