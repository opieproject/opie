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

#include "obstacle.h"
#include "codes.h"

#include <qpe/resource.h>

Obstacle::Obstacle(QCanvas* canvas, int x, int y)
         : QCanvasSprite(0, canvas)
{
   newObstacle(x, y);
} 

void Obstacle::newObstacle(int x, int y)
{
   QCanvasPixmapArray* obstaclearray = new QCanvasPixmapArray(Resource::findPixmap("snake/wall.png"));
 
   setSequence(obstaclearray);
 
   move(x, y);
 
   show();
   canvas()->update();
}
 
int Obstacle::rtti() const
{
   return obstacle_rtti;
}

Obstacle::~Obstacle()
{
}
