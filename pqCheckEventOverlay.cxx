// SPDX-FileCopyrightText: Copyright (c) Kitware Inc.
// SPDX-FileCopyrightText: Copyright (c) Sandia Corporation
// SPDX-License-Identifier: BSD-3-Clause

#include "pqCheckEventOverlay.h"

#include <QPainter>

pqCheckEventOverlay::pqCheckEventOverlay(QWidget* parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_NoSystemBackground);
  setAttribute(Qt::WA_TransparentForMouseEvents);
  this->Valid = false;
  this->GlWidget = false;
  this->Specific = false;
  this->setObjectName("Overlay");
}

void pqCheckEventOverlay::paintEvent(QPaintEvent*)
{
  QPainter p(this);
  // Draw red on invalid widget
  QPen pen(Qt::red, pqCheckEventOverlay::OVERLAY_PEN_WIDTH);
  if (this->Valid)
  {
    // Draw green on valid widget
    pen.setColor(Qt::green);
  }
  p.setPen(pen);

  // Remove the margins to draw
  p.drawRect(0, 0, width() - pqCheckEventOverlay::OVERLAY_MARGIN,
    height() - pqCheckEventOverlay::OVERLAY_MARGIN);
}
