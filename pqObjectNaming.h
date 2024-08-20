// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqObjectNaming_h
#define _pqObjectNaming_h

#include <QString>

#include "QtTestingExport.h"

class QObject;
#if QT_VERSION < 0x060000
class QStringList;
#endif

/// Provides functionality to ensuring that Qt objects can be uniquely identified for recording and
/// playback of regression tests
class QTTESTING_EXPORT pqObjectNaming
{
public:
  /// Returns a unique identifier for the given object that can be serialized for later regression
  /// test playback
  static const QString GetName(QObject& Object);
  /// Given a unique identifier returned by GetName(), returns the corresponding object, or NULL
  static QObject* GetObject(const QString& Name);

  /** Dumps the widget hierarchy to a string */
  static void DumpHierarchy(QStringList& results);
  /** Dumps a subtree of the widget hierarchy to a string */
  static void DumpHierarchy(QObject& Object, QStringList& results);

  /// Recover the last error message
  static QString lastErrorMessage();
};

#endif // !_pqObjectNaming_h
