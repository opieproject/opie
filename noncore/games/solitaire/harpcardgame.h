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
**  
** created on base of patiencecardgame by cam (C.A.Mader) 2002
** Rules for this game:
**      use 2 decks = 104 cards
**      deal 8 rows with one open card in the first place
**	one hidden and one open in the second place and so on
**      append red to black and vice versa
**      each card can be layed on a free place
**      deal 8 cards at once
**
**********************************************************************/
#ifndef HARP_CARD_GAME_H
#define HARP_CARD_GAME_H 


#include "patiencecardgame.h"


class HarpFaceDownDeck : public PatienceFaceDownDeck
{
public:
    HarpFaceDownDeck(int x, int y, QCanvas *canvas) :
        PatienceFaceDownDeck(x, y, canvas) { }

};


class HarpDiscardPile : public PatienceDiscardPile
{
public:
    HarpDiscardPile(int x, int y, QCanvas *canvas) :
        PatienceDiscardPile(x, y, canvas) { }

};


class HarpWorkingPile :  public PatienceWorkingPile 
{
public:
    HarpWorkingPile(int x, int y, QCanvas *canvas) :
	PatienceWorkingPile(x, y, canvas) { }

    virtual bool isAllowedOnTop(Card *card) {
	if ( card->isFacing() &&
//	     ( ( ( cardOnTop() == NULL ) && (card->getValue() == king) ) ||		// only kings are allowed on empty places
	     ( (cardOnTop() == NULL) ||							// aeach card can use an emply place
	       ( (cardOnTop() != NULL) &&
 	         ((int)card->getValue() + 1 == (int)cardOnTop()->getValue()) &&
	         (card->isRed() != cardOnTop()->isRed()) ) ) )
	    return TRUE;
        return FALSE;	
    }
    virtual bool isAllowedToBeMoved(Card *card) {
	if (!card->isFacing()) return FALSE;

        int nextExpectedValue = (int)card->getValue();
        bool nextExpectedColor = card->isRed();

        while ((card != NULL)) {
            if ( (int)card->getValue() != nextExpectedValue )
                return FALSE;
            if ( card->isRed() != nextExpectedColor )
                return FALSE;
            nextExpectedValue--;;
            nextExpectedColor = !nextExpectedColor;
            card = cardInfront(card);
        }
        return TRUE;
    }

    virtual void cardRemoved(Card *card) {
	Q_UNUSED(card);

	Card *newTopCard = cardOnTop();

	if ( !newTopCard ) {
	    top = QPoint( pileX, pileY );
	    setNextX( pileX );
    	    setNextY( pileY );
	    return;
	} else {
	    top = getCardPos(NULL);
	    if ( newTopCard->isFacing() == FALSE ) {
		int offsetDown = ( qt_screen->deviceWidth() < 200 ) ? 9 : 13;
		// correct the position taking in to account the card is not
    		// yet flipped, but will become flipped
    		top = QPoint( top.x(), top.y() - 3 );		// no moving to the side
		newTopCard->flipTo( top.x(), top.y() );
		top = QPoint( top.x(), top.y() + offsetDown );
	    }
    	    setNextX( top.x() );
	    setNextY( top.y() );
	}
    }
    virtual QPoint getCardPos(Card *c) {
	int x = pileX, y = pileY;
	Card *card = cardOnBottom();
	while ((card != c) && (card != NULL)) {
	    if (card->isFacing()) {
		int offsetDown = ( qt_screen->deviceWidth() < 200 ) ? 9 : 13;
		y += offsetDown; 
	    } else {
		x += 0;   					// no moving to the side 
		y += 3;
	    }
	    card = cardInfront(card); 
	}
	return QPoint( x, y );
    }

    virtual QPoint getHypertheticalNextCardPos(void) {
//        return top;
        return QPoint( getNextX(), getNextY() );
    }

private:
    QPoint top;

};


class HarpCardGame : public CanvasCardGame
{
public:
    HarpCardGame(QCanvas *c, bool snap, QWidget *parent = 0);
//    virtual ~HarpCardGame();
    virtual void deal(void);
    virtual bool haveWeWon() { 
	return ( discardPiles[0]->kingOnTop() &&
		 discardPiles[1]->kingOnTop() &&
		 discardPiles[2]->kingOnTop() &&
		 discardPiles[3]->kingOnTop() &&
		 discardPiles[4]->kingOnTop() &&
		 discardPiles[5]->kingOnTop() &&
		 discardPiles[6]->kingOnTop() &&
		 discardPiles[7]->kingOnTop() );;
    }
    virtual void mousePress(QPoint p);
    virtual void mouseRelease(QPoint p) { Q_UNUSED(p); }
//    virtual void mouseMove(QPoint p);
    virtual bool mousePressCard(Card *card, QPoint p);
    virtual void mouseReleaseCard(Card *card, QPoint p) { Q_UNUSED(card); Q_UNUSED(p); }
//    virtual void mouseMoveCard(Card *card, QPoint p) { Q_UNUSED(card); Q_UNUSED(p); }
    bool canTurnOverDeck(void) { return (FALSE); }
    void throughDeck(void) { }
    bool snapOn;
    void writeConfig( Config& cfg );
    void readConfig( Config& cfg );
private:
    HarpWorkingPile *workingPiles[8];
    HarpDiscardPile *discardPiles[8];
    HarpFaceDownDeck *faceDownDealingPile;
};


#endif

