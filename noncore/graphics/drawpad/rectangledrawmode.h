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

#ifndef RECTANGLEDRAWMODE_H
#define RECTANGLEDRAWMODE_H

#include "drawmode.h"

#include <qpointarray.h>

class RectangleDrawMode : public DrawMode
{ 
public:
    RectangleDrawMode(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas);
    ~RectangleDrawMode();

    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);

private:
    bool m_mousePressed;
    QPointArray m_polyline;
};

#endif // RECTANGLEDRAWMODE_H
