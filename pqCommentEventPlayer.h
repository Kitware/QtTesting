/*=========================================================================

   Program: ParaView
   Module:    pqNativeFileDialogEventPlayer.h

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

#ifndef __pqCommentEventPlayer_h
#define __pqCommentEventPlayer_h

// QtTesting includes
#include "pqTestUtility.h"
#include "pqWidgetEventPlayer.h"

/// This class is a comment class.
/// Do no action on Object.
/// Emit comment wrote in the xml, which can be catch by someone else.

class QTTESTING_EXPORT pqCommentEventPlayer : public pqWidgetEventPlayer
{
  Q_OBJECT
  typedef pqWidgetEventPlayer Superclass;

public:
  pqCommentEventPlayer(pqTestUtility* testUtility, QObject* p = 0);
  ~pqCommentEventPlayer() override;

  using Superclass::playEvent;
  bool playEvent(
    QObject* Object, const QString& Command, const QString& Arguments, bool& Error) override;

Q_SIGNALS:
  void comment(const QString&);

private:
  pqCommentEventPlayer(const pqCommentEventPlayer&);            // Not implemented
  pqCommentEventPlayer& operator=(const pqCommentEventPlayer&); // Not implemented

  pqTestUtility* TestUtility;
};

#endif // __pqCommentEventPlayer_h
