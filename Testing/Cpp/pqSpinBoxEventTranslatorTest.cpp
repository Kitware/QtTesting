// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

// Qt includes
#include <QApplication>
#include <QSpinBox>
#include <QStyle>
#include <QtTest/QtTest>

// QtTesting includes
#include "pqTestUtility.h"

#include "pqTest.h"

// ----------------------------------------------------------------------------
class pqSpinBoxEventTranslatorTester : public QObject
{
  Q_OBJECT

private Q_SLOTS:

  void initTestCase();
  void init();
  void cleanup();
  void cleanupTestCase();

  void testRecordLeftMouseClick();
  void testRecordLeftMouseClick_data();

  void testRecordRightMouseClick();
  void testRecordRightMouseClick_data();

  void testRecordMiddleMouseClick();
  void testRecordMiddleMouseClick_data();

  void testRecordKeyBoardClick();
  void testRecordKeyBoardClick_data();

  void testRecordComplexClick();

  void testRecordCheckMouseClick();
  void testRecordCheckMouseClick_data();

private:
  QSpinBox* SpinBox;

  pqTestUtility* TestUtility;
  pqDummyEventObserver* EventObserver;
};

// ----------------------------------------------------------------------------
void pqSpinBoxEventTranslatorTester::initTestCase()
{
  this->TestUtility = new pqTestUtility();
  this->EventObserver = new pqDummyEventObserver();
  this->TestUtility->addEventObserver("xml", this->EventObserver);
  this->TestUtility->addEventSource("xml", new pqDummyEventSource());

  this->SpinBox = 0;
}

// ----------------------------------------------------------------------------
void pqSpinBoxEventTranslatorTester::init()
{
  // Init SpinBox
  this->SpinBox = new QSpinBox();
  this->SpinBox->setObjectName("spinBoxTest");
  this->SpinBox->setMinimum(-100);

  // Start to record events
  this->TestUtility->recordTestsBySuffix("xml");

  // Fire the event "enter" to connect spinbox signals to the translator slots
  QEvent enter(QEvent::Enter);
  qApp->notify(this->SpinBox, &enter);
}

// ----------------------------------------------------------------------------
void pqSpinBoxEventTranslatorTester::cleanup()
{
  this->TestUtility->stopRecords(0);
  delete this->SpinBox;
}

// ----------------------------------------------------------------------------
void pqSpinBoxEventTranslatorTester::cleanupTestCase()
{
  this->EventObserver = 0;
  delete this->TestUtility;
}

// ----------------------------------------------------------------------------
void pqSpinBoxEventTranslatorTester::testRecordLeftMouseClick()
{
  QFETCH(int, X);
  QFETCH(int, Y);
  QFETCH(QString, recordEmitted);

  QTest::mouseClick(this->SpinBox, Qt::LeftButton, Qt::NoModifier, QPoint(X, Y));
  QCOMPARE(this->EventObserver->Text, recordEmitted);

  this->EventObserver->Text = QString();
}

// ----------------------------------------------------------------------------
void pqSpinBoxEventTranslatorTester::testRecordLeftMouseClick_data()
{
  QSpinBox spinBox;

  QTest::addColumn<int>("X");
  QTest::addColumn<int>("Y");
  QTest::addColumn<QString>("recordEmitted");

  QSize size = spinBox.size();
  int frameWidth = spinBox.style()->pixelMetric(QStyle::PM_SpinBoxFrameWidth) + 1;

  QTest::newRow("0,0") << 0 << 0 << QString();
  QTest::newRow("invalid") << -size.rwidth() << frameWidth << QString();
  QTest::newRow("middle") << size.rwidth() / 2 << size.rheight() / 2 << QString();
  QTest::newRow("arrow up") << size.rwidth() - frameWidth << frameWidth
                            << QString("spinBoxTest, set_int, 1#");
  QTest::newRow("arrow down") << size.rwidth() - frameWidth << size.rheight() - frameWidth
                              << QString("spinBoxTest, set_int, -1#");
}

// ----------------------------------------------------------------------------
void pqSpinBoxEventTranslatorTester::testRecordRightMouseClick()
{
  QFETCH(int, X);
  QFETCH(int, Y);
  QFETCH(QString, recordEmitted);

  QTest::mouseClick(this->SpinBox, Qt::RightButton, Qt::NoModifier, QPoint(X, Y));
  QCOMPARE(this->EventObserver->Text, recordEmitted);

  this->EventObserver->Text = QString();
}

// ----------------------------------------------------------------------------
void pqSpinBoxEventTranslatorTester::testRecordRightMouseClick_data()
{
  QSpinBox spinBox;

  QTest::addColumn<int>("X");
  QTest::addColumn<int>("Y");
  QTest::addColumn<QString>("recordEmitted");

  QSize size = spinBox.size();
  int frameWidth = spinBox.style()->pixelMetric(QStyle::PM_SpinBoxFrameWidth) + 1;

  QTest::newRow("0,0") << 0 << 0 << QString();
  QTest::newRow("invalid") << -size.rwidth() << frameWidth << QString();
  QTest::newRow("middle") << size.rwidth() / 2 << size.rheight() / 2 << QString();
  QTest::newRow("arrow down") << size.rwidth() - frameWidth << size.rheight() - frameWidth
                              << QString();
  QTest::newRow("arrow up") << size.rwidth() - frameWidth << frameWidth << QString();
}

// ----------------------------------------------------------------------------
void pqSpinBoxEventTranslatorTester::testRecordMiddleMouseClick()
{
  QFETCH(int, X);
  QFETCH(int, Y);
  QFETCH(QString, recordEmitted);

  QTest::mouseClick(this->SpinBox, Qt::MiddleButton, Qt::NoModifier, QPoint(X, Y));
  QCOMPARE(this->EventObserver->Text, recordEmitted);

  this->EventObserver->Text = QString();
}

// ----------------------------------------------------------------------------
void pqSpinBoxEventTranslatorTester::testRecordMiddleMouseClick_data()
{
  this->testRecordRightMouseClick_data();
}

// ----------------------------------------------------------------------------
void pqSpinBoxEventTranslatorTester::testRecordKeyBoardClick()
{
  QFETCH(QString, number);
  QFETCH(QString, recordEmitted);
  this->SpinBox->clear();

  QTest::keyClicks(this->SpinBox, number);
  QCOMPARE(this->EventObserver->Text, recordEmitted);

  this->EventObserver->Text = QString();
}

// ----------------------------------------------------------------------------
void pqSpinBoxEventTranslatorTester::testRecordKeyBoardClick_data()
{
  QTest::addColumn<QString>("number");
  QTest::addColumn<QString>("recordEmitted");

  QTest::newRow("33") << QString::number(33)
                      << QString("%1#%2#%3#%4#")
                           .arg(QString("spinBoxTest, set_int, 3"),
                             QString("spinBoxTest, set_int, 3"),
                             QString("spinBoxTest, set_int, 33"),
                             QString("spinBoxTest, set_int, 33"));
  QTest::newRow("-5") << QString::number(-5)
                      << QString("%1#%2#%3#")
                           .arg(QString("spinBoxTest, set_int, 0"), // corresponds to "-"
                             QString("spinBoxTest, set_int, -5"),
                             QString("spinBoxTest, set_int, -5"));
}

// ----------------------------------------------------------------------------
void pqSpinBoxEventTranslatorTester::testRecordComplexClick()
{
  QSize size = this->SpinBox->size();
  int frameWidth = this->SpinBox->style()->pixelMetric(QStyle::PM_SpinBoxFrameWidth) + 1;

  QString recordExpected;

  QTest::keyClicks(this->SpinBox, "10");
  recordExpected.append(QString("spinBoxTest, set_int, 1#"));
  recordExpected.append(QString("spinBoxTest, set_int, 1#"));
  recordExpected.append(QString("spinBoxTest, set_int, 10#"));
  recordExpected.append(QString("spinBoxTest, set_int, 10#"));

  QTest::mouseClick(this->SpinBox, Qt::LeftButton, Qt::NoModifier,
    QPoint(size.rwidth() - frameWidth, size.rheight() - frameWidth));
  recordExpected.append(QString("spinBoxTest, set_int, 9#"));

  QTest::mouseClick(this->SpinBox, Qt::LeftButton, Qt::NoModifier,
    QPoint(size.rwidth() - frameWidth, size.rheight() - frameWidth));
  recordExpected.append(QString("spinBoxTest, set_int, 8#"));

  QTest::mouseClick(
    this->SpinBox, Qt::LeftButton, Qt::NoModifier, QPoint(size.rwidth() - frameWidth, frameWidth));
  recordExpected.append(QString("spinBoxTest, set_int, 9#"));

  QTest::mouseClick(
    this->SpinBox, Qt::LeftButton, Qt::NoModifier, QPoint(size.rwidth() - frameWidth, frameWidth));
  recordExpected.append(QString("spinBoxTest, set_int, 10#"));

  QTest::mouseClick(
    this->SpinBox, Qt::LeftButton, Qt::NoModifier, QPoint(size.rwidth() - frameWidth, frameWidth));
  recordExpected.append(QString("spinBoxTest, set_int, 11#"));

  QTest::mouseClick(
    this->SpinBox, Qt::LeftButton, Qt::NoModifier, QPoint(size.rwidth() - frameWidth, frameWidth));
  recordExpected.append(QString("spinBoxTest, set_int, 12#"));

  QTest::mouseClick(
    this->SpinBox, Qt::LeftButton, Qt::NoModifier, QPoint(size.rwidth() - frameWidth, frameWidth));
  recordExpected.append(QString("spinBoxTest, set_int, 13#"));

  QTest::keyClicks(this->SpinBox, "0");
  recordExpected.append(QString("spinBoxTest, set_int, 0#"));
  recordExpected.append(QString("spinBoxTest, set_int, 0#"));

  QCOMPARE(this->EventObserver->Text, recordExpected);
  this->EventObserver->Text = QString();
}

// ----------------------------------------------------------------------------
void pqSpinBoxEventTranslatorTester::testRecordCheckMouseClick()
{
  this->EventObserver->Text = QString();
  this->SpinBox->clear();
  this->SpinBox->setValue(17);
  this->TestUtility->recorder()->check(true);

  QFETCH(QString, recordEmitted);
  QTest::mouseClick(this->SpinBox, Qt::LeftButton);
  QCOMPARE(this->EventObserver->Text, recordEmitted);
  this->EventObserver->Text = QString();
}

// ----------------------------------------------------------------------------
void pqSpinBoxEventTranslatorTester::testRecordCheckMouseClick_data()
{
  QTest::addColumn<QString>("recordEmitted");

  QTest::newRow("Click") << QString("%1#%2#").arg(
    QString("spinBoxTest, set_int, 17"), QString("Check, spinBoxTest, value, 17"));
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(pqSpinBoxEventTranslatorTest)
#include "moc_pqSpinBoxEventTranslatorTest.cpp"
