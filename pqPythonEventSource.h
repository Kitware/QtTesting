// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqPythonEventSource_h
#define _pqPythonEventSource_h

#include "pqThreadedEventSource.h"
#include <QString>

/** Concrete implementation of pqEventSource that retrieves events recorded
by pqPythonEventObserver */
class QTTESTING_EXPORT pqPythonEventSource : public pqThreadedEventSource
{
  Q_OBJECT
public:
  pqPythonEventSource(QObject* p = 0);
  ~pqPythonEventSource();

  void setContent(const QString& path);

  static QString getProperty(QString& object, QString& prop);
  static void setProperty(QString& object, QString& prop, const QString& value);
  static QStringList getChildren(QString& object);
  static QString invokeMethod(QString& object, QString& method);

protected:
  virtual void run();
  virtual void start();

protected Q_SLOTS:
  void threadGetProperty();
  void threadSetProperty();
  void threadGetChildren();
  void threadInvokeMethod();

private:
  class pqInternal;
  pqInternal* Internal;

  /// method called to initialize Python. This can be called several times. It
  /// will init Python only if it hasn't been already.
  void initPythonIfNeeded();
};

#endif // !_pqPythonEventSource_h
