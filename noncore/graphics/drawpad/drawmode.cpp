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

#include "drawmode.h"

#include "drawpad.h"
#include "drawpadcanvas.h"

DrawMode::DrawMode(DrawPad* drawPad, DrawPadCanvas* drawPadCanvas)
    : QObject()
{
    m_pDrawPad = drawPad;
    m_pDrawPadCanvas = drawPadCanvas;
}

DrawMode::~DrawMode()
{
}
