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

#ifndef ELLIPSEDRAWMODE_H
#define ELLIPSEDRAWMODE_H

#include "shapedrawmode.h"

class EllipseDrawMode : public ShapeDrawMode
{ 
public:
    EllipseDrawMode(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas);
    ~EllipseDrawMode();

protected:
    void drawFinalShape(QPainter& p);
    void drawTemporaryShape(QPainter& p);
};

#endif // ELLIPSEDRAWMODE_H
