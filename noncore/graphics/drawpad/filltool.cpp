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
#include "page.h"

#include <qimage.h>

const int FILL_THRESHOLD = 65536;

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
        if (m_pDrawPad->antiAliasing()) {
            m_mask.create(m_image.width(), m_image.height(), 8, 2);
            m_mask.fill(0);

            fillMaskLine(x, y);

            for (int i = 0; i < m_image.width(); i++) {
                for (int j = 0; j < m_image.height(); j++) {
                    if (m_mask.pixelIndex(i, j) == 1) {
                        setInterpolatedPixel(i, j);
                    }
                }
            }

        } else {
            fillLine(x, y);
        }

        m_pDrawPadCanvas->currentPage()->convertFromImage(m_image);
        m_pDrawPadCanvas->viewport()->update();

        m_pDrawPadCanvas->backupPage();
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

void FillTool::fillMaskLine(int x, int y)
{
    if ((x >= 0) && (x < m_image.width()) && (y >= 0) && (y < m_image.height())) {
        if (m_mask.pixelIndex(x, y) == 0) {
                if (rgbDistance(m_image.pixel(x, y), m_oldRgb) < FILL_THRESHOLD) {
                int x1, x2;

                x1 = x - 1;
                x2 = x + 1;

                while ((x1 >= 0) && (rgbDistance(m_image.pixel(x1, y), m_oldRgb) < FILL_THRESHOLD)) {
                    x1--;
                }

                while ((x2 < m_image.width()) && (rgbDistance(m_image.pixel(x2, y), m_oldRgb) < FILL_THRESHOLD)) {
                    x2++;
                }

                for (int i = x1 + 1; i < x2; i++) {
                    m_mask.setPixel(i, y, 1);
                }

                for (int i = x1 + 1; i < x2; i++) {
                    fillMaskLine(i, y - 1);
                }

                for (int i = x1 + 1; i < x2; i++) {
                    fillMaskLine(i, y + 1);
                }
            }
        }
    }
}

void FillTool::setInterpolatedPixel(int x, int y)
{
    int fillRed = QMIN(QMAX(qRed(m_fillRgb) + qRed(m_image.pixel(x, y)) - qRed(m_oldRgb), 0), 255);
    int fillGreen = QMIN(QMAX(qGreen(m_fillRgb) + qGreen(m_image.pixel(x, y)) - qGreen(m_oldRgb), 0), 255);
    int fillBlue = QMIN(QMAX(qBlue(m_fillRgb) + qBlue(m_image.pixel(x, y)) - qBlue(m_oldRgb), 0), 255);

    m_image.setPixel(x, y, qRgb(fillRed, fillGreen, fillBlue));
}

int FillTool::rgbDistance(QRgb rgb1, QRgb rgb2)
{
    int redDistance = qRed(rgb2) - qRed(rgb1);
    int greenDistance = qGreen(rgb2) - qGreen(rgb1);
    int blueDistance = qBlue(rgb2) - qBlue(rgb1);

    return (redDistance * redDistance + greenDistance * greenDistance + blueDistance * blueDistance);
}
