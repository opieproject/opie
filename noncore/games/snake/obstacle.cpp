/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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



Obstacle::Obstacle(QCanvas* canvas, int y)
         : QCanvasSprite(0,canvas)
{
    newObstacle(y);
} 

void Obstacle::newObstacle(int y)
{
   QPixmap obstaclePix( Resource::findPixmap("snake/wall.png") );
   
   if ( obstaclePix.width() > canvas()->width()*3/5 ) {
       int w = canvas()->width()*3/5;
       w = w - w % 16;
       obstaclePix.resize( w, obstaclePix.height() );
   }

   QList<QPixmap> pixl;
   pixl.append( &obstaclePix );

   QPoint nullp;
   QList<QPoint> pl;
   pl.append( &nullp );
   
   QCanvasPixmapArray* obstaclearray = new QCanvasPixmapArray(pixl, pl);
   setSequence(obstaclearray);
   
   int x = ( canvas()->width() - obstaclePix.width() )/2;
   x = x - x % 16;
   y = y - y % 16;
   move(x, y);
   setZ( -100 );
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
