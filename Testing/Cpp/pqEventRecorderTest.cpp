// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

// Qt includes
#include <QApplication>

// QtTesting includes
#include "pqEventObserver.h"
#include "pqEventRecorder.h"
#include "pqTestUtility.h"

#include "pqTest.h"

// ----------------------------------------------------------------------------
class pqEventRecorderTester : public QObject
{
  Q_OBJECT

private Q_SLOTS:

  void testDefaults();

  void testSetFile();

  void testSetObserver();
  void testSetTranslator();

  void testSetContiniousFlush();
  void testSetContiniousFlush_data();

  void testRecordEvents();
  void testRecordEvents_data();
};

// ----------------------------------------------------------------------------
void pqEventRecorderTester::testDefaults()
{
  pqEventRecorder eventRecorder;

  QIODevice* nullDevice = 0;
  pqEventObserver* nullObserver = 0;
  pqEventTranslator* nullTranslator = 0;

  QCOMPARE(eventRecorder.isRecording(), false);
  QCOMPARE(eventRecorder.file(), nullDevice);
  QCOMPARE(eventRecorder.observer(), nullObserver);
  QCOMPARE(eventRecorder.translator(), nullTranslator);
}

// ----------------------------------------------------------------------------
void pqEventRecorderTester::testSetFile()
{
  pqEventRecorder recorder;

  QFile file;
  recorder.setFile(&file);
  QCOMPARE(recorder.file(), &file);
}

// ----------------------------------------------------------------------------
void pqEventRecorderTester::testSetObserver()
{
  pqEventRecorder recorder;

  pqDummyEventObserver observer;
  recorder.setObserver(&observer);
  QCOMPARE(recorder.observer(), &observer);
}

// ----------------------------------------------------------------------------
void pqEventRecorderTester::testSetTranslator()
{
  pqEventRecorder recorder;

  pqEventTranslator translator;
  recorder.setTranslator(&translator);
  QCOMPARE(recorder.translator(), &translator);
}

// ----------------------------------------------------------------------------
void pqEventRecorderTester::testSetContiniousFlush()
{
  pqEventRecorder recorder;

  QFETCH(bool, activeObserver);
  QFETCH(bool, first);
  QFETCH(bool, firstResult);
  QFETCH(bool, second);
  QFETCH(bool, secondResult);

  recorder.setObserver(activeObserver ? new pqDummyEventObserver() : 0);
  recorder.setContinuousFlush(first);
  QCOMPARE(recorder.continuousFlush(), firstResult);
  recorder.setContinuousFlush(second);
  QCOMPARE(recorder.continuousFlush(), secondResult);
}

// ----------------------------------------------------------------------------
void pqEventRecorderTester::testSetContiniousFlush_data()
{
  QTest::addColumn<bool>("activeObserver");
  QTest::addColumn<bool>("first");
  QTest::addColumn<bool>("firstResult");
  QTest::addColumn<bool>("second");
  QTest::addColumn<bool>("secondResult");

  QTest::newRow("1") << false << false << false << false << false;
  QTest::newRow("2") << false << false << false << true << false;
  QTest::newRow("3") << false << true << false << false << false;
  QTest::newRow("4") << false << true << false << true << false;

  QTest::newRow("5") << true << false << false << false << false;
  QTest::newRow("6") << true << false << false << true << true;
  QTest::newRow("7") << true << true << true << false << false;
  QTest::newRow("8") << true << true << true << true << true;
}

// ----------------------------------------------------------------------------
void pqEventRecorderTester::testRecordEvents()
{
  pqEventRecorder recorder;

  QFETCH(QObject*, ioDevice);
  QFETCH(QObject*, activeObserver);
  QFETCH(QObject*, activeTranslator);
  QFETCH(bool, continuousFlush);
  QFETCH(bool, started);
  QFETCH(bool, flushing);

  recorder.recordEvents(qobject_cast<pqEventTranslator*>(activeTranslator),
    qobject_cast<pqEventObserver*>(activeObserver), qobject_cast<QFile*>(ioDevice),
    continuousFlush);

  QCOMPARE(recorder.isRecording(), started);
  QCOMPARE(recorder.continuousFlush(), flushing);
}

// ----------------------------------------------------------------------------
void pqEventRecorderTester::testRecordEvents_data()
{
  QTest::addColumn<QObject*>("ioDevice");
  QTest::addColumn<QObject*>("activeObserver");
  QTest::addColumn<QObject*>("activeTranslator");
  QTest::addColumn<bool>("continuousFlush");
  QTest::addColumn<bool>("started");
  QTest::addColumn<bool>("flushing");

  QFile* nullFile = 0;
  QFile* file = new QFile;

  pqEventObserver* nullObserver = 0;
  pqDummyEventObserver* observer = new pqDummyEventObserver;

  pqEventTranslator* nullTranslator = 0;
  pqEventTranslator* translator = new pqEventTranslator;

  QTest::newRow("1") << qobject_cast<QObject*>(nullFile) << qobject_cast<QObject*>(nullObserver)
                     << qobject_cast<QObject*>(nullTranslator) << false << false << false;
  QTest::newRow("2") << qobject_cast<QObject*>(nullFile) << qobject_cast<QObject*>(nullObserver)
                     << qobject_cast<QObject*>(translator) << false << false << false;
  QTest::newRow("3") << qobject_cast<QObject*>(nullFile) << qobject_cast<QObject*>(observer)
                     << qobject_cast<QObject*>(translator) << false << false << false;
  QTest::newRow("4") << qobject_cast<QObject*>(file) << qobject_cast<QObject*>(observer)
                     << qobject_cast<QObject*>(translator) << false << true << false;
  QTest::newRow("4") << qobject_cast<QObject*>(file) << qobject_cast<QObject*>(observer)
                     << qobject_cast<QObject*>(translator) << true << true << true;
}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(pqEventRecorderTest)
#include "moc_pqEventRecorderTest.cpp"
