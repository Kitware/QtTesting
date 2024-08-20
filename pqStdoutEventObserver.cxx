// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqStdoutEventObserver.h"
#include <stdio.h>

void pqStdoutEventObserver::onRecordEvent(
  const QString& Widget, const QString& Command, const QString& Arguments, const int& eventType)
{
  printf("event: %s %s %s %i\n", Widget.toUtf8().data(), Command.toUtf8().data(),
    Arguments.toUtf8().data(), eventType);
}
