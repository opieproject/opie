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

#ifndef LINEDRAWMODE_H
#define LINEDRAWMODE_H

#include "shapedrawmode.h"

class LineDrawMode : public ShapeDrawMode
{ 
public:
    LineDrawMode(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas);
    ~LineDrawMode();

protected:
    void drawFinalShape(QPainter& p);
    void drawTemporaryShape(QPainter& p);
};

#endif // LINEDRAWMODE_H
