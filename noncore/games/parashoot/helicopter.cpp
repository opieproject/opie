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

#include "helicopter.h"
#include "man.h"
#include "codes.h"

#include <qpe/resource.h>

#include <qregexp.h>

static QList<Helicopter> all;

Helicopter::Helicopter(QCanvas* canvas) :
    QCanvasSprite(0, canvas),
    chikachika("aland01")
{
    all.append(this);
    hits = 0;
    QCanvasPixmapArray* helicopterarray = new QCanvasPixmapArray();
    QString h0 = Resource::findPixmap("parashoot/helicopter0001");
    h0.replace(QRegExp("0001"),"%1");
    helicopterarray->readPixmaps(h0,3 );
    setSequence(helicopterarray);
    setAnimated(true);
    move(canvas->width(), 5);
    setVelocity(-2, 0);
    chikachika.playLoop();
    show();
}

Helicopter::~Helicopter()
{
    all.remove(this);
}

int fr = 0;

void Helicopter::advance(int phase)
{
   QCanvasSprite::advance(phase);
   if (phase == 0) {
          setFrame(fr%3);
          fr++;
          checkCollision();
   }
}

void Helicopter::checkCollision()
{
    if (x() == 6) {
        setAnimated(false); //setVelocity(0, 0);
        dropman();
    }
    if (x() < 0)
	done();
}

void Helicopter::dropman()
{
    (void)new Man(canvas(), 15, 25); 
    (void)new Man(canvas(), 35, 25);
    takeOff();
}

void Helicopter::done()
{
    hits++;
    if (hits >= 2) {
        setAnimated(false);
        delete this;
    }
}

void Helicopter::takeOff()
{
    setVelocity(-1, 0);
}

int Helicopter::rtti() const
{
    return helicopter_rtti;
}

void Helicopter::silenceAll()
{
    for (Helicopter* h = all.first(); h; h = all.next())
	h->chikachika.stop();
}

void Helicopter::deleteAll()
{
    Helicopter* h;
    while ((h = all.first()))
	delete h;
}

