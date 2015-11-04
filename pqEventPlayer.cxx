/*=========================================================================

   Program: ParaView
   Module:    pqEventPlayer.cxx

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

#include "pqEventPlayer.h"

#include "pqAbstractActivateEventPlayer.h"
#include "pqAbstractBooleanEventPlayer.h"
#include "pqAbstractDoubleEventPlayer.h"
#include "pqAbstractIntEventPlayer.h"
#include "pqAbstractItemViewEventPlayer.h"
#include "pqAbstractMiscellaneousEventPlayer.h"
#include "pqAbstractStringEventPlayer.h"
#include "pqBasicWidgetEventPlayer.h"
#include "pqCommentEventPlayer.h"
#include "pqObjectNaming.h"
#include "pqTabBarEventPlayer.h"
#include "pqTreeViewEventPlayer.h"
#include "pqTableViewEventPlayer.h"
#include "pqNativeFileDialogEventPlayer.h"
#include "pq3DViewEventPlayer.h"
#include "pqEventTypes.h"

#include <QApplication>
#include <QWidget>
#include <QDebug>

// ----------------------------------------------------------------------------
pqEventPlayer::pqEventPlayer()
{
}

// ----------------------------------------------------------------------------
pqEventPlayer::~pqEventPlayer()
{
}

// ----------------------------------------------------------------------------
void pqEventPlayer::addDefaultWidgetEventPlayers(pqTestUtility* util)
{
  addWidgetEventPlayer(new pqCommentEventPlayer(util));
  addWidgetEventPlayer(new pqBasicWidgetEventPlayer());
  addWidgetEventPlayer(new pqAbstractActivateEventPlayer());
  addWidgetEventPlayer(new pqAbstractBooleanEventPlayer());
  addWidgetEventPlayer(new pqAbstractDoubleEventPlayer());
  addWidgetEventPlayer(new pqAbstractIntEventPlayer());
  addWidgetEventPlayer(new pqAbstractItemViewEventPlayer());
  addWidgetEventPlayer(new pqAbstractStringEventPlayer());
  addWidgetEventPlayer(new pqTabBarEventPlayer());
  addWidgetEventPlayer(new pqTreeViewEventPlayer());
  addWidgetEventPlayer(new pqTableViewEventPlayer());
  addWidgetEventPlayer(new pqAbstractMiscellaneousEventPlayer());
  addWidgetEventPlayer(new pq3DViewEventPlayer("QGLWidget"));
  addWidgetEventPlayer(new pqNativeFileDialogEventPlayer(util));
}

// ----------------------------------------------------------------------------
QList<pqWidgetEventPlayer*> pqEventPlayer::players() const
{
  return this->Players;
}

// ----------------------------------------------------------------------------
void pqEventPlayer::addWidgetEventPlayer(pqWidgetEventPlayer* Player)
{
  if(Player)
    {
    // We Check if the Player has already been added previously
    int index =
      this->getWidgetEventPlayerIndex(QString(Player->metaObject()->className()));
    if (index != -1)
      {
      return;
      }

    this->Players.push_front(Player);
    Player->setParent(this);
    }
}

// ----------------------------------------------------------------------------
bool pqEventPlayer::removeWidgetEventPlayer(const QString& className)
{
  int index = this->getWidgetEventPlayerIndex(className);
  if (index == -1)
    {
    return false;
    }

  this->Players.removeAt(index);
  return true;
}

// ----------------------------------------------------------------------------
pqWidgetEventPlayer* pqEventPlayer::getWidgetEventPlayer(const QString& className)
{
  int index = this->getWidgetEventPlayerIndex(className);
  if (index == -1)
    {
    return 0;
    }

  return this->Players.at(index);
}

// ----------------------------------------------------------------------------
int pqEventPlayer::getWidgetEventPlayerIndex(const QString& className)
{
  for(int i = 0 ; i < this->Players.count() ; ++i)
    {
    if (this->Players.at(i)->metaObject()->className() == className)
      {
      return i;
      }
    }
  return -1;
}

// ----------------------------------------------------------------------------
void pqEventPlayer::playEvent(const QString& objectString,
                              const QString& command,
                              const QString& arguments,
                              bool& error)
{
  emit this->eventAboutToBePlayed(objectString, command, arguments);
  // If we can't find an object with the right name, we're done ...
  QObject* const object = pqObjectNaming::GetObject(objectString);

  // Scroll bar depends on monitor's resolution
  if(!object && objectString.contains(QString("QScrollBar")))
    {
    emit this->eventPlayed(objectString, command, arguments);
    error = false;
    return;
    }

  if(!object && !command.startsWith("comment"))
    {
    qCritical() << pqObjectNaming::lastErrorMessage();
    emit this->errorMessage(pqObjectNaming::lastErrorMessage());
    error = true;
    return;
    }

  // Loop through players until the event gets handled ...
  bool accepted = false;
  bool tmpError = false;
  if (command.startsWith("comment"))
    {
    pqWidgetEventPlayer* widgetPlayer =
        this->getWidgetEventPlayer(QString("pqCommentEventPlayer"));
    pqCommentEventPlayer* commentPlayer =
        qobject_cast<pqCommentEventPlayer*>(widgetPlayer);
    if (commentPlayer)
      {
      accepted = commentPlayer->playEvent(object, command, arguments, tmpError);
      }
    }
  else
    {
    for(int i = 0; i != this->Players.size(); ++i)
      {
      accepted = this->Players[i]->playEvent(object, command, arguments, pqEventTypes::EVENT, tmpError);
      if(accepted)
        {
        break;
        }
      }
    }

  // The event wasn't handled at all ...
  if(!accepted)
    {
    QString errorMessage =
        QString("Unhandled event %1 object %2\n")
          .arg(command, object ? object->objectName() : objectString);
    qCritical() << errorMessage;
    emit this->errorMessage(errorMessage);
    error = true;
    return;
    }

  // The event was handled, but there was a problem ...
  if(accepted && tmpError)
    {
    QString errorMessage =
        QString("Event error %1 object %2\n")
          .arg(command, object ? object->objectName() : objectString);
    qCritical() << errorMessage;
    emit this->errorMessage(errorMessage);
    error = true;
    return;
    }

  // The event was handled successfully ...
  emit this->eventPlayed(objectString, command, arguments);
  error = false;
}

// ----------------------------------------------------------------------------
void pqEventPlayer::playCheckEvent(const QString& objectString,
                                   const QString& property,
                                   const QString& arguments,
                                   bool& error)
{
  // Inform world about playing event
  emit this->eventAboutToBePlayed(objectString, property, arguments);

  // Recover QObject from it's name
  QObject* const object = pqObjectNaming::GetObject(objectString);

  // Check for object existence
  if(!object)
    {
    qCritical() << pqObjectNaming::lastErrorMessage();
    emit this->errorMessage(pqObjectNaming::lastErrorMessage());
    error = true;
    return;
    }

  // Try to play check event using players
  for(int i = 0; i != this->Players.size(); ++i)
    {
    if (this->Players[i]->playEvent(object, property, arguments, pqEventTypes::CHECK_EVENT, error))
      {
      if (error)
        {
        QString errorMessage = "Error processing " + property + " for " + objectString;
        qCritical() << errorMessage.toAscii().data();
        emit this->errorMessage(errorMessage.toAscii().data());
        }
      return;
      }
    }

  // Recover QProperty
  QVariant propertyValue = object->property(property.toAscii().data());

  // Check it is valid
  if (!propertyValue.isValid())
    {
    QString errorMessage = objectString + " has no valid property named:" + property;
    qCritical() << errorMessage.toAscii().data();
    emit this->errorMessage(errorMessage.toAscii().data());
    error = true;
    return;
    }

  // Check property value
  if (propertyValue.toString() != arguments)
    {
    QString errorMessage = objectString + " property value is: " + propertyValue.toString()
      + ". Expecting: "+ arguments + ".";
    qCritical() << errorMessage.toAscii().data();
    emit this->errorMessage(errorMessage.toAscii().data());
    error = true;
    return;
    }

  // The check event was correctly played.
  emit this->eventPlayed(objectString, property, arguments);
  error = false;
}

