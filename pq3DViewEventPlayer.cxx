/*=========================================================================

   Program: ParaView
   Module:    pq3DViewEventPlayer.cxx

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

#include "pq3DViewEventPlayer.h"

#include <QApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QtDebug>
#include <QRegExp>
#include <QWidget>

pq3DViewEventPlayer::pq3DViewEventPlayer(const QByteArray& classname, QObject * p)
      : pqWidgetEventPlayer(p), mClassType(classname)
{
}

bool pq3DViewEventPlayer::playEvent(QObject* Object,
                                    const QString& Command,
                                    const QString& Arguments,
                                    bool& Error)
{
    QWidget* widget = qobject_cast<QWidget*>(Object);
    if(widget && Object->inherits(mClassType.data()))
      {
      if (Command == "mousePress" || Command=="mouseRelease" || Command=="mouseMove")
        {
        QRegExp mouseRegExp("\\(([^,]*),([^,]*),([^,]),([^,]),([^,]*)\\)");
        if (mouseRegExp.indexIn(Arguments)!= -1)
          {
          QVariant v = mouseRegExp.cap(1);
          int x = static_cast<int>(v.toDouble() * widget->size().width());
          v = mouseRegExp.cap(2);
          int y = static_cast<int>(v.toDouble() * widget->size().height());
          v = mouseRegExp.cap(3);
          Qt::MouseButton button = static_cast<Qt::MouseButton>(v.toInt());
          v = mouseRegExp.cap(4);
          Qt::MouseButtons buttons = static_cast<Qt::MouseButton>(v.toInt());
          v = mouseRegExp.cap(5);
          Qt::KeyboardModifiers keym = static_cast<Qt::KeyboardModifier>(v.toInt());
          QEvent::Type type = (Command == "mousePress")? QEvent::MouseButtonPress :
            ((Command=="mouseMove")?  QEvent::MouseMove : QEvent::MouseButtonRelease);
          QMouseEvent e(type, QPoint(x,y), button, buttons, keym);
          qApp->notify(Object, &e);
          }
        return true;
        }
      }
    return this->Superclass::playEvent(Object, Command, Arguments, Error);
}

