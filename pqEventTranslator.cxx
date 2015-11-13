/*=========================================================================

   Program: ParaView
   Module:    pqEventTranslator.cxx

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

#include "pqEventTranslator.h"

#include "pqAbstractButtonEventTranslator.h"
#include "pqAbstractItemViewEventTranslator.h"
#include "pqAbstractSliderEventTranslator.h"
#include "pqBasicWidgetEventTranslator.h"
#include "pqComboBoxEventTranslator.h"
#include "pqDoubleSpinBoxEventTranslator.h"
#include "pqEventComment.h"
#include "pqLineEditEventTranslator.h"
#include "pqMenuEventTranslator.h"
#include "pqObjectNaming.h"
#include "pqSpinBoxEventTranslator.h"
#include "pqTabBarEventTranslator.h"
#include "pqTreeViewEventTranslator.h"
#include "pqNativeFileDialogEventTranslator.h"
#include "pq3DViewEventTranslator.h"

#include <QCoreApplication>
#include <QtDebug>
#include <QSet>
#include <QVector>
#include <QPainter>
#include <QMetaProperty>

class pqCheckEventOverlay : public QWidget {
public:
  pqCheckEventOverlay(QWidget * parent = 0) : QWidget(parent) {
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    this->Valid = false;
  }

  bool Valid;
protected:
  void paintEvent(QPaintEvent *) {
    QPainter p(this);
    QPen pen(Qt::red, 5);
    if (this->Valid)
      {
      pen.setColor(Qt::green);
      }
    p.setPen(pen);
    p.drawRect(0, 0, width()-2, height()-2);
    // TODO Bug when hovering the QTabWidget, ht pqQVTKWidget seems to draw the rect as well
  }
};

////////////////////////////////////////////////////////////////////////////////
// pqEventTranslator::pqImplementation
struct pqEventTranslator::pqImplementation
{
  pqImplementation()
  {
  this->EventComment = 0;
  this->Checking = false;
  this->CheckOverlay = new pqCheckEventOverlay(NULL);
  this->CheckOverlay->hide();
  this->CheckOverlayWidgetOn = NULL;
  }

  ~pqImplementation()
  {
  if(this->EventComment)
    {
    delete this->EventComment;
    }
  delete this->CheckOverlay;
  this->CheckOverlayWidgetOn = NULL;
  }

  pqEventComment* EventComment;
  /// Stores the working set of widget translators
  QList<pqWidgetEventTranslator*> Translators;
  /// Stores the set of objects that should be ignored when translating events
  QSet<QObject*> IgnoredObjects;

  // list of widgets for which mouse propagation will happen
  // we'll only translate the first and ignore the rest
  QList<QWidget*> MouseParents;

  bool Checking;
  QPointer<pqCheckEventOverlay> CheckOverlay;
  QPointer<QWidget> CheckOverlayWidgetOn;
};

////////////////////////////////////////////////////////////////////////////////
// pqEventTranslator

// ----------------------------------------------------------------------------
pqEventTranslator::pqEventTranslator(QObject* p)
 : QObject(p),
  Implementation(new pqImplementation())
{
  this->ignoreObject(this->Implementation->CheckOverlay);
}

// ----------------------------------------------------------------------------
pqEventTranslator::~pqEventTranslator()
{
  this->stop();
  delete Implementation;
}

// ----------------------------------------------------------------------------
void pqEventTranslator::start()
{
  QCoreApplication::instance()->installEventFilter(this);
  emit this->started();
}

// ----------------------------------------------------------------------------
void pqEventTranslator::stop()
{
  QCoreApplication::instance()->removeEventFilter(this);
  this->check(false);
  emit this->stopped();
}

// ----------------------------------------------------------------------------
void pqEventTranslator::addDefaultWidgetEventTranslators(pqTestUtility* util)
{
  addWidgetEventTranslator(new pqBasicWidgetEventTranslator());
  addWidgetEventTranslator(new pqAbstractButtonEventTranslator());
  addWidgetEventTranslator(new pqAbstractItemViewEventTranslator());
  addWidgetEventTranslator(new pqAbstractSliderEventTranslator());
  addWidgetEventTranslator(new pqComboBoxEventTranslator());
  addWidgetEventTranslator(new pqDoubleSpinBoxEventTranslator());
  addWidgetEventTranslator(new pqLineEditEventTranslator());
  addWidgetEventTranslator(new pqMenuEventTranslator());
  addWidgetEventTranslator(new pqSpinBoxEventTranslator());
  addWidgetEventTranslator(new pqTabBarEventTranslator());
  addWidgetEventTranslator(new pqTreeViewEventTranslator());
  addWidgetEventTranslator(new pq3DViewEventTranslator("QGLWidget"));
  addWidgetEventTranslator(new pqNativeFileDialogEventTranslator(util));
}

// ----------------------------------------------------------------------------
void pqEventTranslator::addWidgetEventTranslator(pqWidgetEventTranslator* Translator)
{
  if(Translator)
    {
    // We Check if the translator has already been added previously.
    int index =
      this->getWidgetEventTranslatorIndex(Translator->metaObject()->className());
    if (index != -1)
      {
      return;
      }

    this->Implementation->Translators.push_front(Translator);
    Translator->setParent(this);

    QObject::connect(
      Translator,
      SIGNAL(recordEvent(QObject*, const QString&, const QString&)),
      this,
      SLOT(onRecordEvent(QObject*, const QString&, const QString&)));
    QObject::connect(
      Translator,
      SIGNAL(recordCheckEvent(QObject*, const QString&, const QString&)),
      this,
      SLOT(onRecordCheckEvent(QObject*, const QString&, const QString&)));
    }
}

// ----------------------------------------------------------------------------
bool pqEventTranslator::removeWidgetEventTranslator(const QString& className)
{
  int index = this->getWidgetEventTranslatorIndex(className);
  if (index == -1)
    {
    return false;
    }

  this->Implementation->Translators.removeAt(index);
  return true;
}

// ----------------------------------------------------------------------------
pqWidgetEventTranslator* pqEventTranslator::getWidgetEventTranslator(
    const QString& className)
{
  int index = this->getWidgetEventTranslatorIndex(className);
  if (index == -1)
    {
    return 0;
    }

  return this->Implementation->Translators.at(index);
}

// ----------------------------------------------------------------------------
int pqEventTranslator::getWidgetEventTranslatorIndex(const QString& className)
{
  for (int i = 0 ; i < this->Implementation->Translators.count() ; ++i)
    {
    if (this->Implementation->Translators.at(i)->metaObject()->className() ==
        className)
      {
      return i;
      }
    }
  return -1;
}

// ----------------------------------------------------------------------------
QList<pqWidgetEventTranslator*> pqEventTranslator::translators() const
{
  return this->Implementation->Translators;
}

// ----------------------------------------------------------------------------
void pqEventTranslator::addDefaultEventManagers(pqTestUtility* util)
{
  this->Implementation->EventComment = new pqEventComment(util);
  QObject::connect(this->Implementation->EventComment,
                   SIGNAL(recordComment(QObject*,QString,QString)),
                   this,
                   SLOT(onRecordEvent(QObject*,QString,QString)));
}

// ----------------------------------------------------------------------------
pqEventComment* pqEventTranslator::eventComment() const
{
  return this->Implementation->EventComment;
}

// ----------------------------------------------------------------------------
void pqEventTranslator::ignoreObject(QObject* Object)
{
  this->Implementation->IgnoredObjects.insert(Object);
}

// ----------------------------------------------------------------------------
bool pqEventTranslator::eventFilter(QObject* Object, QEvent* Event)
{
#if QT_VERSION >= 0x050000
  if(Object->isWindowType())
    {
    return false;
    }
#endif

  QWidget* widget = qobject_cast<QWidget*>(Object);

  // mouse events are propagated to parents
  // our event translators/players don't quite like that,
  // so lets consume those extra ones
  if(Event->type() == QEvent::MouseButtonPress ||
     Event->type() == QEvent::MouseButtonDblClick ||
     Event->type() == QEvent::MouseButtonRelease)
    {
    if(!this->Implementation->MouseParents.empty() &&
      this->Implementation->MouseParents.first() == Object)
      {
      // right on track
      this->Implementation->MouseParents.removeFirst();
      return false;
      }

    // find the chain of parent that will get this mouse event
    this->Implementation->MouseParents.clear();
    for(QWidget* w = widget->parentWidget(); w; w = w->parentWidget())
      {
      this->Implementation->MouseParents.append(w);
      if(w->isWindow() || w->testAttribute(Qt::WA_NoMousePropagation))
        {
        break;
        }
      }
    }

  if (widget != NULL && this->Implementation->Checking)
    {

    if(this->Implementation->IgnoredObjects.contains(Object))
      {
      return false;
      }

    const QMetaProperty metaProp = Object->metaObject()->userProperty();

    if (Event->type() == QEvent::MouseMove 
        && this->Implementation->CheckOverlayWidgetOn != widget)
      {
      if (widget->parentWidget() != NULL)
        { 
        this->Implementation->CheckOverlay->Valid = metaProp.isValid();
        this->Implementation->CheckOverlay->setParent(qobject_cast<QWidget*>(widget->parent()));
        this->Implementation->CheckOverlay->setGeometry(widget->geometry());
        this->Implementation->CheckOverlayWidgetOn = widget;
        this->Implementation->CheckOverlay->show();
        }
      else
        {
        this->Implementation->CheckOverlay->hide();
        this->Implementation->CheckOverlayWidgetOn = NULL;
        }
      }
    else if (this->Implementation->CheckOverlayWidgetOn == widget)
      {
      /*      if (Event->type() == QEvent::Leave) //TODO: Bug when leaving the widget, the overlay is still visible
              {
              this->Implementation->CheckOverlay->hide();
              this->Implementation->CheckOverlayWidgetOn = NULL;
              }
              else */if(Event->type() == QEvent::Resize)
        {
        this->Implementation->CheckOverlay->setGeometry(widget->geometry());
        }
      }

    if (Event->type() == QEvent::MouseButtonRelease)
      {
      if (metaProp.isValid())
        {
        QString propName = metaProp.name();
        onRecordCheckEvent(Object, propName, Object->property(propName.toAscii().data()).toString());
        return true;
        }
      else
        {
        qWarning() << "Error checking an event for object, widget type not supported:" << Object->metaObject()->className();
        }
      }
    if (dynamic_cast<QInputEvent*>(Event) != NULL)
      {
      return true;
      }
    }
  else
    {
    for(int i = 0; i != this->Implementation->Translators.size(); ++i)
      {
      bool error = false;
      if(this->Implementation->Translators[i]->translateEvent(Object, Event, error))
        {
        if(error)
          {
          qWarning() << "Error translating an event for object " << Object;
          }
        return false;
        }
      }
    }

  return false;
}

// ----------------------------------------------------------------------------
void pqEventTranslator::onRecordEvent(QObject* Object,
                                      const QString& Command,
                                      const QString& Arguments)
{
  if(this->Implementation->IgnoredObjects.contains(Object))
    return;

  QString name;
  // When sender is pqEventObject, the Object name can be NULL.
  if (!qobject_cast<pqEventComment*>(this->sender()) || Object)
    {
    name = pqObjectNaming::GetName(*Object);
    if(name.isEmpty())
      return;
    }

  emit recordEvent(name, Command, Arguments);
}

// ----------------------------------------------------------------------------
void pqEventTranslator::onRecordCheckEvent(QObject* Object,
                                      const QString& Property,
                                      const QString& Arguments)
{
  if(this->Implementation->IgnoredObjects.contains(Object))
    {
    return;
    }

  QString name = pqObjectNaming::GetName(*Object);
  if(name.isEmpty())
    {
    return;
    }
  emit recordCheckEvent(name, Property, Arguments);
}

// ----------------------------------------------------------------------------
void pqEventTranslator::check(bool value)
{
  this->Implementation->Checking = value;

  if(!value)
    {
/*    delete this->Implementation->CheckOverlay;
    this->Implementation->CheckOverlay = NULL;*/
    this->Implementation->CheckOverlay->hide();
    this->Implementation->CheckOverlayWidgetOn = NULL;
    }
}
