// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqRecordEventsDialog_h
#define _pqRecordEventsDialog_h

#include "QtTestingExport.h"
#include <QDialog>

class pqEventRecorder;
class pqTestUtility;

/// Provides a standard dialog that will record user input to an XML file as long as the dialog
/// remains open
class QTTESTING_EXPORT pqRecordEventsDialog : public QDialog
{
  Q_OBJECT

public:
  /**
  Creates the dialog and begins translating user input with the supplied translator.
  */
  pqRecordEventsDialog(pqEventRecorder* recorder, pqTestUtility* testUtility, QWidget* Parent);

private Q_SLOTS:
  void done(int) override;
  void onEventRecorded(const QString&, const QString&, const QString&, int eventType);

  void addComment();

  /**
   * Called when dashboard mode is toggled.
   * Call setDashboardMode on the test utility
   * and record a dashboard mode toggle event on the main window
   */
  void onDashboardModeToggled(bool toggle);

public Q_SLOTS:
  void updateUi();

private:
  pqRecordEventsDialog(const pqRecordEventsDialog&);
  pqRecordEventsDialog& operator=(const pqRecordEventsDialog&);
  ~pqRecordEventsDialog() override;

  void ignoreObject(QObject* object);

  struct pqImplementation;
  pqImplementation* const Implementation;
};

#endif // !_pqRecordEventsDialog_h
