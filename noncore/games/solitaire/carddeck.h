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
#ifndef CARD_DECK_H
#define CARD_DECK_H


class Card;


class CardDeck
{
public:
    CardDeck(int jokers = 0);
    virtual ~CardDeck();

    void createDeck();
    void shuffle();
    int getNumberOfCards();
    int getNumberOfJokers();
    
    virtual Card *newCard( eValue v, eSuit s, bool f );
    virtual void deal() { }

    Card **cards;
private:
    int numberOfJokers;
    bool deckCreated;
};


#endif

