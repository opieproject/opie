/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef CANVAS_SHAPES_H
#define CANVAS_SHAPES_H


#include <qcanvas.h>


class QPainter;


class CanvasRoundRect : QCanvasRectangle
{
public:
    CanvasRoundRect(int x, int y, QCanvas *canvas);
    void redraw();
protected:
    void drawShape(QPainter &p);
};


class CanvasCircleOrCross : QCanvasRectangle
{
public:
    CanvasCircleOrCross(int x, int y, QCanvas *canvas);
    void redraw();
    void setCircle();
    void setCross();
protected:
    void drawShape(QPainter &p);
private:
    bool circleShape;
};


#endif

