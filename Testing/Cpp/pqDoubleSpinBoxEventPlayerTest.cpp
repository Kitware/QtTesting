// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

// Qt includes
#include <QApplication>
#include <QDoubleSpinBox>
#include <QStyle>
#include <QtTest/QtTest>

// QtTesting includes
#include "pqEventTypes.h"
#include "pqTestUtility.h"

#include "pqTest.h"

// ----------------------------------------------------------------------------
class pqDoubleSpinBoxEventPlayerTester : public QObject
{
  Q_OBJECT

private Q_SLOTS:

  void initTestCase();
  void init();
  void cleanup();
  void cleanupTestCase();

  void testPlayBackCommandSetDouble();
  void testPlayBackCommandSetDouble_data();

private:
  QDoubleSpinBox* DoubleSpinBox;

  pqTestUtility* TestUtility;
};

// ----------------------------------------------------------------------------
void pqDoubleSpinBoxEventPlayerTester::initTestCase()
{
  this->TestUtility = new pqTestUtility();
  this->TestUtility->addEventObserver("xml", new pqDummyEventObserver);
  this->TestUtility->addEventSource("xml", new pqDummyEventSource());

  this->DoubleSpinBox = 0;
}

// ----------------------------------------------------------------------------
void pqDoubleSpinBoxEventPlayerTester::init()
{
  // Init the QSpinBox
  this->DoubleSpinBox = new QDoubleSpinBox();
  this->DoubleSpinBox->setObjectName("doubleSpinBoxTest");
  this->DoubleSpinBox->setMinimum(-99.99);
  this->DoubleSpinBox->setValue(0);
}

// ----------------------------------------------------------------------------
void pqDoubleSpinBoxEventPlayerTester::cleanup()
{
  delete this->DoubleSpinBox;
}

// ----------------------------------------------------------------------------
void pqDoubleSpinBoxEventPlayerTester::cleanupTestCase()
{
  delete this->TestUtility;
}

// ----------------------------------------------------------------------------
void pqDoubleSpinBoxEventPlayerTester::testPlayBackCommandSetDouble()
{
  QFETCH(double, value);
  QFETCH(double, result);
  QFETCH(int, count);

  QSignalSpy spy(this->DoubleSpinBox, SIGNAL(valueChanged(double)));

  bool error;
  this->TestUtility->eventPlayer()->playEvent(
    QString("doubleSpinBoxTest"), QString("set_double"), QString::number(value), error);

  QCOMPARE(error, false);
  QCOMPARE(this->DoubleSpinBox->value(), result);

  this->TestUtility->eventPlayer()->playEvent(QString("doubleSpinBoxTest"), QString("value"),
    QString::number(result), pqEventTypes::CHECK_EVENT, error);

  QCOMPARE(error, false);
  QCOMPARE(spy.count(), count);
}

// ----------------------------------------------------------------------------
void pqDoubleSpinBoxEventPlayerTester::testPlayBackCommandSetDouble_data()
{
  QTest::addColumn<double>("value");
  QTest::addColumn<double>("result");
  QTest::addColumn<int>("count");

  QTest::newRow("negative") << -10.5 << -10.5 << 1;
  QTest::newRow("positive") << 33.25 << 33.25 << 1;
  QTest::newRow("negativeoutside") << -200.00 << -99.99 << 1;
  QTest::newRow("positiveoutside") << 200.00 << 99.99 << 1;
  QTest::newRow("negativeboundary") << -99.99 << -99.99 << 1;
  QTest::newRow("positiveboundary") << 99.99 << 99.99 << 1;
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(pqDoubleSpinBoxEventPlayerTest)
#include "moc_pqDoubleSpinBoxEventPlayerTest.cpp"
