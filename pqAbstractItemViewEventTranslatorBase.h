// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause
#ifndef __pqAbstractItemViewEventTranslatorBase_h
#define __pqAbstractItemViewEventTranslatorBase_h

#include "pqWidgetEventTranslator.h"
#include <QAbstractItemModel> // for QModelIndexList
#include <QPointer>
#include <QtGlobal>

class QModelIndex;
class QAbstractItemView;
class QItemSelection;
class QItemSelectionModel;

/// Event recorder for QAbstractItemView. Records the toggling of the check states for
/// tree widget items. The recorded state can be played back using
/// pqTreeViewEventPlayer.
class QTTESTING_EXPORT pqAbstractItemViewEventTranslatorBase : public pqWidgetEventTranslator
{
  Q_OBJECT
  typedef pqWidgetEventTranslator Superclass;

public:
  pqAbstractItemViewEventTranslatorBase(QObject* parent = 0);
  ~pqAbstractItemViewEventTranslatorBase() override;

  /// Overridden to handle events on QAbstractItemView
  using pqWidgetEventTranslator::translateEvent;
  bool translateEvent(QObject* object, QEvent* event, int eventType, bool& error) override;

  /// find and set the corrected abstract item view
  virtual QAbstractItemView* findCorrectedAbstractItemView(QObject* object) const = 0;

protected Q_SLOTS:
  virtual void onClicked(const QModelIndex&);
  virtual void onActivated(const QModelIndex&);
  virtual void onDoubleClicked(const QModelIndex&);
  virtual void onCurrentChanged(const QModelIndex&);
  virtual void onSelectionChanged(const QItemSelection&);

  /// Compute a visual rectangle for the item and signal it
  virtual void onEnteredCheck(const QModelIndex&) = 0;
  virtual void onViewportEnteredCheck();

protected:
  QString getIndexAsString(const QModelIndex& item);
  QString getIndicesAsString(const QModelIndexList& selectedIndices);

  /// Record events from @a abstractItemView when signals are fired.
  /// @sa monitorSignalsInternal()
  void monitorSignals(QAbstractItemView* abstractItemView);

  /// Subclasses (pqTreeViewEventTranslator, possibly others) override this to monitor
  /// additional view specific signals
  virtual void monitorSignalsInternal(QAbstractItemView* abstractItemView)
  {
    Q_UNUSED(abstractItemView);
  }

  QPointer<QAbstractItemView> AbstractItemView;
  QPointer<QItemSelectionModel> ItemSelectionModel;
  const QModelIndex* ModelItemCheck;
  bool Editing;
  bool AbstractItemViewMouseTracking;
  bool Checking;
  QPoint LastPos;

private:
  pqAbstractItemViewEventTranslatorBase(
    const pqAbstractItemViewEventTranslatorBase&);              // Not implemented.
  void operator=(const pqAbstractItemViewEventTranslatorBase&); // Not implemented.
};

#endif
