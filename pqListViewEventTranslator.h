// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#ifndef __pqListViewEventTranslator_h
#define __pqListViewEventTranslator_h

#include "pqAbstractItemViewEventTranslatorBase.h"

/// Event recorder for QTableView. Records the toggling of the check states for
/// Table widget items. The recorded state can be played back using
/// pqListViewEventPlayer.
class QTTESTING_EXPORT pqListViewEventTranslator : public pqAbstractItemViewEventTranslatorBase
{
  Q_OBJECT
  typedef pqAbstractItemViewEventTranslatorBase Superclass;

public:
  pqListViewEventTranslator(QObject* parent = 0);
  ~pqListViewEventTranslator() override;

  /// find and set the corrected abstract item view
  QAbstractItemView* findCorrectedAbstractItemView(QObject* object) const override;

protected Q_SLOTS:
  /// Compute a visual rectangle for the item and signal it
  void onEnteredCheck(const QModelIndex&) override;

private:
  pqListViewEventTranslator(const pqListViewEventTranslator&); // Not implemented.
  void operator=(const pqListViewEventTranslator&);            // Not implemented.
};

#endif
