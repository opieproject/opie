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

#include "filltool.h"

#include "drawpad.h"
#include "drawpadcanvas.h"

#include <qimage.h>
#include <qpixmap.h>

FillTool::FillTool(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas)
    : Tool(drawPad, drawPadCanvas)
{
}

FillTool::~FillTool()
{
}

void FillTool::mousePressEvent(QMouseEvent* e)
{
    int x = e->x();
    int y = e->y();

    m_image = m_pDrawPadCanvas->currentPage()->convertToImage();
    m_fillRgb = m_pDrawPad->brush().color().rgb();
    m_oldRgb = m_image.pixel(x, y);

    if (m_oldRgb != m_fillRgb) {
        fillLine(x, y);

        m_pDrawPadCanvas->currentPage()->convertFromImage(m_image);
        m_pDrawPadCanvas->viewport()->update();
    }
}

void FillTool::mouseReleaseEvent(QMouseEvent* e)
{
    Q_UNUSED(e)
}

void FillTool::mouseMoveEvent(QMouseEvent* e)
{
    Q_UNUSED(e)
}

void FillTool::fillLine(int x, int y)
{
    if ((x >= 0) && (x < m_image.width()) && (y >= 0) && (y < m_image.height())) {
        if (m_image.pixel(x, y) == m_oldRgb) {
            int x1, x2;

            x1 = x - 1;
            x2 = x + 1;

            while ((x1 >= 0) && (m_image.pixel(x1, y) == m_oldRgb)) {
                x1--;
            }

            while ((x2 < m_image.width()) && (m_image.pixel(x2, y) == m_oldRgb)) {
                x2++;
            }

            for (int i = x1 + 1; i < x2; i++) {
                m_image.setPixel(i, y, m_fillRgb);
            }

            for (int i = x1 + 1; i < x2; i++) {
                fillLine(i, y - 1);
            }

            for (int i = x1 + 1; i < x2; i++) {
                fillLine(i, y + 1);
            }
        }
    }
}
