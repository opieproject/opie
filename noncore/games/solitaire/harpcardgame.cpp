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
**      one hidden and one open in the second place and so on
**      append red to black and vice versa
**      each card can be layed on a free place
**      deal 8 cards at once
**
**
**********************************************************************/
#include <qgfx_qws.h>
#include "harpcardgame.h"


extern int highestZ;


HarpCardGame::HarpCardGame(QCanvas *c, bool snap, QWidget *parent) : CanvasCardGame(*c, snap, parent, 2)	// Use 2 Decks
{
    highestZ = 0;

    for (int i = 0; i < 8; i++) {
        discardPiles[i] = new HarpDiscardPile( 27 + i * 26, 10, canvas() );
        addCardPile(discardPiles[i]);
    }
    for (int i = 0; i < 8; i++) {
        workingPiles[i] = new HarpWorkingPile( 27 + i * 26, 50, canvas() );
        addCardPile(workingPiles[i]);
    }
    faceDownDealingPile = new HarpFaceDownDeck( 2, 10, canvas() );
}


void HarpCardGame::deal(void)
{
    highestZ = 1;
    int t = 0;

    beginDealing();

    for (int i = 0; i < 8; i++) {
	for (int k = 0; k < i+1; k++, t++) {
	    Card *card = cards[t];
	    workingPiles[i]->addCardToTop(card);
	    card->setCardPile( workingPiles[i] );
	    card->setPos( 0, 0, highestZ );
	    card->setFace(k==i);
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


void HarpCardGame::readConfig( Config& cfg )
{
    cfg.setGroup("GameState");

    // Create Cards, but don't shuffle or deal them yet
    createDeck();

    // Move the cards to their piles (deal them to their previous places)
    beginDealing();

    highestZ = 1;

    for (int i = 0; i < 8; i++) {
        QString pile;
        pile.sprintf( "HarpDiscardPile%i", i );
        readPile( cfg, discardPiles[i], pile, highestZ );
    }

    for (int i = 0; i < 8; i++) {
        QString pile;
        pile.sprintf( "HarpWorkingPile%i", i );
        readPile( cfg, workingPiles[i], pile, highestZ );
    }

    readPile( cfg, faceDownDealingPile, "HarpFaceDownDealingPile", highestZ );

    highestZ++;

    endDealing();
}


void HarpCardGame::writeConfig( Config& cfg )
{
    cfg.setGroup("GameState");
    for ( int i = 0; i < 8; i++ ) {
	QString pile;
	pile.sprintf( "HarpDiscardPile%i", i );
	discardPiles[i]->writeConfig( cfg, pile );
    }
    for ( int i = 0; i < 8; i++ ) {
	QString pile;
	pile.sprintf( "HarpWorkingPile%i", i );
	workingPiles[i]->writeConfig( cfg, pile );
    }
    faceDownDealingPile->writeConfig( cfg, "HarpFaceDownDealingPile" );
}


bool HarpCardGame::mousePressCard( Card *card, QPoint p )
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



void HarpCardGame::mousePress(QPoint p)
{
    Q_UNUSED(p);
}


