// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqEventObserver.h"

#include <QTextStream>

////////////////////////////////////////////////////////////////////////////////////
// pqEventObserver

pqEventObserver::pqEventObserver(QObject* p)
  : QObject(p)
  , Stream(NULL)
{
}

pqEventObserver::~pqEventObserver() {}

void pqEventObserver::setStream(QTextStream* stream)
{
  this->Stream = stream;
}
