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
#include "man.h"
#include "base.h"

#include <qpe/resource.h>

#include <qregexp.h>

int mancount;

Man::Man(QCanvas* canvas) :
    QCanvasSprite(0, canvas),
    splat("lose")
{
    manarray = new QCanvasPixmapArray();
    QString m0 = Resource::findPixmap("parashoot/man0001");
    m0.replace(QRegExp("0001"),"%1");
    manarray->readPixmaps(m0, 7);  
    setSequence(manarray);
    setAnimated(true);
    mancount++;
    dead = false;
    start();
}

Man::Man(QCanvas* canvas, int x, int y) :
    QCanvasSprite(0, canvas),
    splat("bang")
{
    manarray = new QCanvasPixmapArray();
    QString m0 = Resource::findPixmap("parashoot/man0001");
    m0.replace(QString("0001"),"%1");
    manarray->readPixmaps(m0, 7);
    setSequence(manarray);
    move(x, y);
    setFrame(5);
    setZ(300);
    show();

    static bool first_time = TRUE;
    if (first_time) {
        first_time = FALSE;
        QTime midnight(0, 0, 0);
        srand(midnight.secsTo(QTime::currentTime()) );
    } 
    int yfallspeed = 0;
    yfallspeed = (rand() % 3) + 1;
    setVelocity(0, yfallspeed);

    mancount++;
    dead = false;
}
int f = 0;

void Man::advance(int phase)
{
    QCanvasSprite::advance(phase);
    if (phase == 0) {
	checkCollision();
	if (dead) {
	    if (count < 10) {
		setFrame(6);
		setVelocity(0,0);
		count++;
	    } else {
		delete this;
		return;
	    }
	}
	if (y() > canvas()->height()-43) {
	    setFrame(f%5);
	    f++;
	    move(x(), canvas()->height()-26);
	    setVelocity(-2, 0);
	} 
    }
} 

void Man::setInitialCoords()
{
    static bool first_time = TRUE;
    if (first_time) {
        first_time = FALSE;
        QTime midnight(0, 0, 0);
        srand(midnight.secsTo(QTime::currentTime()) );
    }
    dx = rand() % (canvas()->width()-16);
    dy = -43;  //height of a man off the screen
}

//check if man has reached the base
void Man::checkCollision()
{ 
    if ( (x() < 23) && (y() == canvas()->height()-26)) {
       QCanvasItem* item;
       QCanvasItemList l=collisions(FALSE);
          for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
             item = *it;
             if ( (item->rtti()== 1800) && (item->collidesWith(this)) ) {
                 Base* base = (Base*) item;
                 base->damageBase();
                 start();
             }
          }
    }
}

void Man::start()
{
   setInitialCoords();
   move(dx, dy);
   setFrame(5);
   setZ(300);
   show();

   static bool first_time = TRUE;
   if (first_time) {
      first_time = FALSE;
      QTime midnight(0, 0, 0);
      srand(midnight.secsTo(QTime::currentTime()) );
   }
   int yfallspeed = 0;
   yfallspeed = (rand() % 3) + 1;
   setVelocity(0, yfallspeed);
}

void Man::done()
{
   splat.play();
   count = 0;
   dead = true;
   setFrame(6);
}

int Man::getManCount()
{
   return mancount;
}

void Man::setManCount(int count)
{
    mancount = count;
}


int Man::rtti() const
{
   return man_rtti;
}

Man::~Man()
{
   mancount--;
}
