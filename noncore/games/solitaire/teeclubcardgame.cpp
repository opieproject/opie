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
#include "teeclubcardgame.h"
  
 
extern int highestZ;
 
 
TeeclubCardGame::TeeclubCardGame(QCanvas *c, bool snap, QWidget *parent) : CanvasCardGame(*c, snap, parent, 2)	// Use 2 Decks
{
    highestZ = 0;

    for (int i = 0; i < 8; i++) {
	discardPiles[i] = new TeeclubDiscardPile( 27 + i * 26, 10, canvas() );
        addCardPile(discardPiles[i]);
    }
    for (int i = 0; i < 9; i++) {
        workingPiles[i] = new TeeclubWorkingPile(  2 + i * 26, 50, canvas() );
        addCardPile(workingPiles[i]);
    }
    faceDownDealingPile = new TeeclubFaceDownDeck( 2, 10, canvas() );
}


void TeeclubCardGame::deal(void)
{
    highestZ = 1;
    int t = 0;

    beginDealing();

    for (int i = 0; i < 9; i++) {
	workingPiles[i]->setOffsetDown(13);
	workingPiles[i]->beginPileResize();
	for (int k = 0; k < 5; k++, t++) {
	    Card *card = cards[t];
	    workingPiles[i]->addCardToTop(card);
	    card->setCardPile( workingPiles[i] );
	    card->setPos( 0, 0, highestZ );
	    card->setFace(TRUE);
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


void TeeclubCardGame::resizePiles()
{
    beginDealing();
    for (int i = 0; i < 9; i++) {    
        while ((workingPiles[i]->getCardPos(NULL).y() > 230) && (workingPiles[i]->getOffsetDown()>1)) {
            // Resizen des Stapels
            workingPiles[i]->setOffsetDown(workingPiles[i]->getOffsetDown()-1);
            Card *card = workingPiles[i]->cardOnBottom();
            int p=0;
            while (card != NULL) {
                card->setPos( 0, 0, p++ );
                card->move( workingPiles[i]->getCardPos( card ) );
                card = workingPiles[i]->cardInfront(card);
            }
        }
    }
    endDealing();
}


void TeeclubCardGame::readConfig( Config& cfg )
{
    cfg.setGroup("GameState");

    // Create Cards, but don't shuffle or deal them yet
    createDeck();

    // Move the cards to their piles (deal them to their previous places)
    beginDealing();

    highestZ = 1;

    for (int i = 0; i < 8; i++) {
        QString pile;
        pile.sprintf( "TeeclubDiscardPile%i", i );
        readPile( cfg, discardPiles[i], pile, highestZ );
    }

    for (int i = 0; i < 9; i++) {
	workingPiles[i]->endPileResize();
        QString pile;
        pile.sprintf( "TeeclubWorkingPile%i", i );
        readPile( cfg, workingPiles[i], pile, highestZ );
	workingPiles[i]->beginPileResize();
    }

    readPile( cfg, faceDownDealingPile, "TeeclubFaceDownDealingPile", highestZ );

    highestZ++;

    endDealing();
    resizePiles();
}


void TeeclubCardGame::writeConfig( Config& cfg )
{
    cfg.setGroup("GameState");
    for ( int i = 0; i < 8; i++ ) {
	QString pile;
	pile.sprintf( "TeeclubDiscardPile%i", i );
	discardPiles[i]->writeConfig( cfg, pile );
    }
    for ( int i = 0; i < 9; i++ ) {
	QString pile;
	pile.sprintf( "TeeclubWorkingPile%i", i );
	workingPiles[i]->writeConfig( cfg, pile );
    }
    faceDownDealingPile->writeConfig( cfg, "TeeclubFaceDownDealingPile" );
}


bool TeeclubCardGame::mousePressCard( Card *card, QPoint p )
{
    Q_UNUSED(p);

    CanvasCard *item = (CanvasCard *)card;
    if (item->isFacing() != TRUE) {
	// From facedown stack
	if ((item->x() == 2) && ((int)item->y() == 10)) {		// Deal 1 card
	    // Move 8 cards, one to each workingPile
	    beginDealing();
	    CanvasCard *card =  (CanvasCard *)faceDownDealingPile->cardOnTop(); 
            card->setZ(highestZ);
            highestZ++;
	    faceDownDealingPile->removeCard(card);
	    workingPiles[0]->addCardToTop(card);
	    card->setCardPile( workingPiles[0] );
	    card->setFace(FALSE);
	    QPoint p = workingPiles[0]->getCardPos(card);
	    card->flipTo( p.x(), p.y() );
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



void TeeclubCardGame::mousePress(QPoint p)
{
    Q_UNUSED(p);
}


