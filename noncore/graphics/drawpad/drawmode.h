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

#ifndef DRAWMODE_H
#define DRAWMODE_H

#include <qobject.h>

class DrawPad;
class DrawPadCanvas;

class DrawMode : QObject
{ 
public:
    DrawMode(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas);
    ~DrawMode();

    virtual void mousePressEvent(QMouseEvent* e) = 0;
    virtual void mouseReleaseEvent(QMouseEvent* e) = 0;
    virtual void mouseMoveEvent(QMouseEvent* e) = 0;

protected:
    DrawPad* m_pDrawPad;
    DrawPadCanvas* m_pDrawPadCanvas;
};

#endif // DRAWMODE_H
