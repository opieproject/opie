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
#include <stdlib.h>
#include <time.h>
#include "card.h"
#include "carddeck.h"


CardDeck::CardDeck(int jokers) : numberOfJokers(jokers), deckCreated(FALSE)
{
    cards = new (Card *)[getNumberOfCards()];
}


CardDeck::~CardDeck()
{
    for (int i = 0; i < getNumberOfCards(); i++)
	delete cards[i];
    delete cards;
}


void CardDeck::createDeck()
{
    if (!deckCreated) {
        for (int i = 0; i < 52; i++)
	    cards[i] = newCard( (eValue)((i % 13) + 1), (eSuit)((i / 13) + 1), FALSE );
        for (int i = 0; i < getNumberOfJokers(); i++)
	    cards[52 + i] = newCard( jokerVal, jokerSuit, FALSE );
        deckCreated = TRUE;
    }
}


void CardDeck::shuffle()
{
    srand(time(NULL));
    for (int i = 0; i < getNumberOfCards(); i++) {
	int index = rand() % getNumberOfCards();
	Card *tmpCard = cards[i];
	cards[i] = cards[index];
	cards[index] = tmpCard;
    }
}


int CardDeck::getNumberOfCards()
{
    return 52 + getNumberOfJokers();
}


int CardDeck::getNumberOfJokers()
{
    return numberOfJokers;
}
    

Card *CardDeck::newCard( eValue v, eSuit s, bool f )
{
    return new Card(v, s, f);
}


