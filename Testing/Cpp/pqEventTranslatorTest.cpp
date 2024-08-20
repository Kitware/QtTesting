// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

// Qt includes
#include <QApplication>

// QtTesting includes
#include "pqEventTranslator.h"
#include "pqSpinBoxEventTranslator.h"
#include "pqTestUtility.h"
#include "pqTreeViewEventTranslator.h"

#include "pqTest.h"

// ----------------------------------------------------------------------------
class pqEventTranslatorTester : public QObject
{
  Q_OBJECT

private Q_SLOTS:

  void testDefaults();

  void testAddWidgetEventTranslator();
  void testAddWidgetEventTranslator_data();

  void testRemoveWidgetEventTranslator();
  void testRemoveWidgetEventTranslator_data();

  void testGetWidgetEventTranslator();

  void testAddDefaultWidgetEventTranslators();
  void testAddDefaultWidgetEventTranslators_data();
};

// ----------------------------------------------------------------------------
void pqEventTranslatorTester::testDefaults()
{
  pqEventTranslator eventTranslator;

  QCOMPARE(eventTranslator.translators().count(), 0);
}

// ----------------------------------------------------------------------------
void pqEventTranslatorTester::testAddWidgetEventTranslator()
{
  pqEventTranslator eventTranslator;

  QFETCH(QObject*, widget1);
  QFETCH(int, newCount1);
  QFETCH(QObject*, widget2);
  QFETCH(int, newCount2);

  // When we add the widgetEventPlayer into the eventPlayer, it is automaticaly
  // reparented to the eventPlayer. So its deletion would be automatic.
  eventTranslator.addWidgetEventTranslator(dynamic_cast<pqWidgetEventTranslator*>(widget1));
  QCOMPARE(eventTranslator.translators().count(), newCount1);

  eventTranslator.addWidgetEventTranslator(dynamic_cast<pqWidgetEventTranslator*>(widget2));
  QCOMPARE(eventTranslator.translators().count(), newCount2);
}

// ----------------------------------------------------------------------------
void pqEventTranslatorTester::testAddWidgetEventTranslator_data()
{
  QTest::addColumn<QObject*>("widget1");
  QTest::addColumn<int>("newCount1");
  QTest::addColumn<QObject*>("widget2");
  QTest::addColumn<int>("newCount2");

  pqWidgetEventTranslator* nullWidget = NULL;
  QTest::newRow("empty_empty") << qobject_cast<QObject*>(nullWidget) << 0
                               << qobject_cast<QObject*>(nullWidget) << 0;
  QTest::newRow("empty_pqSpinBox") << qobject_cast<QObject*>(nullWidget) << 0
                                   << qobject_cast<QObject*>(new pqSpinBoxEventTranslator()) << 1;
  QTest::newRow("pqSpinBox_pqSpinBox")
    << qobject_cast<QObject*>(new pqSpinBoxEventTranslator()) << 1
    << qobject_cast<QObject*>(new pqSpinBoxEventTranslator()) << 1;
  QTest::newRow("pqSpinBox_pqTreeView")
    << qobject_cast<QObject*>(new pqSpinBoxEventTranslator()) << 1
    << qobject_cast<QObject*>(new pqTreeViewEventTranslator()) << 2;
}

// ----------------------------------------------------------------------------
void pqEventTranslatorTester::testRemoveWidgetEventTranslator()
{
  pqEventTranslator eventTranslator;

  QFETCH(QString, nameToRemove);
  QFETCH(bool, firstResult);
  QFETCH(bool, secondResult);
  QFETCH(int, newCount);
  QFETCH(bool, thirdResult);

  QCOMPARE(eventTranslator.removeWidgetEventTranslator(nameToRemove), firstResult);

  // When we add the widgetEventPlayer into the eventPlayer, it is automaticaly
  // reparented to the eventPlayer. So its deletion would be automatic.
  pqTreeViewEventTranslator* treeView = new pqTreeViewEventTranslator();
  eventTranslator.addWidgetEventTranslator(treeView);

  QCOMPARE(eventTranslator.translators().count(), 1);
  QCOMPARE(eventTranslator.removeWidgetEventTranslator(nameToRemove), secondResult);
  QCOMPARE(eventTranslator.translators().count(), newCount);
  QCOMPARE(eventTranslator.removeWidgetEventTranslator(nameToRemove), thirdResult);
}

// ----------------------------------------------------------------------------
void pqEventTranslatorTester::testRemoveWidgetEventTranslator_data()
{
  QTest::addColumn<QString>("nameToRemove");
  QTest::addColumn<bool>("firstResult");
  QTest::addColumn<bool>("secondResult");
  QTest::addColumn<int>("newCount");
  QTest::addColumn<bool>("thirdResult");

  QTest::newRow("empty") << "" << false << false << 1 << false;
  QTest::newRow("wrong") << "pqSpinBoxEventTranslator" << false << false << 1 << false;
  QTest::newRow("right") << "pqTreeViewEventTranslator" << false << true << 0 << false;
}

// ----------------------------------------------------------------------------
void pqEventTranslatorTester::testGetWidgetEventTranslator()
{
  pqEventTranslator eventTranslator;

  pqSpinBoxEventTranslator* nullWidget = NULL;
  QCOMPARE(eventTranslator.getWidgetEventTranslator(0), nullWidget);
  QCOMPARE(eventTranslator.getWidgetEventTranslator("pqSpinBoxEventTranslator"), nullWidget);

  // When we add the widgetEventPlayer into the eventPlayer, it is automaticaly
  // reparented to the eventPlayer. So its deletion would be automatic.
  pqSpinBoxEventTranslator* spinBox = new pqSpinBoxEventTranslator();
  eventTranslator.addWidgetEventTranslator(spinBox);

  QCOMPARE(eventTranslator.getWidgetEventTranslator(0), nullWidget);
  QCOMPARE(eventTranslator.getWidgetEventTranslator("pqTreeViewEventTranslator"), nullWidget);
  QCOMPARE(eventTranslator.getWidgetEventTranslator("pqSpinBoxEventTranslator"), spinBox);
}

// ----------------------------------------------------------------------------
void pqEventTranslatorTester::testAddDefaultWidgetEventTranslators()
{
  pqEventTranslator eventTranslator;
  pqTestUtility testUtility;
  eventTranslator.addDefaultWidgetEventTranslators(&testUtility);
  QList<pqWidgetEventTranslator*> translators = eventTranslator.translators();

  QFETCH(int, index);
  QFETCH(QString, widgetEventTranslatorName);

  QCOMPARE(QString(translators.at(index)->metaObject()->className()), widgetEventTranslatorName);
}

// ----------------------------------------------------------------------------
void pqEventTranslatorTester::testAddDefaultWidgetEventTranslators_data()
{
  QTest::addColumn<int>("index");
  QTest::addColumn<QString>("widgetEventTranslatorName");

  QTest::newRow("0") << 0 << "pqNativeFileDialogEventTranslator";
  QTest::newRow("1") << 1 << "pq3DViewEventTranslator";
  QTest::newRow("2") << 2 << "pqListViewEventTranslator";
  QTest::newRow("3") << 3 << "pqTableViewEventTranslator";
  QTest::newRow("4") << 4 << "pqTreeViewEventTranslator";
  QTest::newRow("5") << 5 << "pqTabBarEventTranslator";
  QTest::newRow("6") << 6 << "pqSpinBoxEventTranslator";
  QTest::newRow("7") << 7 << "pqMenuEventTranslator";
  QTest::newRow("8") << 8 << "pqLineEditEventTranslator";
  QTest::newRow("9") << 9 << "pqDoubleSpinBoxEventTranslator";
  QTest::newRow("10") << 10 << "pqComboBoxEventTranslator";
  QTest::newRow("11") << 11 << "pqAbstractSliderEventTranslator";
  QTest::newRow("12") << 12 << "pqAbstractItemViewEventTranslator";
  QTest::newRow("13") << 13 << "pqAbstractButtonEventTranslator";
  QTest::newRow("14") << 14 << "pqBasicWidgetEventTranslator";
}
// ----------------------------------------------------------------------------
CTK_TEST_MAIN(pqEventTranslatorTest)
#include "moc_pqEventTranslatorTest.cpp"
