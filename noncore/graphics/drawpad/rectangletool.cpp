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

#include <qpainter.h>
#include "rectangletool.h"

#include "drawpad.h"
#include "drawpadcanvas.h"

RectangleTool::RectangleTool(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas)
    : ShapeTool(drawPad, drawPadCanvas)
{
}

RectangleTool::~RectangleTool()
{
}

void RectangleTool::drawFinalShape(QPainter& p)
{
    p.setRasterOp(Qt::NotROP);
    p.drawRect(QRect(m_polyline[2], m_polyline[0]));
    p.setPen(m_pDrawPad->pen());
    p.setRasterOp(Qt::CopyROP);
    p.drawRect(QRect(m_polyline[2], m_polyline[0]));
}

void RectangleTool::drawTemporaryShape(QPainter& p)
{
    p.setRasterOp(Qt::NotROP);
    p.drawRect(QRect(m_polyline[2], m_polyline[1]));
    p.drawRect(QRect(m_polyline[2], m_polyline[0]));
}
