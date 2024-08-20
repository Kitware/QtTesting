// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

// Qt includes
#include <QApplication>
#include <QtTest/QtTest>

#include <QTemplate>

// QtTesting includes
#include "pqTestUtility.h"

#include "pqTest.h"

// ----------------------------------------------------------------------------
class pqTemplateEventPlayerTester : public QObject
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
};

// ----------------------------------------------------------------------------
void pqTemplateEventPlayerTester::initTestCase()
{
  this->TestUtility = new pqTestUtility();
  this->TestUtility->addEventObserver("xml", new pqDummyEventObserver);
  this->TestUtility->addEventSource("xml", new pqDummyEventSource());

  this->Template = 0;
}

// ----------------------------------------------------------------------------
void pqTemplateEventPlayerTester::init()
{
  // Init the QTemplate
  this->Template = new QTemplate();
  this->Template->setObjectName("TemplateTest");
  this->Template->setMinimum(-99);
}

// ----------------------------------------------------------------------------
void pqTemplateEventPlayerTester::cleanup()
{
  delete this->Template;
}

// ----------------------------------------------------------------------------
void pqTemplateEventPlayerTester::cleanupTestCase()
{
  delete this->TestUtility;
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(pqTemplateEventPlayerTest)
#include "moc_pqTemplateEventPlayerTest.cpp"
