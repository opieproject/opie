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

#ifndef SHAPEDRAWMODE_H
#define SHAPEDRAWMODE_H

#include "drawmode.h"

#include <qpointarray.h>

class ShapeDrawMode : public DrawMode
{ 
public:
    ShapeDrawMode(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas);
    ~ShapeDrawMode();

    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);

protected:
    virtual void drawFinalShape(QPainter& p) = 0;
    virtual void drawTemporaryShape(QPainter& p) = 0;

    QPointArray m_polyline;

private:
    bool m_mousePressed;
};

#endif // SHAPEDRAWMODE_H
