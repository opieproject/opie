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

#ifndef TOOL_H
#define TOOL_H

#include <qobject.h>

class DrawPad;
class DrawPadCanvas;

class Tool : public QObject
{
protected:
    Tool(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas);

public:
    virtual ~Tool();

    virtual void mousePressEvent(QMouseEvent* e) = 0;
    virtual void mouseReleaseEvent(QMouseEvent* e) = 0;
    virtual void mouseMoveEvent(QMouseEvent* e) = 0;

protected:
    DrawPad* m_pDrawPad;
    DrawPadCanvas* m_pDrawPadCanvas;
};

#endif // TOOL_H
