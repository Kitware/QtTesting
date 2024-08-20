// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqPythonEventObserver.h"

#include <QTextStream>

////////////////////////////////////////////////////////////////////////////////////
// pqPythonEventObserver

pqPythonEventObserver::pqPythonEventObserver(QObject* p)
  : pqEventObserver(p)
{
}

pqPythonEventObserver::~pqPythonEventObserver() {}

void pqPythonEventObserver::setStream(QTextStream* stream)
{
  pqEventObserver::setStream(stream);
  if (this->Stream)
  {
    *this->Stream << "#/usr/bin/env python\n\n";
    *this->Stream << "import QtTesting\n\n";
  }
}

void pqPythonEventObserver::onRecordEvent(
  const QString& Widget, const QString& Command, const QString& Arguments, const int& eventType)
{
  if (this->Stream)
  {
    QString varname = this->Names[Widget];
    if (varname.isEmpty())
    {
      varname = QString("object%1").arg(this->Names.count());
      this->Names.insert(Widget, varname);
      QString objname("%1 = '%2'");
      objname = objname.arg(varname);
      objname = objname.arg(Widget);
      *this->Stream << objname << "\n";
    }

    QString pycommand("QtTesting.playCommand(%1, '%2', '%3', '%4')");
    pycommand = pycommand.arg(varname);
    pycommand = pycommand.arg(Command);
    pycommand = pycommand.arg(Arguments);
    pycommand = pycommand.arg(eventType);
    *this->Stream << pycommand << "\n";

    Q_EMIT eventRecorded(widget, command, arguments, eventType);
  }
}
