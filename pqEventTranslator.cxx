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

#include "pq3DViewEventTranslator.h"
#include "pqAbstractButtonEventTranslator.h"
#include "pqAbstractItemViewEventTranslator.h"
#include "pqAbstractSliderEventTranslator.h"
#include "pqBasicWidgetEventTranslator.h"
#include "pqComboBoxEventTranslator.h"
#include "pqDoubleSpinBoxEventTranslator.h"
#include "pqEventComment.h"
#include "pqEventTypes.h"
#include "pqLineEditEventTranslator.h"
#include "pqMenuEventTranslator.h"
#include "pqNativeFileDialogEventTranslator.h"
#include "pqObjectNaming.h"
#include "pqSpinBoxEventTranslator.h"
#include "pqTabBarEventTranslator.h"
#include "pqTreeViewEventTranslator.h"

#include <QCoreApplication>
#include <QtDebug>
#include <QSet>
#include <QVector>
#include <QPainter>
#include <QMetaProperty>

#include <iostream>

class pqCheckEventOverlay : public QWidget {
public:
  pqCheckEventOverlay(QWidget * parent = 0) : QWidget(parent) {
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    this->Valid = false;
    this->GlWidget = false;
  }

  bool Valid;
  bool GlWidget;
  static const int OVERLAY_MARGIN = 2;
  static const int OVERLAY_PEN_WIDTH = 5;

protected:
  void paintEvent(QPaintEvent *) {
    QPainter p(this);
    // Draw red on invalid widget
    QPen pen(Qt::red, OVERLAY_PEN_WIDTH);
    if (this->Valid)
      {
      // Draw green on valid widget
      pen.setColor(Qt::green);
      }
    p.setPen(pen);
    // -2 for the line width
    p.drawRect(0, 0, width()-OVERLAY_MARGIN, height()-OVERLAY_MARGIN);
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
  this->Recording = false;

  // Create overlay and hide it
  this->CheckOverlay = new pqCheckEventOverlay(NULL);
  this->hideOverlay();
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

  void hideOverlay()
    {
    this->CheckOverlay->hide();
    this->CheckOverlay->setParent(NULL);
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
  bool Recording;
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
  // Ignore the overlay so it is transparent to events.
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

    // Legacy Connection, for translator not using event types
    QObject::connect(
      Translator,
      SIGNAL(recordEvent(QObject*, const QString&, const QString&)),
      this,
      SLOT(onRecordEvent(QObject*, const QString&, const QString&)));

    // Connect record event
    QObject::connect(
      Translator,
      SIGNAL(recordEvent(int, QObject*, const QString&, const QString&)),
      this,
      SLOT(onRecordEvent(int, QObject*, const QString&, const QString&)));
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
  if (this->Implementation->Recording)
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

    // Checking mode
    if (widget != NULL && this->Implementation->Checking)
      {

      // In Gl Case, parentless widget is not transparent to mouse event
      // The event is  applied to the overlayed widget
      if (this->Implementation->CheckOverlay->GlWidget &&
           widget == this->Implementation->CheckOverlay &&
           Event->type() == QEvent::MouseButtonRelease)
        {
        widget = this->Implementation->CheckOverlayWidgetOn;
        }

      // Ignore object if specified
      if(this->Implementation->IgnoredObjects.contains(widget))
        {
        return false;
        }

      // Recover user meta propperty
      const QMetaProperty metaProp = widget->metaObject()->userProperty();

      // Mouse Move on a non-previously overlayed widget
      if (Event->type() == QEvent::MouseMove
          && this->Implementation->CheckOverlayWidgetOn != widget)
        {

        // Check for any valid translator
        bool validTranslator = false;
        for(int i = 0; i != this->Implementation->Translators.size(); ++i)
          {
          if(this->Implementation->Translators[i]->canTranslateCheckEvent(widget))
            {
            validTranslator = true;
            }
          }

        // Draw overlay
        this->Implementation->hideOverlay();
        if (widget->parentWidget() != NULL)
          {
          // Check if widget is glWidget
          this->Implementation->CheckOverlay->GlWidget = pqEventTranslator::isQVTKMetaObject(widget->metaObject());

          // Check if widget is parent to gl widget
          QList<QWidget *> children = widget->findChildren<QWidget *>();
          foreach(QWidget * child, children)
            {
            if (pqEventTranslator::isQVTKMetaObject(child->metaObject()))
              {
              // Ignore widget containing a Gl widget as a child
              this->ignoreObject(widget);
              return false;
              }
            }

          // Check widget size is valid, if not do not draw overlay
          // Widget can still be clicked and checked
          int sizeThreshold = pqCheckEventOverlay::OVERLAY_MARGIN + 2*pqCheckEventOverlay::OVERLAY_PEN_WIDTH;
          if (widget->width() < sizeThreshold || widget->height() < sizeThreshold)
            {
              return false;
            }

          // Set the validity of the overlay, via metaProp or valid translator
          this->Implementation->CheckOverlay->Valid = metaProp.isValid() || validTranslator;

          if (this->Implementation->CheckOverlay->GlWidget)
            {
            this->Implementation->CheckOverlay->setParent(NULL);
            // Cannot draw QPainter directive in openGl context, bust use another context, aka another window
            //this->Implementation->CheckOverlay->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint); // ToolTip is always on top
            this->Implementation->CheckOverlay->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint); //Tool generate avatar
            this->Implementation->CheckOverlay->setAttribute(Qt::WA_NoSystemBackground, true);
            this->Implementation->CheckOverlay->setAttribute(Qt::WA_TranslucentBackground, true);
            this->Implementation->CheckOverlay->setAttribute(Qt::WA_PaintOnScreen, true);
            this->Implementation->CheckOverlay->setAttribute(Qt::WA_TransparentForMouseEvents, true);

            // Resize and move widget
            this->Implementation->CheckOverlay->resize(widget->size());
            this->Implementation->CheckOverlay->move(widget->mapToGlobal(QPoint(0,0)));
            }
          else
            {
            // Restore window flags
            this->Implementation->CheckOverlay->setWindowFlags(Qt::Widget);
            this->Implementation->CheckOverlay->setAttribute(Qt::WA_NoSystemBackground, false);
            this->Implementation->CheckOverlay->setAttribute(Qt::WA_TranslucentBackground, false);
            this->Implementation->CheckOverlay->setAttribute(Qt::WA_PaintOnScreen, false);

            // Set parent of the overlay to be parent of the overlayed widget
            this->Implementation->CheckOverlay->setParent(qobject_cast<QWidget*>(widget->parent()));

            // Set overlay geometry to be the same o as oerlayed widget
            this->Implementation->CheckOverlay->setGeometry(widget->geometry());
            }

          // Show and Register overlay
          this->Implementation->CheckOverlay->show();
          this->Implementation->CheckOverlayWidgetOn = widget;
          }
        }
      // event on currently overlayed widget
      else if (this->Implementation->CheckOverlayWidgetOn == widget)
        {
        /*      if (Event->type() == QEvent::Leave) //TODO: Bug when leaving the widget, the overlay is still visible
                {
                this->Implementation->hideOverlay();
                }
                else */

        // Resize event
        if(Event->type() == QEvent::Resize)
          {
          // Set overlay geometry
          if (this->Implementation->CheckOverlay->GlWidget)
            {

            this->Implementation->CheckOverlay->move(widget->mapToGlobal(QPoint(0,0)));
            this->Implementation->CheckOverlay->resize(widget->size());
            }
          else
            {
            this->Implementation->CheckOverlay->setGeometry(widget->geometry());
            }
          }
        }

      // Mouse button release -> Check Event
      if (Event->type() == QEvent::MouseButtonRelease)
        {
        // Check Translators
        for(int i = 0; i != this->Implementation->Translators.size(); ++i)
          {
          bool error = false;
          if(this->Implementation->Translators[i]->translateCheckEvent(widget, error))
            {
            if(error)
              {
              qWarning() << "Error translating a check event for object " << widget;
              }
            return true;
            }
          }

        // Check meta prop validity
        if (metaProp.isValid())
          {
          // Recover meto prop name
          QString propName = metaProp.name();

          // Record check event
          onRecordEvent(pqEventTypes::CHECK_EVENT, widget, propName, widget->property(propName.toAscii().data()).toString());
          return true;
          }
        else
          {
          // Inform user trying to check uncheckable widget
          qWarning() << "Error checking an event for object, widget type not supported:" << widget->metaObject()->className();
          }
        }
      // Block all input events, so the UI is static but still drawn.
      if (dynamic_cast<QInputEvent*>(Event) != NULL)
        {
        return true;
        }
      }
    // Event Recording
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
    }
  return false;
}

// ----------------------------------------------------------------------------
void pqEventTranslator::onRecordEvent(QObject* Object,
                                      const QString& Command,
                                      const QString& Arguments)
{
  this->onRecordEvent(pqEventTypes::EVENT, Object, Command, Arguments);
}

// ----------------------------------------------------------------------------
void pqEventTranslator::onRecordEvent(int eventType,
                                      QObject* Object,
                                      const QString& Command,
                                      const QString& Arguments)
{
  if(this->Implementation->IgnoredObjects.contains(Object))
    {
    return;
    }

  QString name;
  if (eventType == pqEventTypes::EVENT)
    {
    // When sender is pqEventObject, the Object name can be NULL.
    if (!qobject_cast<pqEventComment*>(this->sender()) || Object)
      {
      name = pqObjectNaming::GetName(*Object);
      if(name.isEmpty())
          return;
      }
    }
  else
    {
    // Check the QObject does have a name
    name = pqObjectNaming::GetName(*Object);
    if(name.isEmpty())
      {
      return;
      }
    }
  // Record the event
  emit recordEvent(eventType, name, Command, Arguments);
}

// ----------------------------------------------------------------------------
bool pqEventTranslator::isQVTKMetaObject(const QMetaObject* metaObject)
{
  if (strcmp(metaObject->className(), "QVTKWidget") == 0)
    {
    return true;
    }
  if (metaObject = metaObject->superClass())
    {
    // Recursive go up the inheritance
    return isQVTKMetaObject(metaObject);
    }
  return false;
}

// ----------------------------------------------------------------------------
void pqEventTranslator::check(bool value)
{
  this->Implementation->Checking = value;

  // Hide avoerlay when not checking
  if(!value)
    {
    this->Implementation->hideOverlay();
    }
}

// ----------------------------------------------------------------------------
void pqEventTranslator::record(bool value)
{
  this->Implementation->Recording = value;

  // Hide avoerlay when not recording
  if(!value)
    {
    this->Implementation->hideOverlay();
    }
}

// ----------------------------------------------------------------------------
bool pqEventTranslator::isRecording()
{
  return this->Implementation->Recording;
}
