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

#ifndef FILLDRAWMODE_H
#define FILLDRAWMODE_H

#include "drawmode.h"

#include <qimage.h>

class FillDrawMode : public DrawMode
{ 
public:
    FillDrawMode(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas);
    ~FillDrawMode();

    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);

private:
    void fillEast(int x, int y);
    void fillSouth(int x, int y);
    void fillWest(int x, int y);
    void fillNorth(int x, int y);

    QImage m_image;
    QRgb m_fillRgb;
    QRgb m_oldRgb;
};

#endif // FILLDRAWMODE_H
