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
#include "base.h"
#include "man.h"

#include <qpe/resource.h>

#include <qregexp.h>

int damage;

Base::Base(QCanvas* canvas) :
    QCanvasSprite(0, canvas),
    kaboom("landmine"),
    ohdear("crmble01")
{
   basearray = new QCanvasPixmapArray();
   QString b0 = Resource::findPixmap("parashoot/b0001");
   b0.replace(QRegExp("0001"),"%1");
   basearray->readPixmaps(b0, 4);
   setSequence(basearray);
   setFrame(0);
   move(2, canvas->height()-50);
   setZ(10);
   show();
   damage = 0;
}

void Base::damageBase()
{
   damage++;

   switch(damage) {
      case 1: setFrame(1); ohdear.play(); break;
      case 2: setFrame(2); ohdear.play(); break;
      case 3: setFrame(3); kaboom.play(); break;
   }
   show(); 
}

bool Base::baseDestroyed()
{
   return (damage >= 3);
}

Base::~Base()
{
}

int Base::rtti() const
{
   return base_rtti;
}
