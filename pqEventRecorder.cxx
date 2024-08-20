// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

// Qt includes
#include <QIODevice>
#include <QTextStream>

// QtTesting includes
#include "pqEventObserver.h"
#include "pqEventRecorder.h"
#include "pqEventTranslator.h"
#include "pqEventTypes.h"

// ----------------------------------------------------------------------------
pqEventRecorder::pqEventRecorder(QObject* parent)
  : Superclass(parent)
{
  this->ActiveObserver = 0;
  this->ActiveTranslator = 0;
  this->File = 0;
  this->ContinuousFlush = false;
  this->RecordInteractionTimings = false;
}

// ----------------------------------------------------------------------------
pqEventRecorder::~pqEventRecorder()
{
  this->ActiveObserver = 0;
  this->ActiveTranslator = 0;
}

// ----------------------------------------------------------------------------
void pqEventRecorder::setContinuousFlush(bool value)
{
  if (!this->ActiveObserver)
  {
    return;
  }

  if (value)
  {
    QObject::connect(this->ActiveObserver, SIGNAL(eventRecorded(QString, QString, QString, int)),
      this, SLOT(flush()));
  }
  else
  {
    QObject::disconnect(this->ActiveObserver, SIGNAL(eventRecorded(QString, QString, QString, int)),
      this, SLOT(flush()));
  }
  this->ContinuousFlush = value;
}

// ----------------------------------------------------------------------------
bool pqEventRecorder::continuousFlush() const
{
  return this->ContinuousFlush;
}

// ----------------------------------------------------------------------------
void pqEventRecorder::setRecordInteractionTimings(bool value)
{
  this->RecordInteractionTimings = value;
  if (this->ActiveTranslator)
  {
    this->ActiveTranslator->recordInteractionTimings(value);
  }
}

// ----------------------------------------------------------------------------
bool pqEventRecorder::recordInteractionTimings() const
{
  return this->RecordInteractionTimings;
}

// ----------------------------------------------------------------------------
void pqEventRecorder::check(bool value)
{
  this->ActiveTranslator->check(value);
}

// ----------------------------------------------------------------------------
void pqEventRecorder::setFile(QIODevice* file)
{
  this->File = file;
}

// ----------------------------------------------------------------------------
QIODevice* pqEventRecorder::file() const
{
  return this->File;
}

// ----------------------------------------------------------------------------
void pqEventRecorder::setObserver(pqEventObserver* observer)
{
  this->ActiveObserver = observer;
}

// ----------------------------------------------------------------------------
pqEventObserver* pqEventRecorder::observer() const
{
  return this->ActiveObserver;
}

// ----------------------------------------------------------------------------
void pqEventRecorder::setTranslator(pqEventTranslator* translator)
{
  this->ActiveTranslator = translator;
  if (this->ActiveTranslator != NULL)
  {
    this->ActiveTranslator->recordInteractionTimings(this->RecordInteractionTimings);
  }
}

// ----------------------------------------------------------------------------
pqEventTranslator* pqEventRecorder::translator() const
{
  return this->ActiveTranslator;
}

// ----------------------------------------------------------------------------
bool pqEventRecorder::isRecording() const
{
  if (this->ActiveTranslator != NULL)
  {
    return this->ActiveTranslator->isRecording();
  }
  else
  {
    return false;
  }
}

// ----------------------------------------------------------------------------
void pqEventRecorder::recordEvents(
  pqEventTranslator* translator, pqEventObserver* observer, QIODevice* file, bool continuousFlush)
{
  this->setTranslator(translator);
  this->setObserver(observer);
  this->setFile(file);
  this->setContinuousFlush(continuousFlush);

  this->start();
}

// ----------------------------------------------------------------------------
void pqEventRecorder::flush()
{
  this->Stream.flush();
}

// ----------------------------------------------------------------------------
void pqEventRecorder::start()
{
  if (!this->File || !this->ActiveObserver || !this->ActiveTranslator)
  {
    return;
  }

  QObject::connect(this->ActiveTranslator, SIGNAL(recordEvent(QString, QString, QString, int)),
    this->ActiveObserver, SLOT(onRecordEvent(QString, QString, QString, int)));

  // Set the device
  this->Stream.setDevice(this->File);

#if QT_VERSION < 0x060000
  // Set UTF8 Codec
  this->Stream.setCodec("UTF-8");
#endif

  // Set the Stream to the Observer
  this->ActiveObserver->setStream(&this->Stream);

  // Start the Translator
  this->ActiveTranslator->start();

  this->ActiveTranslator->record(true);
  Q_EMIT this->started();
}

// ----------------------------------------------------------------------------
void pqEventRecorder::stop(int value)
{
  QObject::disconnect(this->ActiveTranslator, SIGNAL(recordEvent(QString, QString, QString, int)),
    this->ActiveObserver, SLOT(onRecordEvent(QString, QString, QString, int)));

  this->ActiveObserver->setStream(NULL);
  this->ActiveTranslator->stop();

  this->ActiveTranslator->record(false);

  if (!value)
  {
    return;
  }

  this->flush();
  Q_EMIT this->stopped();
}

// ----------------------------------------------------------------------------
void pqEventRecorder::unpause(bool value)
{
  this->pause(!value);
}

// ----------------------------------------------------------------------------
void pqEventRecorder::pause(bool value)
{
  this->ActiveTranslator->record(!value);
  Q_EMIT this->paused(value);
}
