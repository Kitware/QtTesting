/*=========================================================================

   Program: ParaView
   Module:    pqAbstractItemViewEventTranslatorBase.cxx

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
#include "pqAbstractItemViewEventTranslatorBase.h"
#include "pqEventTypes.h"

#include <QEvent>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QVariant>

//-----------------------------------------------------------------------------
pqAbstractItemViewEventTranslatorBase::pqAbstractItemViewEventTranslatorBase(QObject* parentObject)
  : Superclass(parentObject)
{
  this->Editing = false;
  this->AbstractItemView = NULL;
  this->ModelItemCheck = NULL;
  this->AbstractItemViewMouseTracking = false;
  this->Checking = false;
}

//-----------------------------------------------------------------------------
pqAbstractItemViewEventTranslatorBase::~pqAbstractItemViewEventTranslatorBase()
{
  if (this->AbstractItemView != NULL)
    {
    this->AbstractItemView->setMouseTracking(this->AbstractItemViewMouseTracking);
    }
}

//-----------------------------------------------------------------------------
bool pqAbstractItemViewEventTranslatorBase::translateEvent(
  QObject* object, QEvent* event, int eventType, bool& error)
{
  // Recover corrected abstract item view
  QAbstractItemView* abstractItemView = this->findCorrectedAbstractItemView(object);
  if (!abstractItemView)
    {
    return false;
    }

  // Don't try to record events for QComboBox implementation details
  if (abstractItemView->inherits("QComboBoxListView"))
    {
    return false;
    }

  if (eventType == pqEventTypes::ACTION_EVENT)
    {
    switch(event->type())
      {
      case QEvent::KeyRelease:
        {
        QKeyEvent* ke = static_cast<QKeyEvent*>(event);
        QModelIndex index = abstractItemView->currentIndex();
        QString indexString = this->getIndexAsString(index);
        if (this->Editing)
          {
          if (ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return)
            {
            QVariant value = abstractItemView->model()->data(index);
            this->Editing = false;
            emit this->recordEvent(abstractItemView, "editAccepted",
              QString("%1,%2").arg(indexString, value.toString()));
            return true;
            break;
            }
          if (ke->key() == Qt::Key_Escape)
            {
            this->Editing = false;
            emit this->recordEvent(abstractItemView, "editCancel", indexString);
            return true;
            break;
            }
          }
        else if (ke->key() == Qt::Key_F2)
          {
          this->Editing = true;
          emit this->recordEvent(abstractItemView, "edit", indexString);
          return true;
          break;
          }
        break;
        }
      case QEvent::Enter:
        {
        if (this->AbstractItemView != abstractItemView || this->Checking)
          {
          //Checking flag
          this->Checking = false;
          if(this->AbstractItemView)
            {
            QObject::disconnect(this->AbstractItemView, 0, this, 0);
            QObject::disconnect(this->AbstractItemView->selectionModel(), 0, this, 0);
            }
          this->connectWidgetToSlots(abstractItemView);
          this->AbstractItemView = abstractItemView;
          }
        return true;
        break;
        }
      default:
        {
        break;
        }
      }
    }
  else if (eventType == pqEventTypes::CHECK_EVENT)
    {
    if (event->type() == QEvent::MouseMove)
      {
      // Entering while checking
      if (this->AbstractItemView != abstractItemView || !this->Checking)
        {
        //Checking flag
        this->Checking = true;

        // Disconnect precedently check item view
        if (this->AbstractItemView != NULL)
          {
          this->AbstractItemView->setMouseTracking(this->AbstractItemViewMouseTracking);
          QObject::disconnect(this->AbstractItemView, SIGNAL(entered(const QModelIndex&)),
                              this, SLOT(onEnteredCheck(const QModelIndex&)));
          QObject::disconnect(this->AbstractItemView, SIGNAL(viewportEntered()),
                              this, SLOT(onViewportEnteredCheck()));
          }

        // Keep track of checked item view
        this->AbstractItemView = abstractItemView;

        // Set mouse tracking so entered item signal are emitted
        this->AbstractItemViewMouseTracking = this->AbstractItemView->hasMouseTracking();
        this->AbstractItemView->setMouseTracking(true);

        // Connect item entered event to the entered check slot
        QObject::connect(this->AbstractItemView, SIGNAL(entered(const QModelIndex&)),
                         this, SLOT(onEnteredCheck(const QModelIndex&)));
        QObject::connect(this->AbstractItemView, SIGNAL(viewportEntered()),
                         this, SLOT(onViewportEnteredCheck()));
        }
      return true;
      }
    // Clicking while checking, actual check event
    if (event->type() == QEvent::MouseButtonRelease)
      {
      // Item Check
      if (this->ModelItemCheck != NULL)
        {
        QString indexString = this->getIndexAsString(*this->ModelItemCheck);
        emit this->recordEvent(abstractItemView, "modelItemData", QString("%1,%2").arg(indexString).arg(
            // Replacing tab by space, as they are not valid in xml
            this->ModelItemCheck->data().toString().replace("\t", " ")),
          pqEventTypes::CHECK_EVENT);
        }
      // Abstract Item View nb row check
      else
        {
        emit this->recordEvent(abstractItemView, "modelRowCount",
          QString::number(abstractItemView->model()->rowCount()), pqEventTypes::CHECK_EVENT);
        }
      return true;
      }
    }
  return this->Superclass::translateEvent(object, event, eventType, error);
}

//-----------------------------------------------------------------------------
void pqAbstractItemViewEventTranslatorBase::connectWidgetToSlots(QAbstractItemView* abstractItemView)
{
  QObject::connect(abstractItemView, SIGNAL(clicked(const QModelIndex&)),
                   this, SLOT(onClicked(const QModelIndex&)));
  QObject::connect(abstractItemView, SIGNAL(activated(const QModelIndex&)),
                   this, SLOT(onActivated(const QModelIndex&)));
  QObject::connect(abstractItemView, SIGNAL(doubleClicked(const QModelIndex&)),
                   this, SLOT(onDoubleClicked(const QModelIndex&)));
  QObject::connect(abstractItemView->selectionModel(),
    SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
    this, SLOT(onCurrentChanged(const QModelIndex&)));
}

//-----------------------------------------------------------------------------
void pqAbstractItemViewEventTranslatorBase::onClicked(
  const QModelIndex& index)
{
  static QModelIndex oldIndex;
  QAbstractItemView* abstractItemView = qobject_cast<QAbstractItemView*>(this->sender());
  QString indexString = this->getIndexAsString(index);
  if ( (index.model()->flags(index) & Qt::ItemIsUserCheckable) != 0)
    {
    // record the check state change if the item is user-checkable.
    emit this->recordEvent( abstractItemView, "setCheckState",
      QString("%1,%3").arg(indexString).arg(
        index.model()->data(index,Qt::CheckStateRole).toInt()));
    }
  if ((abstractItemView->editTriggers() & QAbstractItemView::SelectedClicked)
        == QAbstractItemView::SelectedClicked &&
      index == oldIndex)
    {
    this->Editing = true;
    emit this->recordEvent(abstractItemView, "edit", indexString);
    }
  oldIndex = index;
}

//-----------------------------------------------------------------------------
void pqAbstractItemViewEventTranslatorBase::onActivated(const QModelIndex & index)
{
  QAbstractItemView* abstractItemView = qobject_cast<QAbstractItemView*>(this->sender());
  QString indexString = this->getIndexAsString(index);
  emit this->recordEvent(abstractItemView, "activate", indexString);
}

//-----------------------------------------------------------------------------
void pqAbstractItemViewEventTranslatorBase::onDoubleClicked(const QModelIndex& index)
{
  QAbstractItemView* abstractItemView = qobject_cast<QAbstractItemView*>(this->sender());
  QString indexString = this->getIndexAsString(index);
  // record the check state change if the item is user-checkable.
  if ((abstractItemView->editTriggers() & QAbstractItemView::DoubleClicked)
      == QAbstractItemView::DoubleClicked)
    {
    this->Editing = true;
    emit this->recordEvent(abstractItemView, "edit", indexString);
    }
}

//-----------------------------------------------------------------------------
QString pqAbstractItemViewEventTranslatorBase::getIndexAsString(const QModelIndex& index)
{
  QString indexString;
  for (QModelIndex curIndex = index; curIndex.isValid(); curIndex = curIndex.parent())
    {
    indexString.prepend(QString("%1.%2.").arg(curIndex.row()).arg(curIndex.column()));
    }

  // remove the last ".".
  indexString.chop(1);
  return indexString;
}

//-----------------------------------------------------------------------------
void pqAbstractItemViewEventTranslatorBase::onCurrentChanged(const QModelIndex& index)
{
  emit this->recordEvent(this->AbstractItemView,
    "setCurrent", this->getIndexAsString(index));
}

//-----------------------------------------------------------------------------
void pqAbstractItemViewEventTranslatorBase::onViewportEnteredCheck()
{
  this->ModelItemCheck = NULL;
  emit this->specificOverlay(this->AbstractItemView->rect());
}

