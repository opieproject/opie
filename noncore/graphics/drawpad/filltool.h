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

#ifndef FILLTOOL_H
#define FILLTOOL_H

#include "tool.h"

#include <qimage.h>

class FillTool : public Tool
{
public:
    FillTool(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas);
    ~FillTool();

    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);

private:
    void fillLine(int x, int y);

    QImage m_image;
    QRgb m_fillRgb;
    QRgb m_oldRgb;
};

#endif // FILLTOOL_H
