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
#include "cardgamelayout.h"
#include "card.h"

CardGameLayout::~CardGameLayout()
{
    // Should I just do setAutoDelete( TRUE ); ?
    for (CardPile *p = first(); p != NULL; p = next())
	delete p;
}


CardPile *CardGameLayout::closestPile(int x, int y, int maxDistance)
{
    int closestDistance = maxDistance * maxDistance;
    CardPile *closestPile = NULL;
    
    for (CardPile *p = first(); p != NULL; p = next()) {
	int d = p->distanceFromNextPos(x, y);
	if (d < closestDistance) {
	    closestDistance = d;
	    closestPile = p;		
	}
    }
    
    return closestPile;
}


void CardGameLayout::beginDealing()
{
    for (CardPile *p = first(); p != NULL; p = next())
	p->beginDealing();
}


void CardGameLayout::endDealing()
{
    for (CardPile *p = first(); p != NULL; p = next())
	p->endDealing();
}


