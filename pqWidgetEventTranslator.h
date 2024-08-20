// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqWidgetEventTranslator_h
#define _pqWidgetEventTranslator_h

#include "QtTestingExport.h"
#include <QObject>
#include <QRect>

/**
Abstract interface for an object that can translate
low-level Qt events into high-level, serializable ParaView
events, for test-cases, demos, tutorials, etc.

\sa pqEventTranslator
*/
class QTTESTING_EXPORT pqWidgetEventTranslator : public QObject
{
  Q_OBJECT

  typedef QObject Superclass;

public:
  pqWidgetEventTranslator(QObject* p = 0);
  ~pqWidgetEventTranslator() override;

  /** Derivatives should implement this and translate events into commands,
  returning "true" if they handled the event, and setting Error
  to "true" if there were any problems. eventType allow to specify different types of events
  like check event*/
  virtual bool translateEvent(QObject* object, QEvent* event, bool& error);
  virtual bool translateEvent(QObject* object, QEvent* event, int eventType, bool& error);

Q_SIGNALS:
  /// Derivatives should emit this signal whenever they wish to record a high-level event
  void recordEvent(
    QObject* Object, const QString& Command, const QString& Arguments, int eventType);
  void recordEvent(QObject* Object, const QString& Command, const QString& Arguments);
  void specificOverlay(const QRect& geometry);

protected:
  pqWidgetEventTranslator(const pqWidgetEventTranslator&);
  pqWidgetEventTranslator& operator=(const pqWidgetEventTranslator&);
};

#endif // !_pqWidgetEventTranslator_h
