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

#include "shapedrawmode.h"

#include "drawpad.h"
#include "drawpadcanvas.h"

#include <qpainter.h>
#include <qpixmap.h>

ShapeDrawMode::ShapeDrawMode(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas)
    : DrawMode(drawPad, drawPadCanvas)
{
    m_mousePressed = false;
    m_polyline.resize(3);
}

ShapeDrawMode::~ShapeDrawMode()
{
}

void ShapeDrawMode::mousePressEvent(QMouseEvent* e)
{
    m_mousePressed = true;
    m_polyline[2] = m_polyline[1] = m_polyline[0] = e->pos();
}

void ShapeDrawMode::mouseReleaseEvent(QMouseEvent* e)
{
    Q_UNUSED(e)

    QPainter painter;
    painter.begin(m_pDrawPadCanvas->currentPage());
    drawFinalShape(painter);
    painter.end();

    QRect r = m_polyline.boundingRect();
    r = r.normalize();
    r.setLeft(r.left() - m_pDrawPad->pen().width());
    r.setTop(r.top() - m_pDrawPad->pen().width());
    r.setRight(r.right() + m_pDrawPad->pen().width());
    r.setBottom(r.bottom() + m_pDrawPad->pen().width());

    QRect viewportRect(m_pDrawPadCanvas->contentsToViewport(r.topLeft()),
                       m_pDrawPadCanvas->contentsToViewport(r.bottomRight()));

    bitBlt(m_pDrawPadCanvas->viewport(), viewportRect.x(), viewportRect.y(),
           m_pDrawPadCanvas->currentPage(), r.x(), r.y(), r.width(), r.height());

    m_pDrawPadCanvas->viewport()->update(viewportRect);

    m_mousePressed = false;
}

void ShapeDrawMode::mouseMoveEvent(QMouseEvent* e)
{
    if (m_mousePressed) {
        m_polyline[0] = e->pos();
        QPainter painter;
        painter.begin(m_pDrawPadCanvas->currentPage());
        drawTemporaryShape(painter);
        painter.end();

        QRect r = m_polyline.boundingRect();
        r = r.normalize();
        r.setLeft(r.left() - m_pDrawPad->pen().width());
        r.setTop(r.top() - m_pDrawPad->pen().width());
        r.setRight(r.right() + m_pDrawPad->pen().width());
        r.setBottom(r.bottom() + m_pDrawPad->pen().width());

        QRect viewportRect(m_pDrawPadCanvas->contentsToViewport(r.topLeft()),
                           m_pDrawPadCanvas->contentsToViewport(r.bottomRight()));

        bitBlt(m_pDrawPadCanvas->viewport(), viewportRect.x(), viewportRect.y(),
               m_pDrawPadCanvas->currentPage(), r.x(), r.y(), r.width(), r.height());

        m_pDrawPadCanvas->viewport()->update(viewportRect);

        m_polyline[1] = m_polyline[0];
    }
}
