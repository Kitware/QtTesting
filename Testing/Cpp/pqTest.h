// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause AND Apache-2.0

// Qt includes
#include <QtTest/QtTest>

// QtTesting includes
#include "pqEventObserver.h"
#include "pqEventSource.h"

// ----------------------------------------------------------------------------
// Dumy pqEventObserver

class pqDummyEventObserver : public pqEventObserver
{
  Q_OBJECT

public:
  pqDummyEventObserver(QObject* p = 0)
    : pqEventObserver(p)
  {
  }
  ~pqDummyEventObserver() {}

  virtual void setStream(QTextStream* /*stream*/) {}

  QString Text;

public Q_SLOTS:
  virtual void onRecordEvent(
    const QString& widget, const QString& command, const QString& arguments, const int& eventType)
  {
    if (eventType == pqEventTypes::CHECK_EVENT)
    {
      this->Text.append(QString("Check, %1, %2, %3#").arg(widget, command, arguments));
    }
    else
    {
      this->Text.append(QString("%1, %2, %3#").arg(widget, command, arguments));
    }
  }
};

// ----------------------------------------------------------------------------
// Dumy eventSource

class pqDummyEventSource : public pqEventSource
{
  typedef pqEventSource Superclass;

public:
  pqDummyEventSource(QObject* p = 0)
    : Superclass(p)
  {
  }
  ~pqDummyEventSource() {}

protected:
  virtual void setContent(const QString& /*xmlfilename*/) { return; }

  int getNextEvent(
    QString& /*widget*/, QString& /*command*/, QString& /*arguments*/, int& /*eventType*/)
  {
    return 0;
  }
};

// ----------------------------------------------------------------------------
// MACRO

#define CTK_TEST_NOOP_MAIN(TestObject)                                                             \
  int TestObject(int argc, char* argv[])                                                           \
  {                                                                                                \
    QObject tc;                                                                                    \
    return QTest::qExec(&tc, argc, argv);                                                          \
  }

#ifdef QT_GUI_LIB

//-----------------------------------------------------------------------------
#define CTK_TEST_MAIN(TestObject)                                                                  \
  int TestObject(int argc, char* argv[])                                                           \
  {                                                                                                \
    QApplication app(argc, argv);                                                                  \
    TestObject##er tc;                                                                             \
    return QTest::qExec(&tc, argc, argv);                                                          \
  }

#else

//-----------------------------------------------------------------------------
#define CTK_TEST_MAIN(TestObject)                                                                  \
  int TestObject(int argc, char* argv[])                                                           \
  {                                                                                                \
    QCoreApplication app(argc, argv);                                                              \
    QTEST_DISABLE_KEYPAD_NAVIGATION                                                                \
    TestObject##er tc;                                                                             \
    return QTest::qExec(&tc, argc, argv);                                                          \
  }

#endif // QT_GUI_LIB

namespace ctkTest
{
static void mouseEvent(QTest::MouseAction action, QWidget* widget, Qt::MouseButton button,
  Qt::KeyboardModifiers stateKey, QPoint pos, int delay = -1)
{
  if (action != QTest::MouseMove)
  {
    QTest::mouseEvent(action, widget, button, stateKey, pos, delay);
    return;
  }
  QTEST_ASSERT(widget);
  // extern int Q_TESTLIB_EXPORT defaultMouseDelay();
  // if (delay == -1 || delay < defaultMouseDelay())
  //    delay = defaultMouseDelay();
  if (delay > 0)
    QTest::qWait(delay);

  if (pos.isNull())
    pos = widget->rect().center();

  QTEST_ASSERT(button == Qt::NoButton || button & Qt::MouseButtonMask);
  QTEST_ASSERT(stateKey == 0 || stateKey & Qt::KeyboardModifierMask);

  stateKey &= static_cast<unsigned int>(Qt::KeyboardModifierMask);

  QMouseEvent me(QEvent::MouseMove, pos, widget->mapToGlobal(pos), button, button, stateKey);
  QSpontaneKeyEvent::setSpontaneous(&me);
  if (!qApp->notify(widget, &me))
  {
    static const char* mouseActionNames[] = { "MousePress", "MouseRelease", "MouseClick",
      "MouseDClick", "MouseMove" };
    QString warning = QString::fromLatin1("Mouse event \"%1\" not accepted by receiving widget");
    QTest::qWarn(warning.arg(QString::fromLatin1(mouseActionNames[static_cast<int>(action)]))
                   .toLatin1()
                   .data());
  }
}

inline void mouseMove(QWidget* widget, Qt::MouseButton button,
  Qt::KeyboardModifiers stateKey = Qt::NoModifier, QPoint pos = QPoint(), int delay = -1)
{
  ctkTest::mouseEvent(QTest::MouseMove, widget, button, stateKey, pos, delay);
}
}
