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

#ifndef RECTANGLETOOL_H
#define RECTANGLETOOL_H

#include "shapetool.h"

class RectangleTool : public ShapeTool
{
public:
    RectangleTool(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas);
    ~RectangleTool();

protected:
    void drawFinalShape(QPainter& p);
    void drawTemporaryShape(QPainter& p);
};

#endif // RECTANGLETOOL_H
