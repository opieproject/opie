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
#include <qgfx_qws.h>
#include "patiencecardgame.h"


int highestZ = 0;


PatienceCardGame::PatienceCardGame(QCanvas *c, bool snap, QWidget *parent) : CanvasCardGame(*c, snap, parent)
{
    numberOfTimesThroughDeck = 0;
    highestZ = 0;

    if ( qt_screen->deviceWidth() < 200 ) {
	circleCross = new CanvasCircleOrCross( 7, 16, canvas() );
	rectangle = new CanvasRoundRect(  30, 10, canvas() );

	for (int i = 0; i < 4; i++) {
	    discardPiles[i] = new PatienceDiscardPile( 78 + i * 23, 10, canvas() );
	    addCardPile(discardPiles[i]);
	}
	for (int i = 0; i < 7; i++) {
	    workingPiles[i] = new PatienceWorkingPile( 5 + i * 23, 50, canvas() );
	    addCardPile(workingPiles[i]);
	}
	faceDownDealingPile = new PatienceFaceDownDeck( 5, 10, canvas() );
	faceUpDealingPile   = new PatienceFaceUpDeck( 30, 10, canvas() );
    } else {
	circleCross = new CanvasCircleOrCross( 7, 18, canvas() );
	rectangle = new CanvasRoundRect(  35, 10, canvas() );

	for (int i = 0; i < 4; i++) {
	    discardPiles[i] = new PatienceDiscardPile( 110 + i * 30, 10, canvas() );
	    addCardPile(discardPiles[i]);
	}
	for (int i = 0; i < 7; i++) {
	    workingPiles[i] = new PatienceWorkingPile( 10 + i * 30, 50, canvas() );
	    addCardPile(workingPiles[i]);
	}
	faceDownDealingPile = new PatienceFaceDownDeck( 5, 10, canvas() );
	faceUpDealingPile   = new PatienceFaceUpDeck( 35, 10, canvas() );
    }
}


PatienceCardGame::~PatienceCardGame()
{
    delete circleCross;
    delete rectangle;
    delete faceDownDealingPile;
    delete faceUpDealingPile;
}


void PatienceCardGame::deal(void)
{
    highestZ = 1;
    int t = 0;

    beginDealing();

    for (int i = 0; i < 7; i++) {
	cards[t]->setFace(TRUE);
	for (int k = i; k < 7; k++, t++) {
	    Card *card = cards[t];
	    workingPiles[k]->addCardToTop(card);
	    card->setCardPile( workingPiles[k] );
	    QPoint p = workingPiles[k]->getCardPos( card );
	    card->setPos( p.x(), p.y(), highestZ );
	    card->showCard();
	    highestZ++;
	}
    }
    
    for ( ; t < 52; t++) {
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


void PatienceCardGame::readConfig( Config& cfg )
{
    cfg.setGroup("GameState");

    // Do we have a config file to read in?
    if ( !cfg.hasKey("numberOfTimesThroughDeck") ) {
	// if not, create a new game
	newGame();
	return;
    }
    // We have a config file, lets read it in and use it

    // Create Cards, but don't shuffle or deal them yet
    createDeck();

    // How many times through the deck have we been
    numberOfTimesThroughDeck = cfg.readNumEntry("NumberOfTimesThroughDeck");

    // restore state to the circle/cross under the dealing pile
    if ( canTurnOverDeck() )
	circleCross->setCircle();
    else
	circleCross->setCross();

    // Move the cards to their piles (deal them to their previous places)
    beginDealing();

    highestZ = 1;

    for (int k = 0; k < 7; k++) {
	QString pile;
	pile.sprintf( "WorkingPile%i", k );
	readPile( cfg, workingPiles[k], pile, highestZ );
    }

    for (int k = 0; k < 4; k++) {
	QString pile;
	pile.sprintf( "DiscardPile%i", k );
	readPile( cfg, discardPiles[k], pile, highestZ );
    }

    readPile( cfg, faceDownDealingPile, "FaceDownDealingPile", highestZ );
    readPile( cfg, faceUpDealingPile,   "FaceUpDealingPile", highestZ );

    highestZ++;

    endDealing();
}


void PatienceCardGame::writeConfig( Config& cfg )
{
    cfg.setGroup("GameState");
    cfg.writeEntry("numberOfTimesThroughDeck", numberOfTimesThroughDeck);

    for ( int i = 0; i < 7; i++ ) {
	QString pile;
	pile.sprintf( "WorkingPile%i", i );
	workingPiles[i]->writeConfig( cfg, pile );
    }
    for ( int i = 0; i < 4; i++ ) {
	QString pile;
	pile.sprintf( "DiscardPile%i", i );
	discardPiles[i]->writeConfig( cfg, pile );
    }
    faceDownDealingPile->writeConfig( cfg, "FaceDownDealingPile" );
    faceUpDealingPile->writeConfig( cfg, "FaceUpDealingPile" );
}


bool PatienceCardGame::mousePressCard( Card *card, QPoint p )
{
    Q_UNUSED(p);

    CanvasCard *item = (CanvasCard *)card;
    if (item->isFacing() != TRUE) {
	// From facedown stack
	if ((item->x() == 5) && ((int)item->y() == 10)) {
	    item->setZ(highestZ);
	    highestZ++;

	    // Added Code
	    faceDownDealingPile->removeCard(item);
	    faceUpDealingPile->addCardToTop(item);
	    item->setCardPile( faceUpDealingPile );

	    if ( qt_screen->deviceWidth() < 200 )
		item->flipTo( 30, (int)item->y() );
	    else
		item->flipTo( 35, (int)item->y() );
	}
	moving = NULL;
	moved = FALSE;

	// move two other cards if we flip three at a time
	int flipped = 1;
	QCanvasItemList l = canvas()->collisions( p );
	for (QCanvasItemList::Iterator it = l.begin(); (it != l.end()) && (flipped != cardsDrawn()); ++it) {
	    if ( (*it)->rtti() == canvasCardId ) {
		CanvasCard *item = (CanvasCard *)*it;
		if (item->animated())
		    continue;
		item->setZ(highestZ);
		highestZ++;
		flipped++;

		// Added Code
		faceDownDealingPile->removeCard(item);
		faceUpDealingPile->addCardToTop(item);
		item->setCardPile( faceUpDealingPile );

		if ( qt_screen->deviceWidth() < 200 )
		    item->flipTo( 30, (int)item->y(), 8 * flipped );
		else
		    item->flipTo( 35, (int)item->y(), 8 * flipped );
	    }
	}
	
	return TRUE;
    }

    return FALSE;
}


void PatienceCardGame::mousePress(QPoint p)
{
    if ( canTurnOverDeck() && 
         (p.x() >  5) && (p.x() < 28) &&
	 (p.y() > 10) && (p.y() < 46) ) {

	beginDealing();
	Card *card = faceUpDealingPile->cardOnTop();
	while ( card ) {
	    card->setPos( 5, 10, highestZ );
	    card->setFace( FALSE );
	    faceUpDealingPile->removeCard( card );
	    faceDownDealingPile->addCardToTop( card );
	    card->setCardPile( faceDownDealingPile );
	    card = faceUpDealingPile->cardOnTop();
    	    highestZ++;
	}
	endDealing();

	throughDeck();
	
	moved = TRUE;
    }
}


