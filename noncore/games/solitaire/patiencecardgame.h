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
#ifndef PATIENCE_CARD_GAME_H
#define PATIENCE_CARD_GAME_H 


#include <qpopupmenu.h>
#include <qmainwindow.h>
#include <qintdict.h>
#include <qcanvas.h>
#include "canvascardgame.h"
#include "canvascardwindow.h"


class PatienceFaceDownDeck : public CardPile, public CanvasRoundRect
{
public:
    PatienceFaceDownDeck(int x, int y, QCanvas *canvas)
        : CardPile(x, y), CanvasRoundRect(x, y, canvas) { }
    virtual bool isAllowedOnTop(Card *card) {
	Q_UNUSED(card);
	// Need to check it is from the faceUpDealingPile
	return TRUE;
    }
    virtual bool isAllowedToBeMoved(Card *card) {
	Q_UNUSED(card);
	//if ( ( !card->isFacing() ) && ( card == cardOnTop() ) )
	if ( card == cardOnTop() )
	    return TRUE;
        return FALSE;	
    }
};


class PatienceFaceUpDeck : public CardPile, public CanvasRoundRect
{
public:
    PatienceFaceUpDeck(int x, int y, QCanvas *canvas)
        : CardPile(x, y), CanvasRoundRect(x, y, canvas) { }
    virtual bool isAllowedOnTop(Card *card) {
	Q_UNUSED(card);
	// Need to check it is from the faceDownDealingPile
	return TRUE;
    }
    virtual bool isAllowedToBeMoved(Card *card) {
	Q_UNUSED(card);
	//if ( ( card->isFacing() ) && ( card == cardOnTop() ) )
	if ( card == cardOnTop() )
	    return TRUE;
        return FALSE;	
    }
};


class PatienceDiscardPile : public CardPile, public CanvasRoundRect
{
public:
    PatienceDiscardPile(int x, int y, QCanvas *canvas)
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
	if ( card->isFacing() && ( card == cardOnTop() ) )
	    return TRUE;
        return FALSE;	
    }
};


class PatienceWorkingPile : public CardPile, public CanvasRoundRect
{
public:
    PatienceWorkingPile(int x, int y, QCanvas *canvas)
        : CardPile(x, y), CanvasRoundRect(x, y, canvas), top(x, y) { }
    virtual bool isAllowedOnTop(Card *card) {
	if ( card->isFacing() &&
	     ( ( ( cardOnTop() == NULL ) && (card->getValue() == king) ) ||
	       ( ( cardOnTop() != NULL ) &&
 	         ( (int)card->getValue() + 1 == (int)cardOnTop()->getValue() ) &&
	         ( card->isRed() != cardOnTop()->isRed() ) ) ) )
	    return TRUE;
        return FALSE;	
    }
    virtual bool isAllowedToBeMoved(Card *card) {
	if ( card->isFacing() )
	    return TRUE;
        return FALSE;	
    }
    virtual void cardAddedToTop(Card *card) {
	Q_UNUSED(card);
	top = getCardPos(NULL);
	setNextX( top.x() );
	setNextY( top.y() );
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
		int offsetDown = ( smallFlag ) ? 9 : 13;
		// correct the position taking in to account the card is not
    		// yet flipped, but will become flipped
    		top = QPoint( top.x() - 1, top.y() - 3 );
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
		int offsetDown = ( smallFlag ) ? 9 : 13;
		y += offsetDown; 
	    } else {
		x += 1;    
		y += 3;
	    }
	    card = cardInfront(card); 
	}
	return QPoint( x, y );
    }
    virtual QPoint getHypertheticalNextCardPos(void) {
	return top;
	// return QPoint( getNextX(), getNextY() );
    }
private:
    QPoint top;

};


class PatienceCardGame : public CanvasCardGame
{
public:
    PatienceCardGame(QCanvas *c, bool snap, QWidget *parent = 0);
    virtual ~PatienceCardGame();
    virtual void deal(void);
    virtual bool haveWeWon() { 
	return ( discardPiles[0]->kingOnTop() &&
		 discardPiles[1]->kingOnTop() &&
		 discardPiles[2]->kingOnTop() &&
		 discardPiles[3]->kingOnTop() );;
    }
    virtual void mousePress(QPoint p);
    virtual void mouseRelease(QPoint p) { Q_UNUSED(p); }
//    virtual void mouseMove(QPoint p);
    virtual bool mousePressCard(Card *card, QPoint p);
    virtual void mouseReleaseCard(Card *card, QPoint p) { Q_UNUSED(card); Q_UNUSED(p); }
//    virtual void mouseMoveCard(Card *card, QPoint p) { Q_UNUSED(card); Q_UNUSED(p); }
    bool canTurnOverDeck(void) { return (numberOfTimesThroughDeck != 3); }
    void throughDeck(void) {
	numberOfTimesThroughDeck++;
	if (numberOfTimesThroughDeck == 3)
    	    circleCross->setCross();
    }
    bool snapOn;
    virtual void writeConfig( Config& cfg );
    virtual void readConfig( Config& cfg );
private:
    CanvasCircleOrCross *circleCross;
    CanvasRoundRect *rectangle;
    PatienceWorkingPile *workingPiles[7];
    PatienceDiscardPile *discardPiles[4];
    PatienceFaceDownDeck *faceDownDealingPile;
    PatienceFaceUpDeck *faceUpDealingPile;
    int numberOfTimesThroughDeck;
};


#endif

