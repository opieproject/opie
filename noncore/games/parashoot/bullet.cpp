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

#include "codes.h"  
#include "bullet.h"
#include "man.h"
#include "helicopter.h"

#include <qpe/resource.h>
#include <qpe/qmath.h>


int limit;
int shotcount;
int nobullets;

Bullet::Bullet(QCanvas* canvas, double angle, int cannonx, int cannony) :
        QCanvasSprite(0, canvas),
	bang("collide01")
{
     QCanvasPixmapArray* bulletarray = new QCanvasPixmapArray(Resource::findPixmap("parashoot/bullet"));
     setSequence(bulletarray);
     if (nobullets < limit) { 
        nobullets++;
        move(cannonx, cannony);
        dy = 0;
        dx = 0;
        show();
        setXY(angle);
        setVelocity(-dx, -dy);
	bang.play();
     } else 
        return;
}

void Bullet::setXY(double angle)
{
   double ang = angle;
   if ( (y() < 0) || (x() < 0) || (y() > canvas()->height()) ||
        (x() > canvas()->width()) )
       delete this; 
   else { 
       double radians = 0;
       radians = ang * 3.14159265/180;
       dx = (qCos(radians)) *7;
       dy = (qSin(radians)) *7;
   }
}

void Bullet::setLimit(int amount)
{
   limit = amount;
}

void Bullet::setNobullets(int amount)
{
   nobullets = amount;
}

void Bullet::checkCollision()
{
    QCanvasItem* item;
    QCanvasItemList l=collisions(FALSE);
      for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
          item = *it;
          if ( (item->rtti()== 1500) && (item->collidesWith(this)) ) {
               Man* deadman = (Man*)item;
               if (deadman->frame() != 5) return;
               deadman->done();
	       emit score(10);
               setShotCount(shotcount+1);
               setAnimated(false);
               nobullets--;
               delete this;
               return;
          }
          else if ( (item->rtti()==1900) && (item->collidesWith(this)) ) {
               Helicopter* deadchopper = (Helicopter*) item;
               deadchopper->done();
	       emit score(50);
               setAnimated(false);
               nobullets--;
               delete this;
               return;
         }
      } 
      //check shot is not out of bounds
     if ( (y() < 0) || (x() < 0) ||
          (y() > canvas()->height()) ||
          ( x() > canvas()->width()))  {
          setAnimated(false);
          nobullets--;
          delete this;
          return;
     }
}

void Bullet::advance(int phase)
{
   QCanvasSprite::advance(phase);

   if (phase == 0) 
         checkCollision();

}

int Bullet::getShotCount()
{
   return shotcount;
}
 
void Bullet::setShotCount(int amount)
{
   shotcount = amount;
}

Bullet::~Bullet()
{

}

int Bullet::rtti() const
{
   return bullet_rtti;
}
