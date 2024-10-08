// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqTestUtility_h
#define _pqTestUtility_h

// Qt includes
#include <QDir>
#include <QFile>
#include <QMap>
#include <QObject>
#include <QSet>
#include <QStringList>
#include <QTextStream>

// QtTesting includes
#include "pqEventDispatcher.h"
#include "pqEventPlayer.h"
#include "pqEventRecorder.h"
#include "pqEventTranslator.h"

#include "QtTestingExport.h"

class pqEventObserver;
class pqEventSource;
class pqPlayBackEventsDialog;

/// Organizes basic functionality for regression testing
class QTTESTING_EXPORT pqTestUtility : public QObject
{
  Q_OBJECT
  /// This property controls whether the RecordEventsDialog is opened when
  /// recording events.
  /// True by default.
  Q_PROPERTY(bool recordWithDialog READ recordWithDialog WRITE setRecordWithDialog)
public:
  pqTestUtility(QObject* parent = 0);
  ~pqTestUtility() override;

public:
  /// Get the event dispatcher. Dispatcher is used to play tests back.
  pqEventDispatcher* dispatcher();

  /// Get the event recorder. Recorder is used to record tests.
  pqEventRecorder* recorder();

  /// Get the event player. This the test-file-interpreter (if you will), that
  /// parses the test file and creates events from it that can be dispatched by
  /// the pqEventDispatcher.
  pqEventPlayer* eventPlayer();

  /// Get the event translator. This is used for recording tests.
  pqEventTranslator* eventTranslator();

  /// add an event source for playback of files
  /// An pqXMLEventSource is automatically added if XML support is enabled.
  /// A pqPythonEventSource is automatically added if Python support is enabled.
  void addEventSource(const QString& fileExtension, pqEventSource* source);

  /// Return a QMap of all the eventSources.
  QMap<QString, pqEventSource*> eventSources() const;

  /// add an event observer for recording files
  /// An pqXMLEventObserver is automatically added if XML support is enabled.
  /// A pqPythonEventObserver is automatically added if Python support is enabled.
  void addEventObserver(const QString& fileExtension, pqEventObserver* translator);

  /// Return a QMap of all the event Observers.
  QMap<QString, pqEventObserver*> eventObservers() const;

  /// Returns if the utility is currently playing a test.
  bool playingTest() const { return this->PlayingTest; }

  /// Plays back the tests given by the filename(s). This is a blocking call i.e.
  /// it does not return until all the tests have been played or any of the test aborted due to
  /// failure. Returns true if the tests exist and played successfully.
  virtual bool playTests(const QStringList& filenames);

  /// start the recording of tests to a file
  void recordTests();
  Q_INVOKABLE void recordTests(const QString& filename);
  Q_INVOKABLE void recordTestsBySuffix(const QString& suffix);

  /// Add custom object properties, which will be saved during the recording
  /// and restored before the playback
  void addObjectStateProperty(QObject* object, const QString& property);
  QMap<QObject*, QStringList> objectStateProperty() const;

  /// add a directory for recording/playback of file dialogs
  void addDataDirectory(const QString& label, const QDir& path);
  /// remove a directory for recording/playback of file dialogs
  bool removeDataDirectory(const QString& label);
  /// recover all dataDirectories
  QMap<QString, QDir> dataDirectory() const;

  /// given filename convert to one of the data directories
  QString convertToDataDirectory(const QString& file);
  /// give a filename convert from one of the data directories
  QString convertFromDataDirectory(const QString& file);

  // Get current test filname if any
  const QString& filename() const { return this->Filename; };

  /// True if a dialog is opened when recording, false otherwise
  bool recordWithDialog() const;
  /// Set whether a dialog is opened when recording.
  void setRecordWithDialog(bool withDialog);

  /**
   * Wether a pqTestUtility implementation supports
   * dashboard mode. Always return false in this implementation.
   */
  virtual bool supportsDashboardMode() { return false; };

public Q_SLOTS:
  bool playTests(const QString& filename);

  /// @note Dialog is deleted on close.
  pqPlayBackEventsDialog* openPlayerDialog();

  void stopTests();
  void stopRecords(int value);
  void pauseRecords(bool value);

  void onRecordStopped();

  /**
   * Slot called when the dashboard mode checkbox is changed
   * and just before recording / stopping recording events.
   * Not implemented in this implementation.
   */
  virtual void setDashboardMode(bool){};

  /**
   * Update the players if needed by the environnement
   * called before playing events.
   * Not implemented in this implementation.
   */
  virtual void updatePlayers(){};

  /**
   * Update the translators if needed by the environnement
   * called before and after recording events.
   * Not implemented in this implementation.
   */
  virtual void updateTranslators(){};

Q_SIGNALS:
  void playbackStarted();
  void playbackStopped();
  void playbackStarted(const QString& filename);
  void playbackStopped(const QString& filename, bool error);

private:
  QMap<QString, QDir>::iterator findBestIterator(
    const QString& file, QMap<QString, QDir>::iterator startIter);
  bool objectStatePropertyAlreadyAdded(QObject* object, const QString& property);

protected:
  pqEventRecorder Recorder;
  pqEventDispatcher Dispatcher;
  pqEventPlayer Player;
  pqEventTranslator Translator;
  bool PlayingTest;
  bool RecordWithDialog;

  QString Filename;
  QIODevice* File;
  QString FileSuffix;

  QMap<QString, pqEventSource*> EventSources;
  QMap<QString, pqEventObserver*> EventObservers;

  QMap<QString, QDir> DataDirectories;
  QMap<QObject*, QStringList> ObjectStateProperty;
};

#endif // !_pqTestUtility_h
