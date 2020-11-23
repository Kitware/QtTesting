/*=========================================================================

   Program: ParaView
   Module:    pqTabBarEventTranslator.cxx

   Copyright (c) 2005-2008 Sandia Corporation, Kitware Inc.
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

=========================================================================*/

#include "pqTabBarEventTranslator.h"

#include <QEvent>
#include <QMainWindow>
#include <QTabBar>

pqTabBarEventTranslator::pqTabBarEventTranslator(QObject* p)
  : pqWidgetEventTranslator(p)
  , CurrentObject(0)
{
}

bool pqTabBarEventTranslator::translateEvent(QObject* Object, QEvent* Event, bool& Error)
{
  QTabBar* const object = qobject_cast<QTabBar*>(Object);
  if (!object)
  {
    return false;
  }

  switch (Event->type())
  {
    case QEvent::Enter:
    {
      if (this->CurrentObject != Object)
      {
        if (this->CurrentObject)
        {
          disconnect(this->CurrentObject, 0, this, 0);
        }

        this->CurrentObject = object;
        connect(object, SIGNAL(currentChanged(int)), this, SLOT(indexChanged(int)));
      }
      return true;
      break;
    }
    default:
      break;
  }
  return this->Superclass::translateEvent(Object, Event, Error);
}

void pqTabBarEventTranslator::indexChanged(int which)
{

  // The QMainWindow preemptively creates unnamed QTabBar's for each dock area and other uses that I
  // couldn't determine.  The objects are instantiated in an unspecified order, so the default
  // unnamed object finding logic just doesn't work reliably.  So instead we will ask the
  // QMainWindow itself to find a tab with the given name from the pqTabBarEventPlayer.  The risk
  // with this approach is that when multiple DockWidgets have the same windowTitle() (which
  // determines the tabname) then we will sometimes pick the wrong one.  This can be mediated in
  // client code by ensuring that each DockWidget has a unique windowTitle() (which they probably
  // should anyway)
  QObject* recordedObject = this->CurrentObject;
  if (QObject* parent = this->CurrentObject->parent();
      parent && this->CurrentObject->objectName().isEmpty())
  {
    QMainWindow* mainWindow = qobject_cast<QMainWindow*>(this->CurrentObject->parent());
    if (mainWindow)
    {
      recordedObject = mainWindow;
    }
  }

  emit recordEvent(recordedObject, "set_tab_with_text", this->CurrentObject->tabText(which));
}
