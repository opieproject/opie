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

#include <qpe/resource.h>

#include <qregexp.h>

#include "codes.h"  
#include "cannon.h"

Cannon::Cannon(QCanvas* canvas) :
        QCanvasSprite(0, canvas)
{
shotsfired=0;
    index = 8;
    cannonx = 0;
    cannony = 0;
    cannonarray = new QCanvasPixmapArray();
    QString c0 = Resource::findPixmap("parashoot/can0001");
    c0.replace(QRegExp("0001"),"%1");
    cannonarray->readPixmaps(c0,17);
    setSequence(cannonarray);
    setFrame(index);
    move(canvas->width()/2-20, canvas->height()-32);
    // co ords for barrel of cannon when upright
    barrelypos = canvas->height()-32;
    barrelxpos = canvas->width()/2;
    movedir = NoDir;
    moveDelay = 0;
    setAnimated(TRUE);
    show();
}

void Cannon::advance(int stage)
{
    if ( stage == 1  && moveDelay-- == 0 ) {
	if (movedir == Left) {
	    if (index > 0) {
		setFrame(index-1);
		index--;
	    }
	}
	if (movedir == Right) {
	    if (index < 16) {
		setFrame(index+1);
		index++;
	    }
	}
	moveDelay = 0;
    }
}

void Cannon::pointCannon(Direction dir)
{
    movedir = dir;
    moveDelay = 0;
    advance(1);
    moveDelay = 1;
}

void Cannon::setCoords()
{
   switch(index) {
      case 0: cannonx = barrelxpos-29;  cannony = barrelypos-8; break;
      case 1: cannonx = barrelxpos-27;  cannony = barrelypos-8; break;
      case 2: cannonx = barrelxpos-25;  cannony = barrelypos-6; break;
      case 3: cannonx = barrelxpos-23;  cannony = barrelypos-4; break;
      case 4: cannonx = barrelxpos-21;  cannony = barrelypos-2; break;
      case 5: cannonx = barrelxpos-19;  cannony = barrelypos; break;
      case 6: cannonx = barrelxpos-15;  cannony = barrelypos; break;
      case 7: cannonx = barrelxpos-10;  cannony = barrelypos; break;
      case 8: cannonx = barrelxpos;     cannony = barrelypos; break;
      case 9: cannonx = barrelxpos+2;   cannony = barrelypos; break;
      case 10: cannonx = barrelxpos+6;  cannony = barrelypos; break;
      case 11: cannonx = barrelxpos+8;  cannony = barrelypos; break;
      case 12: cannonx = barrelxpos+12; cannony = barrelypos-2; break;
      case 13: cannonx = barrelxpos+18; cannony = barrelypos-4; break;
      case 14: cannonx = barrelxpos+22; cannony = barrelypos-6; break;
      case 15: cannonx = barrelxpos+26; cannony = barrelypos-8; break;
      case 16: cannonx = barrelxpos+28; cannony = barrelypos-8; break;
   }
}

double Cannon::shootAngle()
{
   switch(index) {
       case 0: return 30.0;
       case 1: return 37.5;
       case 2: return 45.0;
       case 3: return 52.5;
       case 4: return 60.0;
       case 5: return 67.5;
       case 6: return 75.0;
       case 7: return 82.5;
       case 8: return 90.0;
       case 9: return 97.5;
       case 10: return 105.0;
       case 11: return 112.5;
       case 12: return 120.0;
       case 13: return 127.5;
       case 14: return 135.0;
       case 15: return 142.5;
       case 16: return 150.0;
    }
    return 0;
}

void Cannon::shoot()
{
    setCoords();
    Bullet* bullet = new Bullet(canvas(), shootAngle(), cannonx, cannony); 
    connect(bullet, SIGNAL(score(int)), this, SIGNAL(score(int)));
	shotsfired++;
}

Cannon::~Cannon()
{
}

int Cannon::rtti() const
{
   return cannon_rtti;
}
