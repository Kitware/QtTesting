#ifndef __TemplateEventTranslator_h
#define __TemplateEventTranslator_h

// QtTesting inlcudes
#include <pqWidgetEventTranslator.h>

class TemplateEventTranslator : public pqWidgetEventTranslator
{
  Q_OBJECT

public:
  TemplateEventTranslator(QObject* parent = 0);

  using Superclass::translateEvent;
  virtual bool translateEvent(QObject* Object, QEvent* Event, bool& Error);

private Q_SLOTS:
  void onDestroyed();

private:
  TemplateEventTranslator(const TemplateEventTranslator&);            // NOT implemented
  TemplateEventTranslator& operator=(const TemplateEventTranslator&); // NOT implemented

  QObject* CurrentObject;
};

#endif
