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

#ifndef FILLEDELLIPSETOOL_H
#define FILLEDELLIPSETOOL_H

#include "shapetool.h"

class FilledEllipseTool : public ShapeTool
{
public:
    FilledEllipseTool(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas);
    ~FilledEllipseTool();

protected:
    void drawFinalShape(QPainter& p);
    void drawTemporaryShape(QPainter& p);
};

#endif // FILLEDELLIPSETOOL_H
