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

#include "filledrectangletool.h"

#include "drawpad.h"
#include "drawpadcanvas.h"

FilledRectangleTool::FilledRectangleTool(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas)
    : ShapeTool(drawPad, drawPadCanvas)
{
}

FilledRectangleTool::~FilledRectangleTool()
{
}

void FilledRectangleTool::drawFinalShape(QPainter& p)
{
    p.setRasterOp(Qt::NotROP);
    p.drawRect(QRect(m_polyline[2], m_polyline[0]));
    p.setRasterOp(Qt::CopyROP);
    p.fillRect(QRect(m_polyline[2], m_polyline[0]), m_pDrawPad->brush());
    p.setPen(m_pDrawPad->pen());
    p.drawRect(QRect(m_polyline[2], m_polyline[0]));
}

void FilledRectangleTool::drawTemporaryShape(QPainter& p)
{
    p.setRasterOp(Qt::NotROP);
    p.drawRect(QRect(m_polyline[2], m_polyline[1]));
    p.drawRect(QRect(m_polyline[2], m_polyline[0]));
}
