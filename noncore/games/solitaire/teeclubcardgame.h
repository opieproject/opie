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
**      deal 9 rows with 5 open cards each
**      append one card to each other card which is one step higher
**	move only columns of cards which are equal in suit
**      each card can be layed on a free place
**      deal 1 card at once on the first pile
**
**
**********************************************************************/
#ifndef TEECLUB_CARD_GAME_H
#define TEECLUB_CARD_GAME_H 


#include "patiencecardgame.h"


class TeeclubFaceDownDeck : public PatienceFaceDownDeck
{
public:
    TeeclubFaceDownDeck(int x, int y, QCanvas *canvas) :
        PatienceFaceDownDeck(x, y, canvas) { }

};


class TeeclubDiscardPile : public CardPile, public CanvasRoundRect
{
public:
    TeeclubDiscardPile(int x, int y, QCanvas *canvas) 
        : CardPile(x, y), CanvasRoundRect(x, y, canvas) { }
    virtual bool isAllowedOnTop(Card *card) {
        if ( card->isFacing() && ( card->getCardPile()->cardInfront(card) == NULL ) &&
           ( ( ( cardOnTop() == NULL ) && ( card->getValue() == ace ) ) ||
             ( ( cardOnTop() != NULL ) &&
               ( (int)card->getValue() == (int)cardOnTop()->getValue() + 1 ) &&
               ( card->getSuit() == cardOnTop()->getSuit() ) ) ) )
            return TRUE;
        return FALSE;
    }
    virtual bool isAllowedToBeMoved(Card *card) {
	if (card->isFacing()) return FALSE;
        return FALSE;									// the deads are sleeping forever
    }
};


class TeeclubWorkingPile :  public PatienceWorkingPile 
{
public:
    TeeclubWorkingPile(int x, int y, QCanvas *canvas) :
	PatienceWorkingPile(x, y, canvas) { }

    virtual bool isAllowedOnTop(Card *card) {
	if ( card->isFacing() &&
//	     ( ( ( cardOnTop() == NULL ) && (card->getValue() == king) ) ||		// use this if only Kings are allowed on empty places
	     ( (cardOnTop() == NULL) ||							// each card can use an empty place 
	       ( (cardOnTop() != NULL) &&
 	         ((int)card->getValue() + 1 == (int)cardOnTop()->getValue()) 		// you can append every color on every color
	     ) ) )
	    return TRUE;
        return FALSE;	
    }

    virtual bool isAllowedToBeMoved(Card *card) {
	if (!card->isFacing()) return FALSE;

        int nextExpectedValue = (int)card->getValue();
        eSuit nextExpectedSuit = card->getSuit();

        while ((card != NULL)) {
            if ( (int)card->getValue() != nextExpectedValue )
                return FALSE;
            if ( card->getSuit() != nextExpectedSuit )
                return FALSE;
            nextExpectedValue--;;
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
            setOffsetDown(13);
	    return;
	} else {
	    top = getCardPos(NULL);
	    if ( newTopCard->isFacing() == FALSE ) {
		int offsetDown = newTopCard->getCardPile()->getOffsetDown();
		// correct the position taking in to account the card is not
    		// yet flipped, but will become flipped
    		top = QPoint( top.x(), top.y() - 3 );		// no moving to the side
		newTopCard->flipTo( top.x(), top.y() );
		top = QPoint( top.x(), top.y() + offsetDown );
	    }
    	    setNextX( top.x() );
	    setNextY( top.y() );
	}

	if ((getCardPos(NULL).y() < 230) && (getOffsetDown()<13)) {
	    // resize the pile
            beginDealing();
            setOffsetDown(getOffsetDown()+1);
	    Card *card = cardOnBottom();
	    int p=0;
	    while (card != NULL) {
		card->setPos( 0, 0, p++ );
		card->move( getCardPos( card ) );
		card = cardInfront(card);
	    }
            endDealing();
	}
    }

    virtual QPoint getCardPos(Card *c) {
	int x = pileX, y = pileY;
	Card *card = cardOnBottom();
	while ((card != c) && (card != NULL)) {
	    if (card->isFacing()) {
		int offsetDown = card->getCardPile()->getOffsetDown(); 			   
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
        return QPoint( getNextX(), getNextY() );
    }

    virtual void cardAddedToTop(Card *c) { 
	Q_UNUSED(c);
        setNextX( getCardPos(NULL).x() );
        setNextY( getCardPos(NULL).y() );

	while (isPileResize() && (getCardPos(NULL).y() > 230) && (getOffsetDown()>1)) {
	    // resize the pile
            beginDealing();
            setOffsetDown(getOffsetDown()-1);
	    Card *card = cardOnBottom();
	    int p=0;
	    while (card != NULL) {
		card->setPos( 0, 0, p++ );
		card->move( getCardPos( card ) );
		card = cardInfront(card);
	    }
            endDealing();
	}

    }




private:
    QPoint top;

};


class TeeclubCardGame : public CanvasCardGame
{
public:
    TeeclubCardGame(QCanvas *c, bool snap, QWidget *parent = 0);
//    virtual ~TeeclubCardGame();
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
    void resizePiles();
private:
    TeeclubWorkingPile *workingPiles[9];
    TeeclubDiscardPile *discardPiles[8];
    TeeclubFaceDownDeck *faceDownDealingPile;
};


#endif

