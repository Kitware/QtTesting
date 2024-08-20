// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef __pqEventComment_h
#define __pqEventComment_h

// Qt includes
#include <QObject>

// QtTesting inlcudes
#include "QtTestingExport.h"
#include "pqTestUtility.h"

/// pqEventComment is responsible for adding any kind of events that are not added
/// by widgets.
/// For exemple, you can add an event to block the playback, to show a custom
/// comment etc ...

class QTTESTING_EXPORT pqEventComment : public QObject
{
  Q_OBJECT
  typedef QObject Superclass;

public:
  pqEventComment(pqTestUtility* util, QObject* parent = 0);
  ~pqEventComment() override;

  /// Call this function to add an event comment, which will display a message,
  /// during the playback in the log.
  void recordComment(const QString& arguments);

  /// Call this function to add an event comment, which will display a message,
  /// and then pause the macro during the playback.
  void recordCommentBlock(const QString& arguments);

Q_SIGNALS:
  /// All functions should emit this signal whenever they wish to record comment event
  void recordComment(QObject* widget, const QString& type, const QString& argument);

protected:
  void recordComment(const QString& command, const QString& arguments, QObject* = 0);

  pqTestUtility* TestUtility;
};

#endif
