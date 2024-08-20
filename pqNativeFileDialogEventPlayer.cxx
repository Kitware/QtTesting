// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqNativeFileDialogEventPlayer.h"

#include "pqEventDispatcher.h"
#include "pqTestUtility.h"
#include <QApplication>
#include <QEvent>
#include <QFileDialog>

#if QT_VERSION < QT_VERSION_CHECK(5, 3, 0)

typedef QString (*_qt_filedialog_existing_directory_hook)(
  QWidget* parent, const QString& caption, const QString& dir, QFileDialog::Options options);
extern Q_DECL_IMPORT _qt_filedialog_existing_directory_hook qt_filedialog_existing_directory_hook;

typedef QString (*_qt_filedialog_open_filename_hook)(QWidget* parent, const QString& caption,
  const QString& dir, const QString& filter, QString* selectedFilter, QFileDialog::Options options);
extern Q_DECL_IMPORT _qt_filedialog_open_filename_hook qt_filedialog_open_filename_hook;

typedef QStringList (*_qt_filedialog_open_filenames_hook)(QWidget* parent, const QString& caption,
  const QString& dir, const QString& filter, QString* selectedFilter, QFileDialog::Options options);
extern Q_DECL_IMPORT _qt_filedialog_open_filenames_hook qt_filedialog_open_filenames_hook;

typedef QString (*_qt_filedialog_save_filename_hook)(QWidget* parent, const QString& caption,
  const QString& dir, const QString& filter, QString* selectedFilter, QFileDialog::Options options);
extern Q_DECL_IMPORT _qt_filedialog_save_filename_hook qt_filedialog_save_filename_hook;

namespace
{
pqNativeFileDialogEventPlayer* self;
_qt_filedialog_existing_directory_hook old_existing_directory_hook;
_qt_filedialog_open_filename_hook old_open_filename_hook;
_qt_filedialog_open_filenames_hook old_open_filenames_hook;
_qt_filedialog_save_filename_hook old_save_filename_hook;

QEventLoop* loop = 0;
QString filename;
QStringList filenames;

QString dir_hook(QWidget* pqNotUsed(parent), const QString& pqNotUsed(caption),
  const QString& pqNotUsed(dir), QFileDialog::Options pqNotUsed(options))
{
  // wait for next event played to give us a filename
  loop->exec();
  return filename;
}

QString filename_hook(QWidget* pqNotUsed(parent), const QString& pqNotUsed(caption),
  const QString& pqNotUsed(dir), const QString& pqNotUsed(filter),
  QString* pqNotUsed(selectedFilter), QFileDialog::Options pqNotUsed(options))
{
  // wait for next event played to give us a filename
  loop->exec();
  return filename;
}

QStringList filenames_hook(QWidget* pqNotUsed(parent), const QString& pqNotUsed(caption),
  const QString& pqNotUsed(dir), const QString& pqNotUsed(filter),
  QString* pqNotUsed(selectedFilter), QFileDialog::Options pqNotUsed(options))
{
  // wait for next event played to give us a filename
  loop->exec();
  return filenames;
}
}

pqNativeFileDialogEventPlayer::pqNativeFileDialogEventPlayer(pqTestUtility* util, QObject* p)
  : pqWidgetEventPlayer(p)
  , mUtil(util)
{
  if (!loop)
  {
    loop = new QEventLoop();
  }

  QObject::connect(util, SIGNAL(playbackStarted()), this, SLOT(start()));
  QObject::connect(util, SIGNAL(playbackStopped()), this, SLOT(stop()));
}

pqNativeFileDialogEventPlayer::~pqNativeFileDialogEventPlayer()
{
  if (loop)
  {
    delete loop;
    loop = NULL;
  }
}

void pqNativeFileDialogEventPlayer::start()
{
  self = this;

  // existing dir hook
  old_existing_directory_hook = qt_filedialog_existing_directory_hook;
  qt_filedialog_existing_directory_hook = dir_hook;

  // open file hook
  old_open_filename_hook = qt_filedialog_open_filename_hook;
  qt_filedialog_open_filename_hook = filename_hook;

  // open files hook
  old_open_filenames_hook = qt_filedialog_open_filenames_hook;
  qt_filedialog_open_filenames_hook = filenames_hook;

  // save file hook
  old_save_filename_hook = qt_filedialog_save_filename_hook;
  qt_filedialog_save_filename_hook = filename_hook;
}

void pqNativeFileDialogEventPlayer::stop()
{
  self = NULL;
  qt_filedialog_existing_directory_hook = old_existing_directory_hook;
  qt_filedialog_open_filename_hook = old_open_filename_hook;
  qt_filedialog_open_filenames_hook = old_open_filenames_hook;
  qt_filedialog_save_filename_hook = old_save_filename_hook;
}

bool pqNativeFileDialogEventPlayer::playEvent(
  QObject* Object, const QString& Command, const QString& Arguments, bool& pqNotUsed(Error))
{

  if (!qobject_cast<QApplication*>(Object))
  {
    return false;
  }

  QStringList normalized_files = Arguments.split(";");
  QStringList files;

  Q_FOREACH (QString file, normalized_files)
  {
    files.append(mUtil->convertFromDataDirectory(file));
  }

  if (Command == "FileOpen" || Command == "DirOpen" || Command == "FileSave")
  {
    filename = files.join(";");
    loop->quit();
    return true;
  }
  else if (Command == "FilesOpen")
  {
    filenames = files;
    loop->quit();
    return true;
  }

  return false;
}
#else
//-----------------------------------------------------------------------------
pqNativeFileDialogEventPlayer::pqNativeFileDialogEventPlayer(pqTestUtility* util, QObject* p)
  : pqWidgetEventPlayer(p)
  , mUtil(util)
{
}
pqNativeFileDialogEventPlayer::~pqNativeFileDialogEventPlayer() {}
void pqNativeFileDialogEventPlayer::start() {}
void pqNativeFileDialogEventPlayer::stop() {}

bool pqNativeFileDialogEventPlayer::playEvent(
  QObject* Object, const QString& Command, const QString& Arguments, bool& Error)

{
  Q_UNUSED(Object);
  Q_UNUSED(Command);
  Q_UNUSED(Arguments);
  Q_UNUSED(Error);
  return false;
}

#endif
