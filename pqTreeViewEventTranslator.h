// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#ifndef __pqTreeViewEventTranslator_h
#define __pqTreeViewEventTranslator_h

#include "pqAbstractItemViewEventTranslatorBase.h"

/// Event recorder for QTreeView. Records the toggling of the check states for
/// tree widget items. The recorded state can be played back using
/// pqTreeViewEventPlayer.
class QTTESTING_EXPORT pqTreeViewEventTranslator : public pqAbstractItemViewEventTranslatorBase
{
  Q_OBJECT
  typedef pqAbstractItemViewEventTranslatorBase Superclass;

public:
  pqTreeViewEventTranslator(QObject* parent = 0);
  ~pqTreeViewEventTranslator() override;

  /// find and set the corrected abstract item view
  QAbstractItemView* findCorrectedAbstractItemView(QObject* object) const override;

protected Q_SLOTS:
  void onExpanded(const QModelIndex&);
  void onCollapsed(const QModelIndex&);

  /// Compute a visual rectangle for the item and signal it
  void onEnteredCheck(const QModelIndex&) override;

protected:
  /// Connect QTree signals to this class slots
  void monitorSignalsInternal(QAbstractItemView* abstractItemView) override;

private:
  pqTreeViewEventTranslator(const pqTreeViewEventTranslator&); // Not implemented.
  void operator=(const pqTreeViewEventTranslator&);            // Not implemented.
};

#endif
