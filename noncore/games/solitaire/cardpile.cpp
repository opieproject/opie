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

#include "cardpile.h"
#include "card.h"

#include <qpe/config.h>
#include <qpoint.h>

#include <qlist.h>


CardPile::CardPile(int x, int y) : pileX(x), pileY(y), dealing(FALSE) {
    pileWidth = 0;
    pileHeight = 0;
    pileNextX = pileX;
    pileNextY = pileY;
    pileCenterX = x + pileWidth / 2;
    pileCenterY = y + pileHeight / 2;
    pileRadius = (pileWidth > pileHeight) ? pileWidth : pileHeight;
}


int CardPile::distanceFromPile(int x, int y) {
    return (pileCenterX-x)*(pileCenterX-x)+(pileCenterY-y)*(pileCenterY-y);
}


int CardPile::distanceFromNextPos(int x, int y) {
    return (pileNextX-x)*(pileNextX-x)+(pileNextY-y)*(pileNextY-y);
}


Card *CardPile::cardInfront(Card *c) {
    CardPile *p = c->getCardPile();
    if (p) {
	p->at(p->find(c));
	return p->next();
    } else {
	return NULL;
    }
}


bool CardPile::kingOnTop() {
    Card *top = cardOnTop();
    return top && top->getValue() == king;
}


bool CardPile::addCardToTop(Card *c) {
    if (dealing || isAllowedOnTop(c)) {
	append((const Card *)c);
	cardAddedToTop(c);
	return TRUE;
    }
    return FALSE;
}


bool CardPile::addCardToBottom(Card *c) {
    if (dealing || isAllowedOnBottom(c)) {
	prepend((const Card *)c);
	cardAddedToBottom(c);
	return TRUE;
    }
    return FALSE;
}


bool CardPile::removeCard(Card *c) {
    if (dealing || isAllowedToBeMoved(c)) {
	take(find(c));
	cardRemoved(c);
	return TRUE;
    }
    return FALSE;
}


void CardPile::writeConfig( Config& cfg, QString name ) {
    int numberOfCards = 0;
    cfg.setGroup( name );
    Card *card = cardOnBottom();
    while ( card ) {
	QString cardStr;
	cardStr.sprintf( "%i", numberOfCards );
	int val  = (int)card->getValue()-1 + ((int)card->getSuit()-1)*13 + (int)card->getDeckNumber()*52;
	cfg.writeEntry( "Card" + cardStr, val );
	cfg.writeEntry( "CardFacing" + cardStr, card->isFacing() );
	card = cardInfront( card );
	numberOfCards++;
    }
    cfg.writeEntry("NumberOfCards", numberOfCards);
}


