// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#ifndef _pqCheckEventOverlay_h
#define _pqCheckEventOverlay_h

#include <QWidget>
/**
Display a green or red overlay rectangle on top of parent widget
\sa pqEventTranslator
*/
class pqCheckEventOverlay : public QWidget
{
  Q_OBJECT
  typedef QWidget Superclass;

public:
  pqCheckEventOverlay(QWidget* parent = 0);

  // true if the overlayed widget can be checked, false otherwise
  bool Valid;

  // true if the overlayed widget in an opengl widget, false otherwise
  bool GlWidget;

  // true if the size of the overlay is specifically defined
  // via resize slot, false otherwise
  bool Specific;

  // Static overlauy margin
  static const int OVERLAY_MARGIN = 2;

  // Static overlay pen width
  static const int OVERLAY_PEN_WIDTH = 5;

protected:
  void paintEvent(QPaintEvent*) override;
};

#endif
