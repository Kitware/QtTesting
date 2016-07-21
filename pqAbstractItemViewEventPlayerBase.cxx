/*=========================================================================

   Program: ParaView
   Module:    pqAbstractItemViewEventPlayerBase.cxx

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
#include "pqAbstractItemViewEventPlayerBase.h"
#include "pqEventTypes.h"

//#include <QCoreApplication>
//#include <QEvent>
//#include <QKeyEvent>
#include <QAbstractItemView>
#include <QDebug>
#include <QMenu>
//-----------------------------------------------------------------------------
pqAbstractItemViewEventPlayerBase::pqAbstractItemViewEventPlayerBase(QObject* parentObject)
  : Superclass(parentObject)
{
}

//-----------------------------------------------------------------------------
pqAbstractItemViewEventPlayerBase::~pqAbstractItemViewEventPlayerBase()
{
}

//-----------------------------------------------------------------------------
QModelIndex pqAbstractItemViewEventPlayerBase::GetIndex(const QString& itemStr,
  QAbstractItemView* abstractItemView, bool &error)
{
  // Get only the "index" part of the string
  int sep = itemStr.indexOf(",");
  QString strIndex = itemStr.left(sep);

  // Recover model index
  QStringList indices = strIndex.split(".",QString::SkipEmptyParts);
  QModelIndex index;
  if(indices.size() < 2)
    {
    error = true;
    return index;
    }

  index = abstractItemView->model()->index(indices[0].toInt(), indices[1].toInt(), index);
  for (int cc=2; (cc+1) < indices.size(); cc+=2)
    {
    index = abstractItemView->model()->index(indices[cc].toInt(), indices[cc+1].toInt(),
                                             index);
    if (!index.isValid())
      {
      error=true;
      qCritical() << "ERROR: Abstract Item view must have changed. "
        << "Indices recorded in the test are no longer valid. Cannot playback.";
      break;
      }
    }
  return index;
}

//-----------------------------------------------------------------------------
QString pqAbstractItemViewEventPlayerBase::GetDataString(const QString& itemStr, bool &error)
{
  // Get only the "data" part of the string
  int sep = itemStr.indexOf(",");
  return itemStr.mid(sep+1);
}
//-----------------------------------------------------------------------------
bool pqAbstractItemViewEventPlayerBase::playEvent(
  QObject* object, const QString& command,
  const QString& arguments, int eventType, bool& error)
{
  QAbstractItemView* abstractItemView= qobject_cast<QAbstractItemView*>(object);
  QMenu* contextMenu= qobject_cast<QMenu*>(object);
  // if this a QMenu (potentially a context menu of the view),
  // we should not move onto parent
  if(!abstractItemView && !contextMenu)
    {
    // mouse events go to the viewport widget
    abstractItemView = qobject_cast<QAbstractItemView*>(object->parent());
    }
  if(!abstractItemView)
    {
    return false;
    }

  if (eventType == pqEventTypes::ACTION_EVENT)
    {
    if (command == "key")
      {
      return false;
      }

    QRegExp regExp1("^([\\d\\.]+),(\\d+)$");
    if (command == "setCheckState" && regExp1.indexIn(arguments) != -1)
      {
      QString strIndex = regExp1.cap(1);
      int check_state = regExp1.cap(2).toInt();

      QModelIndex index = pqAbstractItemViewEventPlayerBase::GetIndex(strIndex, abstractItemView, error);
      if (error)
        {
        return true;
        }
      if (abstractItemView->model()->data(index,
        Qt::CheckStateRole).toInt() != check_state)
        {
        abstractItemView->model()->setData(index,
          static_cast<Qt::CheckState>(check_state),
          Qt::CheckStateRole);
        }
      return true;
      }
    else if(command == "edit")
      {
      QString strIndex = arguments;
      QModelIndex index = pqAbstractItemViewEventPlayerBase::GetIndex(strIndex, abstractItemView, error);
      if (error)
        {
        return true;
        }
      abstractItemView->edit(index);
      return true;
      }
    else if (command == "editCancel")
      {
      QString strIndex = arguments;
      QModelIndex index = pqAbstractItemViewEventPlayerBase::GetIndex(strIndex, abstractItemView, error);
      abstractItemView->closePersistentEditor(index);
      return true;
      }
    else if (command == "editAccepted")
      {
      QStringList list = arguments.split(',');
      QModelIndex index = pqAbstractItemViewEventPlayerBase::GetIndex(list.at(0), abstractItemView, error);
      QVariant value = QVariant(list.at(1));
      abstractItemView->model()->setData(index, value);
      abstractItemView->closePersistentEditor(index);
      return true;
      }
    else if (command == "setCurrent" || command == "activate")
      {
      QString strIndex = arguments;
      QModelIndex index = pqAbstractItemViewEventPlayerBase::GetIndex(strIndex, abstractItemView, error);
      if (error)
        {
        return true;
        }
      abstractItemView->setFocus();
      abstractItemView->setCurrentIndex(index);
      // The following code will cause a modal dialog to close,
      // such as the pqColorPresetDialog, if the abstractItemView widget is not handling the key event,
      // the key event will bevpropogated to the abstractItemView's parent, in this case the dialog,
      // and the dialog be default has OK/Cancel button in focus, and will process the key event,
      // close itself. This is causing a few a paraview's tests using pqColorPresetDialog to fail.
      // The fix here (above) is to add "abstractItemView->setFocus();" so that the
      // "abstractItemView->setCurrentIndex(index);" will trigger selection changed event in abstractItemView,
      // which is the purpose of the following code.
      /*
         QKeyEvent kd(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
         QKeyEvent ku(QEvent::KeyRelease, Qt::Key_Enter, Qt::NoModifier);
         QCoreApplication::sendEvent(abstractItemView, &kd);
         QCoreApplication::sendEvent(abstractItemView, &ku);
       */

      return true;
      }
    }
  else if (eventType == pqEventTypes::CHECK_EVENT)
    {
    if (command == "modelItemData")
      {
      // Recover index to check
      QString itemString = arguments;
      QModelIndex index = pqAbstractItemViewEventPlayerBase::GetIndex(itemString, abstractItemView, error);
      if (error)
        {
        return true;
        }
      // Recover Data to check
      QString testDataString = pqAbstractItemViewEventPlayerBase::GetDataString(itemString, error);
      if (error)
        {
        return true;
        }
      // Recover current data, replacing tab by space as they are not existing in xml
      QString currentDataString = index.data().toString();
      currentDataString.replace("\t", " ");

      // Compare strings
      if (currentDataString != testDataString)
        {
        qCritical() << "ERROR: Checked item contain :"
          << index.data().toString() << ".Expecting :"
          << testDataString;
        error = true;
        }
      return true;
      }
    else if (command == "modelRowCount")
      {
      if (abstractItemView->model()->rowCount() != arguments.toInt())
        {
        qCritical() << "ERROR: Checked abstract item view has "
          <<  abstractItemView->model()->rowCount() << " rows. Expecting :"
          << arguments;
        error = true;
        }
      return true;
      }
    }
  return this->Superclass::playEvent(object, command, arguments, eventType, error);
}

