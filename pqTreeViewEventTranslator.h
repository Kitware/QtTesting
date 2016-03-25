/*=========================================================================

   Program: ParaView
   Module:    pqTreeViewEventTranslator.h

   Copyright (c) 2005,2006 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2.

   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

========================================================================*/
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
  pqTreeViewEventTranslator(QObject* parent=0);
  ~pqTreeViewEventTranslator();

  /// Connect QTree signals to this class slots
  virtual void connectWidgetToSlots(QAbstractItemView* abstractItemView);

  /// find and set the corrected abstract item view
  virtual QAbstractItemView* findCorrectedAbstractItemView(QObject* object) const;

protected slots:
  void onExpanded(const QModelIndex&);
  void onCollapsed(const QModelIndex&);

  /// Compute a visual rectangle for the item and signal it
  void onEnteredCheck(const QModelIndex&);

private:
  pqTreeViewEventTranslator(const pqTreeViewEventTranslator&); // Not implemented.
  void operator=(const pqTreeViewEventTranslator&); // Not implemented.
};

#endif
