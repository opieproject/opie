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

#include "ellipsetool.h"

#include "drawpad.h"
#include "drawpadcanvas.h"

#include <qpainter.h>
#include <qpixmap.h>

EllipseTool::EllipseTool(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas)
    : ShapeTool(drawPad, drawPadCanvas)
{
}

EllipseTool::~EllipseTool()
{
}

void EllipseTool::drawFinalShape(QPainter& p)
{
    p.setRasterOp(Qt::NotROP);
    p.drawRect(QRect(m_polyline[2], m_polyline[0]));
    p.setPen(m_pDrawPad->pen());
    p.setRasterOp(Qt::CopyROP);
    p.drawEllipse(QRect(m_polyline[2], m_polyline[0]));
}

void EllipseTool::drawTemporaryShape(QPainter& p)
{
    p.setRasterOp(Qt::NotROP);
    p.drawRect(QRect(m_polyline[2], m_polyline[1]));
    p.drawRect(QRect(m_polyline[2], m_polyline[0]));
}
