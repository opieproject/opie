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
#ifndef FREECELL_CARD_GAME_H 
#define FREECELL_CARD_GAME_H 


#include "patiencecardgame.h"


extern int numberOfFreeCells;


class FreecellDiscardPile : public PatienceDiscardPile
{
public:
    FreecellDiscardPile(int x, int y, QCanvas *canvas) :
    	PatienceDiscardPile(x, y, canvas) { }

};


class FreecellWorkingPile : public PatienceWorkingPile
{
public:
    FreecellWorkingPile(int x, int y, QCanvas *canvas) :
    	PatienceWorkingPile(x, y, canvas) { }

     virtual bool isAllowedOnTop(Card *card) {
	if ( cardOnBottom() == NULL ) {
	    int numberOfCardsBeingMoved = 0;
	    Card *tempCard = card;

	    while ((tempCard != NULL)) {
		numberOfCardsBeingMoved++;
		tempCard = cardInfront(tempCard); 
	    }
       
	    if (numberOfCardsBeingMoved > numberOfFreeCells)
		return FALSE;
	}

	if ( card->isFacing() &&
	      cardOnTop() == NULL )
	    return TRUE;
        return PatienceWorkingPile::isAllowedOnTop( card );	
    }
    
    virtual bool isAllowedToBeMoved(Card *card) {
	int nextExpectedValue = (int)card->getValue();
	bool nextExpectedColor = card->isRed();
	int numberOfCardsBeingMoved = 0;
	
	while ((card != NULL)) {
	    numberOfCardsBeingMoved++;
	    if ( (int)card->getValue() != nextExpectedValue )
		return FALSE;
	    if ( card->isRed() != nextExpectedColor )
		return FALSE;
	    nextExpectedValue--;;
	    nextExpectedColor = !nextExpectedColor;
	    card = cardInfront(card); 
	}
       
	if (numberOfCardsBeingMoved <= (numberOfFreeCells + 1))
	    return TRUE;
	
	return FALSE;
    }
    virtual void cardRemoved(Card *card) {
	if ( !isDealing() &&  !cardOnTop() )
	    numberOfFreeCells++;
	PatienceWorkingPile::cardRemoved( card );
    }
    virtual void cardAddedToTop(Card *card) {
	if ( !isDealing() && cardOnBottom() == card )
	    numberOfFreeCells--;
	PatienceWorkingPile::cardAddedToTop( card );
    }
};


class FreecellFreecellPile : public CardPile, public CanvasRoundRect
{
public:
    FreecellFreecellPile(int x, int y, QCanvas *canvas)
        : CardPile(x, y), CanvasRoundRect(x, y, canvas) { }
    virtual bool isAllowedOnTop(Card *card) {
	if ( ( cardOnTop() == NULL ) && ( card->getCardPile()->cardInfront(card) == NULL  ) )
	    return TRUE;
        return FALSE;	
    }
    virtual bool isAllowedToBeMoved(Card *card) {
	Q_UNUSED(card);
    	return TRUE;
    }
    virtual void cardAddedToTop(Card *card) {
	Q_UNUSED(card);
	numberOfFreeCells--;
    }
    virtual void cardRemoved(Card *card) {
	Q_UNUSED(card);
	numberOfFreeCells++;
    }
};


class FreecellCardGame : public CanvasCardGame
{
public:
    FreecellCardGame(QCanvas *c, bool snap, QWidget *parent = 0);
    virtual void deal(void);
    virtual bool haveWeWon() { 
	return ( discardPiles[0]->kingOnTop() &&
		 discardPiles[1]->kingOnTop() &&
		 discardPiles[2]->kingOnTop() &&
		 discardPiles[3]->kingOnTop() );
    }
    virtual void mousePress(QPoint p) { Q_UNUSED(p); }
    virtual void mouseRelease(QPoint p) { Q_UNUSED(p); }
//    virtual void mouseMove(QPoint p);
    virtual bool mousePressCard(Card *card, QPoint p);
    virtual void mouseReleaseCard(Card *card, QPoint p) { Q_UNUSED(card); Q_UNUSED(p); }
//    virtual void mouseMoveCard(Card *card, QPoint p) { Q_UNUSED(card); Q_UNUSED(p); }
    void readConfig( Config& cfg );
    void writeConfig( Config& cfg );
    bool snapOn;
private:
    FreecellFreecellPile *freecellPiles[8];
    FreecellWorkingPile *workingPiles[8];
    FreecellDiscardPile *discardPiles[4];
};


#endif

