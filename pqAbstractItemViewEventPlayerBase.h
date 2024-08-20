// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#ifndef __pqAbstractItemViewEventPlayerBase_h
#define __pqAbstractItemViewEventPlayerBase_h

#include "pqWidgetEventPlayer.h"
#include <QModelIndex>

class QAbstractItemView;

/// pqAbstractItemViewEventPlayerBase is a player for QAbstractItemViewWidget. Plays back the state
/// recorded using pQAbstractItemViewEventTranslatorBase.
class QTTESTING_EXPORT pqAbstractItemViewEventPlayerBase : public pqWidgetEventPlayer
{
  Q_OBJECT
  typedef pqWidgetEventPlayer Superclass;

public:
  pqAbstractItemViewEventPlayerBase(QObject* parent = 0);
  ~pqAbstractItemViewEventPlayerBase() override = 0;

  /// Play an event on a QAbstractViewItem
  using Superclass::playEvent;
  bool playEvent(QObject* object, const QString& command, const QString& arguments, int eventType,
    bool& error) override;

protected:
  /// Get index using a previously recorded string
  static QModelIndex GetIndex(
    const QString& str_index, QAbstractItemView* abstractItemView, bool& error);

  /// Get data string using a previously recorded string
  static QString GetDataString(const QString& str_index, bool& error);

private:
  pqAbstractItemViewEventPlayerBase(const pqAbstractItemViewEventPlayerBase&); // Not implemented.
  void operator=(const pqAbstractItemViewEventPlayerBase&);                    // Not implemented.
};

#endif
