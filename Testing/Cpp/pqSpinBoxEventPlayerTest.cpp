// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

// Qt includes
#include <QApplication>
#include <QSpinBox>
#include <QtTest/QtTest>

// QtTesting includes
#include "pqEventTypes.h"
#include "pqTestUtility.h"

#include "pqTest.h"

// ----------------------------------------------------------------------------
class pqSpinBoxEventPlayerTester : public QObject
{
  Q_OBJECT

private Q_SLOTS:

  void initTestCase();
  void init();
  void cleanup();
  void cleanupTestCase();

  void testPlayBackCommandSetInt();
  void testPlayBackCommandSetInt_data();

private:
  QSpinBox* SpinBox;

  pqTestUtility* TestUtility;
};

// ----------------------------------------------------------------------------
void pqSpinBoxEventPlayerTester::initTestCase()
{
  this->TestUtility = new pqTestUtility();
  this->TestUtility->addEventObserver("xml", new pqDummyEventObserver);
  this->TestUtility->addEventSource("xml", new pqDummyEventSource());

  this->SpinBox = 0;
}

// ----------------------------------------------------------------------------
void pqSpinBoxEventPlayerTester::init()
{
  // Init the QSpinBox
  this->SpinBox = new QSpinBox();
  this->SpinBox->setObjectName("spinBoxTest");
  this->SpinBox->setMinimum(-99);
}

// ----------------------------------------------------------------------------
void pqSpinBoxEventPlayerTester::cleanup()
{
  delete this->SpinBox;
}

// ----------------------------------------------------------------------------
void pqSpinBoxEventPlayerTester::cleanupTestCase()
{
  delete this->TestUtility;
}

// ----------------------------------------------------------------------------
void pqSpinBoxEventPlayerTester::testPlayBackCommandSetInt()
{
  QFETCH(int, value);
  QFETCH(int, result);
  QFETCH(int, count);

  QSignalSpy spy(this->SpinBox, SIGNAL(valueChanged(int)));

  bool error;
  this->TestUtility->eventPlayer()->playEvent(
    QString("spinBoxTest"), QString("set_int"), QString::number(value), error);

  QCOMPARE(this->SpinBox->value(), result);
  QCOMPARE(error, false);

  this->TestUtility->eventPlayer()->playEvent(QString("spinBoxTest"), QString("value"),
    QString::number(result), pqEventTypes::CHECK_EVENT, error);
  QCOMPARE(error, false);
  QCOMPARE(spy.count(), count);
}

// ----------------------------------------------------------------------------
void pqSpinBoxEventPlayerTester::testPlayBackCommandSetInt_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");
  QTest::addColumn<int>("count");

  QTest::newRow("positive") << 10 << 10 << 1;
  QTest::newRow("negative") << -33 << -33 << 1;
  QTest::newRow("negativeoutside") << -200 << -99 << 1;
  QTest::newRow("positiveoutside") << 200 << 99 << 1;
  QTest::newRow("negativeboundary") << -99 << -99 << 1;
  QTest::newRow("positiveboundary") << 99 << 99 << 1;
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(pqSpinBoxEventPlayerTest)
#include "moc_pqSpinBoxEventPlayerTest.cpp"
