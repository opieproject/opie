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
** Modified by C.A.Mader 2002
**
**********************************************************************/
#include <qgfx_qws.h>
#include "chicanecardgame.h"


extern int highestZ;


ChicaneCardGame::ChicaneCardGame(QCanvas *c, bool snap, QWidget *parent) : CanvasCardGame(*c, snap, parent, 2)	// Use 2 Decks
{
    highestZ = 0;

    for (int i = 0; i < 8; i++) {
        discardPiles[i] = new ChicaneDiscardPile( 27 + i * 26, 10, canvas() );
        addCardPile(discardPiles[i]);
    }
    for (int i = 0; i < 8; i++) {
        workingPiles[i] = new ChicaneWorkingPile( 27 + i * 26, 50, canvas() );
        addCardPile(workingPiles[i]);
    }
    faceDownDealingPile = new ChicaneFaceDownDeck( 2, 10, canvas() );
}


void ChicaneCardGame::deal(void)
{
    highestZ = 1;
    int t = 0;

    beginDealing();

    for (int i = 0; i < 8; i++) {
	for (int k = 0; k < 4; k++, t++) {
	    Card *card = cards[t];
	    workingPiles[i]->addCardToTop(card);
	    card->setCardPile( workingPiles[i] );
	    card->setPos( 0, 0, highestZ );
	    card->setFace(k==3);
	    card->move( workingPiles[i]->getCardPos( card ) );
	    card->showCard();
	    highestZ++;
	}
    }
    
    for ( ; t < getNumberOfCards(); t++) {
	Card *card = cards[t];
	faceDownDealingPile->addCardToTop(card);
	card->setCardPile( faceDownDealingPile );
	QPoint p = faceDownDealingPile->getCardPos( card );
	card->setPos( p.x(), p.y(), highestZ );
	card->showCard();
	highestZ++;
    }

    endDealing();
}


void ChicaneCardGame::readConfig( Config& cfg )
{
    cfg.setGroup("GameState");

    // Create Cards, but don't shuffle or deal them yet
    createDeck();

    // Move the cards to their piles (deal them to their previous places)
    beginDealing();

    highestZ = 1;

    for (int i = 0; i < 8; i++) {
        QString pile;
        pile.sprintf( "ChicaneDiscardPile%i", i );
        readPile( cfg, discardPiles[i], pile, highestZ );
    }

    for (int i = 0; i < 8; i++) {
        QString pile;
        pile.sprintf( "ChicaneWorkingPile%i", i );
        readPile( cfg, workingPiles[i], pile, highestZ );
    }

    readPile( cfg, faceDownDealingPile, "ChicaneFaceDownDealingPile", highestZ );

    highestZ++;

    endDealing();
}


void ChicaneCardGame::writeConfig( Config& cfg )
{
    cfg.setGroup("GameState");
    for ( int i = 0; i < 8; i++ ) {
	QString pile;
	pile.sprintf( "ChicaneDiscardPile%i", i );
	discardPiles[i]->writeConfig( cfg, pile );
    }
    for ( int i = 0; i < 8; i++ ) {
	QString pile;
	pile.sprintf( "ChicaneWorkingPile%i", i );
	workingPiles[i]->writeConfig( cfg, pile );
    }
    faceDownDealingPile->writeConfig( cfg, "ChicaneFaceDownDealingPile" );
}


bool ChicaneCardGame::mousePressCard( Card *card, QPoint p )
{
    Q_UNUSED(p);

    CanvasCard *item = (CanvasCard *)card;
    if (item->isFacing() != TRUE) {
	// From facedown stack
	if ((item->x() == 2) && ((int)item->y() == 10)) {		// Deal a row of 8 cards
	    // Move 8 cards, one to each workingPile
	    beginDealing();
	    for (int i=0; i<8 && faceDownDealingPile->cardOnTop(); i++) {
		CanvasCard *card =  (CanvasCard *)faceDownDealingPile->cardOnTop(); 
	        card->setZ(highestZ);
	        highestZ++;
		faceDownDealingPile->removeCard(card);
		workingPiles[i]->addCardToTop(card);
		card->setCardPile( workingPiles[i] );
		card->setFace(FALSE);
		QPoint p = workingPiles[i]->getCardPos(card);
		card->flipTo( p.x(), p.y() );
	    }
	    endDealing();
	}
	moving = NULL;
	moved = FALSE;

	return TRUE;
    } else if ( !card->getCardPile()->isAllowedToBeMoved(card) ) {	// Don't allow unclean columns to be moved
       	moving = NULL;
        return TRUE;
    }

    return FALSE;
}



void ChicaneCardGame::mousePress(QPoint p)
{
    Q_UNUSED(p);
}


