// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

// Qt includes
#include <QDebug>

// QtTesting includes
#include "pqEventComment.h"

// ----------------------------------------------------------------------------
pqEventComment::pqEventComment(pqTestUtility* util, QObject* parent)
  : Superclass(parent)
{
  this->TestUtility = util;
}

// ----------------------------------------------------------------------------
pqEventComment::~pqEventComment()
{
  this->TestUtility = 0;
}

// ----------------------------------------------------------------------------
void pqEventComment::recordComment(const QString& arguments)
{
  this->recordComment(QString("comment"), arguments);
}

// ----------------------------------------------------------------------------
void pqEventComment::recordCommentBlock(const QString& arguments)
{
  this->recordComment(QString("comment-block"), arguments);
}

// ----------------------------------------------------------------------------
void pqEventComment::recordComment(
  const QString& command, const QString& arguments, QObject* object)
{
  if (arguments.isEmpty())
  {
    qCritical() << "The comment is empty ! No comment has been added !";
    return;
  }

  Q_EMIT this->recordComment(object, command, arguments);
}
