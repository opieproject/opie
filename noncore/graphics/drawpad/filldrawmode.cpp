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

#include "filldrawmode.h"

#include "drawpad.h"
#include "drawpadcanvas.h"

#include <qimage.h>
#include <qpixmap.h>

FillDrawMode::FillDrawMode(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas)
    : DrawMode(drawPad, drawPadCanvas)
{
}

FillDrawMode::~FillDrawMode()
{
}

void FillDrawMode::mousePressEvent(QMouseEvent* e)
{
    int x = e->x();
    int y = e->y();

    m_image = m_pDrawPadCanvas->currentPage()->convertToImage();
    m_fillRgb = m_pDrawPad->brush().color().rgb();
    m_oldRgb = m_image.pixel(x, y);

    if (m_oldRgb != m_fillRgb) {
        m_image.setPixel(x, y, m_fillRgb);
        fillEast(x + 1, y);
        fillSouth(x, y + 1);
        fillWest(x - 1, y);
        fillNorth(x, y - 1);

        m_pDrawPadCanvas->currentPage()->convertFromImage(m_image);
        m_pDrawPadCanvas->repaint();
    }
}

void FillDrawMode::mouseReleaseEvent(QMouseEvent* e)
{
    Q_UNUSED(e)
}

void FillDrawMode::mouseMoveEvent(QMouseEvent* e)
{
    Q_UNUSED(e)
}

void FillDrawMode::fillEast(int x, int y)
{
    if (x < m_image.width()) {
        if (m_image.pixel(x, y) == m_oldRgb) {
            m_image.setPixel(x, y, m_fillRgb);
            fillEast(x + 1, y);
            fillSouth(x, y + 1);
            fillNorth(x, y - 1);
        }
    }
}

void FillDrawMode::fillSouth(int x, int y)
{
    if (y < m_image.height()) {
        if (m_image.pixel(x, y) == m_oldRgb) {
            m_image.setPixel(x, y, m_fillRgb);
            fillEast(x + 1, y);
            fillSouth(x, y + 1);
            fillWest(x - 1, y);
        }
    }
}

void FillDrawMode::fillWest(int x, int y)
{
    if (x >= 0) {
        if (m_image.pixel(x, y) == m_oldRgb) {
            m_image.setPixel(x, y, m_fillRgb);
            fillSouth(x, y + 1);
            fillWest(x - 1, y);
            fillNorth(x, y - 1);
        }
    }
}

void FillDrawMode::fillNorth(int x, int y)
{
    if (y >= 0) {
        if (m_image.pixel(x, y) == m_oldRgb) {
            m_image.setPixel(x, y, m_fillRgb);
            fillEast(x + 1, y);
            fillWest(x - 1, y);
            fillNorth(x, y - 1);
        }
    }
}
