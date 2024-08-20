// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqPlayBackEventsDialog_h
#define _pqPlayBackEventsDialog_h

#include "QtTestingExport.h"
#include <QDialog>

class pqEventPlayer;
class pqEventDispatcher;
class pqTestUtility;

/// Provides a standard dialog that will PlayBack user input to an XML file as long as the dialog
/// remains open
class QTTESTING_EXPORT pqPlayBackEventsDialog : public QDialog
{
  Q_OBJECT

  typedef QDialog Superclass;

public:
  /**
  Creates the dialog and begins translating user input with the supplied translator.
  */
  pqPlayBackEventsDialog(
    pqEventPlayer& Player, pqEventDispatcher& Source, pqTestUtility* TestUtility, QWidget* Parent);
  ~pqPlayBackEventsDialog() override;

private Q_SLOTS:
  void onEventAboutToBePlayed(const QString&, const QString&, const QString&);
  void loadFiles();
  void insertFiles();
  void removeFiles();
  void onPlayOrPause(bool);
  void onOneStep();
  void onStarted();
  void onStarted(const QString&);
  void onStopped();
  void onModal(bool value);

public Q_SLOTS:
  void done(int) override;
  void updateUi();

protected:
  void moveEvent(QMoveEvent* event) override;

private:
  void loadFiles(const QStringList& filenames);
  void addFile(const QString& filename);
  QStringList selectedFileNames() const;

  pqPlayBackEventsDialog(const pqPlayBackEventsDialog&);            // Not Implemented
  pqPlayBackEventsDialog& operator=(const pqPlayBackEventsDialog&); // Not Implemented

  struct pqImplementation;
  pqImplementation* const Implementation;
};

#endif // !_pqPlayBackEventsDialog_h
