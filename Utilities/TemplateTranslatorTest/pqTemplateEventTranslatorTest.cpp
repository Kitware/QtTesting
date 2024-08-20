// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

// Qt includes
#include <QApplication>
#include <QStyle>
#include <QtTest/QtTest>

#include <QTemplate>

// QtTesting includes
#include "pqTestUtility.h"

#include "pqTest.h"

// ----------------------------------------------------------------------------
class pqTemplateEventTranslatorTester : public QObject
{
  Q_OBJECT

private Q_SLOTS:

  void initTestCase();
  void init();
  void cleanup();
  void cleanupTestCase();

private:
  QTemplate* Template;

  pqTestUtility* TestUtility;
  pqDummyEventObserver* EventObserver;
};

// ----------------------------------------------------------------------------
void pqTemplateEventTranslatorTester::initTestCase()
{
  this->TestUtility = new pqTestUtility();
  this->EventObserver = new pqDummyEventObserver();
  this->TestUtility->addEventObserver("xml", this->EventObserver);
  this->TestUtility->addEventSource("xml", new pqDummyEventSource());

  this->Template = 0;
}

// ----------------------------------------------------------------------------
void pqTemplateEventTranslatorTester::init()
{
  // Init the Template
  this->Template = new QTemplate();
  this->Template->setObjectName("TemplateTest");

  // Start to record events
  this->TestUtility->recordTestsBySuffix("xml");

  // Fire the event "enter" to connect Template signals to the translator slots
  QEvent enter(QEvent::Enter);
  qApp->notify(this->Template, &enter);
}

// ----------------------------------------------------------------------------
void pqTemplateEventTranslatorTester::cleanup()
{
  this->TestUtility->stopRecords(0);
  delete this->Template;
}

// ----------------------------------------------------------------------------
void pqTemplateEventTranslatorTester::cleanupTestCase()
{
  this->EventObserver = 0;

  delete this->TestUtility;
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(pqTemplateEventTranslatorTest)
#include "moc_pqTemplateEventTranslatorTest.cpp"
