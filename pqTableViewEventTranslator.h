// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#ifndef __pqTableViewEventTranslator_h
#define __pqTableViewEventTranslator_h

#include "pqAbstractItemViewEventTranslatorBase.h"

/// Event recorder for QTableView. Records the toggling of the check states for
/// Table widget items. The recorded state can be played back using
/// pqTableViewEventPlayer.
class QTTESTING_EXPORT pqTableViewEventTranslator : public pqAbstractItemViewEventTranslatorBase
{
  Q_OBJECT
  typedef pqAbstractItemViewEventTranslatorBase Superclass;

public:
  pqTableViewEventTranslator(QObject* parent = 0);
  ~pqTableViewEventTranslator() override;

  /// find and set the corrected abstract item view
  QAbstractItemView* findCorrectedAbstractItemView(QObject* object) const override;

protected Q_SLOTS:
  /// Compute a visual rectangle for the item and signal it
  void onEnteredCheck(const QModelIndex&) override;

private:
  pqTableViewEventTranslator(const pqTableViewEventTranslator&); // Not implemented.
  void operator=(const pqTableViewEventTranslator&);             // Not implemented.
};

#endif
