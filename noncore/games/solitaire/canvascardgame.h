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
#ifndef CANVAS_CARD_GAME_H
#define CANVAS_CARD_GAME_H

#include "cardgame.h"
#include "canvasshapes.h"
#include "canvascard.h"

#include <qpe/resource.h>
#include <qpe/config.h>

#include <qmainwindow.h>
#include <qpe/qpemenubar.h>
#include <qpainter.h>

#include <stdlib.h>
#include <time.h>


class CanvasCardPile;


class CanvasCardGame : public QCanvasView, public CardGame
{
public:
    CanvasCardGame(QCanvas &c, bool snap, QWidget *parent = 0, const char *name = 0, WFlags f = 0) :
    	QCanvasView( &c, parent, name, f ),
	moved(FALSE),
	moving(NULL),
	alphaCardPile( NULL ), 
	cardXOff(0), cardYOff(0),
	snapOn(snap),
	numberToDraw(1) { }

    virtual ~CanvasCardGame();

    virtual Card *newCard( eValue v, eSuit s, bool f ) {
	return new CanvasCard( v, s, f, canvas() );
    }

    virtual void readConfig( Config& cfg ) { Q_UNUSED( cfg ); }
    virtual void writeConfig( Config& cfg ) { Q_UNUSED( cfg ); }

    virtual void gameWon();
    virtual bool haveWeWon() { return FALSE; }

    virtual bool mousePressCard(Card *card, QPoint p) { Q_UNUSED(card); Q_UNUSED(p); return FALSE; }  
    virtual void mouseReleaseCard(Card *card, QPoint p) { Q_UNUSED(card); Q_UNUSED(p); }

    void cancelMoving() { moving = NULL; }
    void toggleSnap() { snapOn = (snapOn == TRUE) ? FALSE : TRUE; }
    void toggleCardsDrawn() { numberToDraw = (numberToDraw == 1) ? 3 : 1; }
    int cardsDrawn() { return numberToDraw; }
    void setNumberToDraw(int numToDraw) { this->numberToDraw = numToDraw; }

    void readPile( Config& cfg, CardPile *pile, QString name, int& highestZ );

protected:
    void contentsMousePressEvent(QMouseEvent *e);
    void contentsMouseReleaseEvent(QMouseEvent *e);
    void contentsMouseMoveEvent(QMouseEvent *e);

protected:
    // Mouse event state variables
    bool moved;
    CanvasCard *moving;
    CanvasCardPile *alphaCardPile;
    int cardXOff, cardYOff;

private:
    bool snapOn;
    int numberToDraw;
};


#endif

