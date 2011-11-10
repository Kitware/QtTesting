/*=========================================================================

   Program: ParaView
   Module:    pqTabBarEventPlayer.cxx

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

#include "pqTabBarEventPlayer.h"

#include <QComboBox>
#include <QLineEdit>
#include <QtDebug>

pqTabBarEventPlayer::pqTabBarEventPlayer(QObject* p)
  : pqWidgetEventPlayer(p)
{
}

bool pqTabBarEventPlayer::playEvent(
  QObject* target, const QString& command, const QString& arguments, bool& error_flag)
{
  if (command != "set_tab"  && command != "set_tab_with_text")
    {
    // I don't handle this. Return false
    return false;
    }

  const QString value = arguments;
    
  QTabBar* const tab_bar = qobject_cast<QTabBar*>(target);
  if (tab_bar && command=="set_tab")
    {
    // "set_tab" saves tab index. This was done in the past. Newly recorded
    // tests will use set_tab_with_text for more reliable playback.
    bool ok = false;
    int which = value.toInt(&ok);
    if(!ok)
      {
      qCritical() << "calling set_tab with invalid argument on " << target;
      error_flag = true;
      }
    else if(tab_bar->count() < which)
      {
      qCritical() << "calling set_tab with out of bounds index on " << target;
      error_flag = true;
      }
    else
      {
      tab_bar->setCurrentIndex(which);
      }
    return true;
    }

  if (tab_bar && command == "set_tab_with_text")
    {
    for (int cc=0 ; cc < tab_bar->count(); cc++)
      {
      if (tab_bar->tabText(cc) == value)
        {
        tab_bar->setCurrentIndex(cc);
        return true;
        }
      }
    qCritical() << "calling set_tab with unknown tab " << value;
    error_flag = true;
    return true;
    }

  qCritical() << "calling set_tab on unhandled type " << target;

  error_flag = true;
  return true;
}

