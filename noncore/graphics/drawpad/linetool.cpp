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

#include "linetool.h"

#include "drawpad.h"
#include "drawpadcanvas.h"

#include <qimage.h>
#include <qpixmap.h>

LineTool::LineTool(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas)
    : ShapeTool(drawPad, drawPadCanvas)
{
}

LineTool::~LineTool()
{
}

void LineTool::drawFinalShape(QPainter& p)
{
    p.setRasterOp(Qt::NotROP);
    p.drawLine(m_polyline[2], m_polyline[0]);

    QRect r = m_polyline.boundingRect();
    r = r.normalize();
    r.setLeft(r.left() - m_pDrawPad->pen().width());
    r.setTop(r.top() - m_pDrawPad->pen().width());
    r.setRight(r.right() + m_pDrawPad->pen().width());
    r.setBottom(r.bottom() + m_pDrawPad->pen().width());

    QPixmap areaPixmap(r.width(), r.height());
    bitBlt(&areaPixmap, QPoint(0, 0), p.device(), r);

    QImage areaImage = areaPixmap.convertToImage();
    QImage bigAreaImage = areaImage.smoothScale(areaImage.width() * 3, areaImage.height() * 3);

    QPixmap bigAreaPixmap;
    bigAreaPixmap.convertFromImage(bigAreaImage);

    QPen bigAreaPen = m_pDrawPad->pen();
    bigAreaPen.setWidth(bigAreaPen.width() * 3);

    QPainter bigAreaPainter;
    bigAreaPainter.begin(&bigAreaPixmap);
    bigAreaPainter.setPen(bigAreaPen);

    if (((m_polyline[2].x() < m_polyline[0].x()) && (m_polyline[2].y() < m_polyline[0].y()))
        || ((m_polyline[2].x() > m_polyline[0].x()) && (m_polyline[2].y() > m_polyline[0].y()))) {

        bigAreaPainter.drawLine(bigAreaPen.width() + 1, bigAreaPen.width() + 1,
                                bigAreaPixmap.width() - bigAreaPen.width() - 2,
                                bigAreaPixmap.height() - bigAreaPen.width() - 2);
    } else {
        bigAreaPainter.drawLine(bigAreaPen.width() + 1, bigAreaPixmap.height() - bigAreaPen.width() - 2,
                                bigAreaPixmap.width() - bigAreaPen.width() - 2, bigAreaPen.width() + 1);
    }

    bigAreaPainter.end();

    bigAreaImage = bigAreaPixmap.convertToImage();
    areaImage = bigAreaImage.smoothScale(bigAreaImage.width() / 3, bigAreaImage.height() / 3);
    areaPixmap.convertFromImage(areaImage);

    p.setRasterOp(Qt::CopyROP);
    p.drawPixmap(r.x(), r.y(), areaPixmap);
}

void LineTool::drawTemporaryShape(QPainter& p)
{
    p.setRasterOp(Qt::NotROP);
    p.drawLine(m_polyline[2], m_polyline[1]);
    p.drawLine(m_polyline[2], m_polyline[0]);
}
