/***************************************************************************
 *                                                                         *
 *   DrawPad - a drawing program for Opie Environment                      *
 *                                                                         *
 *   (C) 2002 by S. Prud'homme <prudhomme@laposte.net>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "linedrawmode.h"

#include "drawpad.h"
#include "drawpadcanvas.h"

LineDrawMode::LineDrawMode(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas)
    : ShapeDrawMode(drawPad, drawPadCanvas)
{
}

LineDrawMode::~LineDrawMode()
{
}

void LineDrawMode::drawFinalShape(QPainter& p)
{
    p.setPen(m_pDrawPad->pen());
    p.drawLine(m_polyline[2], m_polyline[0]);
}

void LineDrawMode::drawTemporaryShape(QPainter& p)
{
    p.setRasterOp(Qt::NotROP);
    p.drawLine(m_polyline[2], m_polyline[1]);
    p.drawLine(m_polyline[2], m_polyline[0]);
}
