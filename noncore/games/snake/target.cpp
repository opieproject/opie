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

#include "target.h"
#include "codes.h"

#include <qpe/resource.h>

#include <stdlib.h>

Target::Target(QCanvas* canvas) 
       : QCanvasSprite(0, canvas)
{
   mouse = new QCanvasPixmapArray(Resource::findPixmap("snake/mouse"));
   setSequence(mouse);
   newTarget();
}

void Target::newTarget()
{
   static bool first_time = TRUE;
   if (first_time) {
         first_time = FALSE;
         QTime midnight(0, 0, 0);
         srand(midnight.secsTo(QTime::currentTime()) );
    }
   do {
     int x = rand() % (canvas()->width()-10);
     x = x - (x % 16) + 2;
     int y = rand() % (canvas()->height()-10);
     y = y - (y % 16) + 2;
     move(x, y);
   } while (onTop());  
   show();
}

bool Target::onTop()
{
   QCanvasItem* item;
   QCanvasItemList l= canvas()->allItems(); //collisions(FALSE);
   for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
          item = *it; 
          if (item != this && item->collidesWith(this)) return true;
      }
   return false;
}

void Target::done()
{
   delete this;
}

int Target::rtti() const
{
   return target_rtti;
}

Target::~Target()
{
}
