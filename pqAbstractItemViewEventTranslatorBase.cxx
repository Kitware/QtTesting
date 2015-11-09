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
  this->AbstractItemViewCheck = NULL;
  this->ModelItemCheck = NULL;
  this->AbstractItemViewCheckMouseTracking = false;
}

//-----------------------------------------------------------------------------
pqAbstractItemViewEventTranslatorBase::~pqAbstractItemViewEventTranslatorBase()
{
  if (this->AbstractItemViewCheck != NULL)
    {
    this->AbstractItemViewCheck->setMouseTracking(this->AbstractItemViewCheckMouseTracking);
    }
}

//-----------------------------------------------------------------------------
bool pqAbstractItemViewEventTranslatorBase::translateEvent(
  QObject* object, QEvent* event, int eventType, bool& error)
{
  QAbstractItemView* abstractItemView = qobject_cast<QAbstractItemView*>(object);
  if(!abstractItemView)
    {
    // mouse events go to the viewport widget
    abstractItemView = qobject_cast<QAbstractItemView*>(object->parent());
    }
  if(!abstractItemView)
    {
    return false;
    }

  if (eventType == pqEventTypes::EVENT)
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
            break;
            }
          if (ke->key() == Qt::Key_Escape)
            {
            this->Editing = false;
            emit this->recordEvent(abstractItemView, "editCancel", indexString);
            break;
            }
          }
        else if (ke->key() == Qt::Key_F2)
          {
          this->Editing = true;
          emit this->recordEvent(abstractItemView, "edit", indexString);
          break;
          }
        break;
        }
      case QEvent::Enter:
        {
        if (this->AbstractItemView != abstractItemView)
          {
          if(this->AbstractItemView)
            {
            QObject::disconnect(this->AbstractItemView, 0, this, 0);
            QObject::disconnect(this->AbstractItemView->selectionModel(), 0, this, 0);
            }
          this->connectWidgetToSlots(abstractItemView);
          this->AbstractItemView = abstractItemView;
          }
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
      if (this->AbstractItemViewCheck != abstractItemView)
        {
        // Disconnect precedently check item view
        if (this->AbstractItemViewCheck != NULL)
          {
          this->AbstractItemViewCheck->setMouseTracking(this->AbstractItemViewCheckMouseTracking);
          QObject::disconnect(this->AbstractItemViewCheck, SIGNAL(entered(const QModelIndex&)),
                              this, SLOT(onEnteredCheck(const QModelIndex&)));
          }

        // Keep track of checked item view
        this->AbstractItemViewCheck = abstractItemView;

        // Set mouse tracking so entered item signal are emitted
        this->AbstractItemViewCheckMouseTracking = this->AbstractItemViewCheck->hasMouseTracking();
        this->AbstractItemViewCheck->setMouseTracking(true);

        // Connect item entered event to the entered check slot
        QObject::connect(this->AbstractItemViewCheck, SIGNAL(entered(const QModelIndex&)),
                         this, SLOT(onEnteredCheck(const QModelIndex&)));
        }
      }
    // Clicking while checking, actual check event
    if (event->type() == QEvent::MouseButtonRelease)
      {
      QString indexString = this->getIndexAsString(*this->ModelItemCheck);
      emit this->recordEvent(pqEventTypes::CHECK_EVENT, abstractItemView,
                             "modelItemData", QString("%1,%2").arg(indexString).arg(
                               this->ModelItemCheck->data().toString().replace("\t", " ")));
      // Replacing tab by space, as they are not valid in xml
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
  QModelIndex curIndex = index;
  QString indexString;
  while (curIndex.isValid())
    {
    indexString.prepend(QString("%1.%2.").arg(curIndex.row()).arg(curIndex.column()));
    curIndex = curIndex.parent();
    }

  // remove the last ".".
  indexString.chop(1);
  return indexString;
}

//-----------------------------------------------------------------------------
void pqAbstractItemViewEventTranslatorBase::onCurrentChanged(const QModelIndex& index)
{
  QAbstractItemView* abstractItemView = this->AbstractItemView;

  if (abstractItemView)
    {
    // record the check state change if the item is user-checkable.
    emit this->recordEvent(abstractItemView,
      "setCurrent", this->getIndexAsString(index));
    }
}
