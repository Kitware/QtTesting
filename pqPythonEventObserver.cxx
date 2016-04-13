/*=========================================================================

   Program: ParaView
   Module:    pqPythonEventObserver.cxx

   Copyright (c) 2005-2008 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 

   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#include "pqPythonEventObserver.h"

#include <QTextStream>

////////////////////////////////////////////////////////////////////////////////////
// pqPythonEventObserver

pqPythonEventObserver::pqPythonEventObserver(QObject* p) 
  : pqEventObserver(p)
{
}

pqPythonEventObserver::~pqPythonEventObserver()
{
}

void pqPythonEventObserver::setStream(QTextStream* stream)
{
  pqEventObserver::setStream(stream);
  if(this->Stream)
    {
    *this->Stream << "#/usr/bin/env python\n\n";
    *this->Stream << "import QtTesting\n\n";
    }
}

void pqPythonEventObserver::onRecordEvent(
  const QString& Widget,
  const QString& Command,
  const QString& Arguments,
  const int& eventType)
{
  if(this->Stream)
    {
    QString varname = this->Names[Widget];
    if(varname == QString::null)
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
    }
}
