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

#include "rectangledrawmode.h"

#include "drawpad.h"
#include "drawpadcanvas.h"

#include <qpainter.h>
#include <qpixmap.h>

RectangleDrawMode::RectangleDrawMode(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas)
    : DrawMode(drawPad, drawPadCanvas)
{
    m_mousePressed = false;
    m_polyline.resize(3);
}

RectangleDrawMode::~RectangleDrawMode()
{
}

void RectangleDrawMode::mousePressEvent(QMouseEvent* e)
{
    m_mousePressed = true;
    m_polyline[2] = m_polyline[1] = m_polyline[0] = e->pos();
}

void RectangleDrawMode::mouseReleaseEvent(QMouseEvent* e)
{
    Q_UNUSED(e)

    QPainter painter;
    painter.begin(m_pDrawPadCanvas->currentPage());
    painter.setPen(m_pDrawPad->pen());
    painter.drawRect(QRect(m_polyline[2], m_polyline[0]));
    painter.end();

    QRect r = m_polyline.boundingRect();
    r = r.normalize();
    r.setLeft(r.left() - m_pDrawPad->pen().width());
    r.setTop(r.top() - m_pDrawPad->pen().width());
    r.setRight(r.right() + m_pDrawPad->pen().width());
    r.setBottom(r.bottom() + m_pDrawPad->pen().width());

    bitBlt(m_pDrawPadCanvas, r.x(), r.y(), m_pDrawPadCanvas->currentPage(), r.x(), r.y(), r.width(), r.height());

    m_mousePressed = false;
}

void RectangleDrawMode::mouseMoveEvent(QMouseEvent* e)
{
    if (m_mousePressed) {
        QPainter painter;
        painter.begin(m_pDrawPadCanvas->currentPage());
        painter.setRasterOp(Qt::NotROP);
        m_polyline[0] = e->pos();
        painter.drawRect(QRect(m_polyline[2], m_polyline[1]));
        painter.drawRect(QRect(m_polyline[2], m_polyline[0]));
        painter.end();

        QRect r = m_polyline.boundingRect();
        r = r.normalize();
        r.setLeft(r.left() - m_pDrawPad->pen().width());
        r.setTop(r.top() - m_pDrawPad->pen().width());
        r.setRight(r.right() + m_pDrawPad->pen().width());
        r.setBottom(r.bottom() + m_pDrawPad->pen().width());

        bitBlt(m_pDrawPadCanvas, r.x(), r.y(), m_pDrawPadCanvas->currentPage(), r.x(), r.y(), r.width(), r.height());

        m_polyline[1] = m_polyline[0];
    }
}
