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

#include "cardgame.h"
#include "canvascard.h"
#include "canvascardwindow.h"

#include <qpe/resource.h>

#include <qpainter.h>
#include <qimage.h>
#include <qpaintdevice.h>
#include <qbitmap.h>

#include <math.h>

#if defined( QT_QWS_CASSIOPEIA )
#define SLOW_HARDWARE
#endif

// Seems to be fast enough to me even without Transformations in the library
//#if defined( QT_NO_TRANSFORMATIONS ) && defined( QT_QWS_IPAQ )
//#define SLOW_HARDWARE
//#endif


QBitmap *Create180RotatedBitmap(QBitmap *srcBitmap)
{
#ifdef QT_NO_TRANSFORMATIONS
    int w = srcBitmap->width();
    int h = srcBitmap->height();
    QBitmap *dstBitmap = new QBitmap( w, h );
    // ### this is very poorly implemented and probably could be much faster
    for (int i = 0; i < w; i++)
	for (int j = 0; j < h; j++)
	    bitBlt( dstBitmap, i, j, srcBitmap, w - i - 1, h - j - 1, 1, 1 );
    return dstBitmap;
#else
    QWMatrix m;
    m.rotate( 180.0 );
    return new QBitmap( srcBitmap->xForm( m ) );
#endif
}


QPixmap *CreateScaledPixmap(QPixmap *srcPixmap, double scaleX, double scaleY)
{
#ifdef QT_NO_TRANSFORMATIONS
    int w = srcPixmap->width();
    int h = srcPixmap->height();
    int newW = (int)(w * scaleX);
    int newH = (int)(h * scaleY);
    QPixmap *dstPixmap = new QPixmap( newW, newH );
    // ### this is very poorly implemented and probably could be much faster
    for (int i = 0; i < newW; i++) {
	int srcX = w * i / newW;
	if (newH == h) {
	    // Optimise for scaleing in the X-axis only
	    bitBlt( dstPixmap, i, 0, srcPixmap, srcX, 0, 1, h );
	} else {
	    for (int j = 0; j < newH; j++) {
		int srcY = h * j / newH;
		bitBlt( dstPixmap, i, j, srcPixmap, srcX, srcY, 1, 1 );
	    }
        }
    }
    return dstPixmap;
#else
    QWMatrix s;
    s.scale( scaleX, scaleY );
    return new QPixmap( srcPixmap->xForm( s ) );
#endif
}


// Initialise static member variables to NULL
QPixmap *CanvasCard::cardsFaces = NULL;
QPixmap *CanvasCard::cardsBacks = NULL;
QBitmap *CanvasCard::cardsChars = NULL;
QBitmap *CanvasCard::cardsSuits = NULL;
QBitmap *CanvasCard::cardsCharsUpsideDown = NULL;
QBitmap *CanvasCard::cardsSuitsUpsideDown = NULL;


CanvasCard::CanvasCard( eValue v, eSuit s, bool f, QCanvas *canvas ) :
	Card(v, s, f), QCanvasRectangle( 0, 0, 1, 1, canvas ), cardBack(1), scaleX(1.0), scaleY(1.0)
{
    if ( !cardsFaces ) {
	if ( smallFlag ) {
	    cardsFaces = new QPixmap( Resource::loadPixmap( "cards/card_face_small" ) );
	    cardsBacks = new QPixmap( Resource::loadPixmap( "cards/card_back0001_small" ) );
	    cardsChars = new QBitmap( Resource::loadBitmap( "cards/card_chars_small" ) );
	    cardsSuits = new QBitmap( Resource::loadBitmap( "cards/card_suits_small" ) );
	} else {
	    cardsFaces = new QPixmap( Resource::loadPixmap( "cards/card_face" ) );
	    cardsBacks = new QPixmap( Resource::loadPixmap( "cards/card_back0001" ) );
	    cardsChars = new QBitmap( Resource::loadBitmap( "cards/card_chars" ) );
	    cardsSuits = new QBitmap( Resource::loadBitmap( "cards/card_suits" ) );
	}
	cardsCharsUpsideDown = Create180RotatedBitmap( cardsChars );
	cardsSuitsUpsideDown = Create180RotatedBitmap( cardsSuits );
    }
    xOff = cardsFaces->width() / 2;
    yOff = cardsFaces->height() / 2;
    setSize( cardsFaces->width(), cardsFaces->height() );
    setPen( NoPen );
    flipping = FALSE;
}


void CanvasCard::setCardBack(int b)
{
    if ( cardBack != b ) {

	cardBack = b;

	if ( cardsBacks )
	    delete cardsBacks;

	if ( smallFlag ) {
	    switch (cardBack) {
		case 0:
		    cardsBacks = new QPixmap( Resource::loadPixmap( "cards/card_back0001_small" ) ); break;
		case 1:
		    cardsBacks = new QPixmap( Resource::loadPixmap( "cards/card_back0002_small" ) ); break;
		case 2:
		    cardsBacks = new QPixmap( Resource::loadPixmap( "cards/card_back0003_small" ) ); break;
		case 3:
		    cardsBacks = new QPixmap( Resource::loadPixmap( "cards/card_back0004_small" ) ); break;
		case 4:
		    cardsBacks = new QPixmap( Resource::loadPixmap( "cards/card_back0005_small" ) ); break;
	    }
	} else {
	    switch (cardBack) {
		case 0:
		    cardsBacks = new QPixmap( Resource::loadPixmap( "cards/card_back0001" ) ); break;
		case 1:
		    cardsBacks = new QPixmap( Resource::loadPixmap( "cards/card_back0002" ) ); break;
		case 2:
		    cardsBacks = new QPixmap( Resource::loadPixmap( "cards/card_back0003" ) ); break;
		case 3:
		    cardsBacks = new QPixmap( Resource::loadPixmap( "cards/card_back0004" ) ); break;
		case 4:
		    cardsBacks = new QPixmap( Resource::loadPixmap( "cards/card_back0005" ) ); break;
	    }
	}

        if ( !isFacing() )
	    redraw();
    }
}


void CanvasCard::draw(QPainter &painter)
{
    int ix = (int)x(), iy = (int)y();

    QPainter *p = &painter;
    QPixmap *unscaledCard = NULL;

    if ((scaleX <= 0.98) || (scaleY <= 0.98)) 
    {
	p = new QPainter();
	unscaledCard = new QPixmap( cardsFaces->width(), cardsFaces->height() );
	p->begin(unscaledCard);
	ix = 0;
	iy = 0;
    }
	
    if ( isFacing() ) {
	
/*
	// Now add the joker and card backs to the list of pixmaps
	QPixmap *CardsBack = new QPixmap( Resource::loadPixmap( "cards/card_joker.png" ) );
	QPoint *newBackHotspot = new QPoint( 0, 0 );
	pixmaps->append((const QPixmap *)CardsBack);
	hotspots->append((const QPoint *)newBackHotspot);
*/

	int w = cardsFaces->width(), h = cardsFaces->height();

//	p->setBrush( NoBrush );
	p->setBrush( QColor( 0xFF, 0xFF, 0xFF ) );

	if ( isRed() == TRUE )
	    p->setPen( QColor( 0xFF, 0, 0 ) );
	else
	    p->setPen( QColor( 0, 0, 0 ) );

	if ( smallFlag ) {
	    p->drawPixmap( ix +      0, iy +     0, *cardsFaces );
	    p->drawPixmap( ix +      3, iy +     3, *cardsChars, 5*(getValue()-1), 0, 5, 6 );
	    p->drawPixmap( ix +     11, iy +     3, *cardsSuits, 5*(getSuit()-1), 0, 5, 6 );
	    p->drawPixmap( ix +  w-3-5, iy + h-3-6, *cardsCharsUpsideDown, 5*(12-getValue()+1), 0, 5, 6 );
	    p->drawPixmap( ix + w-11-5, iy + h-3-6, *cardsSuitsUpsideDown, 5*(3-getSuit()+1), 0, 5, 6 );
	} else {
	    p->drawPixmap( ix +      0, iy +     0, *cardsFaces );
	    p->drawPixmap( ix +      4, iy +     4, *cardsChars, 7*(getValue()-1), 0, 7, 7 );
	    p->drawPixmap( ix +     12, iy +     4, *cardsSuits, 7*(getSuit()-1), 0, 7, 8 );
	    p->drawPixmap( ix +  w-4-7, iy + h-4-7, *cardsCharsUpsideDown, 7*(12-getValue()+1), 0, 7, 7 );
	    p->drawPixmap( ix + w-12-7, iy + h-5-7, *cardsSuitsUpsideDown, 7*(3-getSuit()+1), 0, 7, 8 );
	}

    } else {
	
	p->drawPixmap( ix, iy, *cardsBacks );

    }

    if (p != &painter)
    {
	p->end();
	QPixmap *scaledCard = CreateScaledPixmap( unscaledCard, scaleX, scaleY );
	int xoff = scaledCard->width() / 2;
	int yoff = scaledCard->height() / 2;
	painter.drawPixmap( (int)x() + xOff - xoff, (int)y() + yOff - yoff, *scaledCard );
	delete p;
	delete unscaledCard;
	delete scaledCard;
    }
}


static const double flipLift = 1.5;


void CanvasCard::flipTo(int x2, int y2, int steps)
{
    flipSteps = steps;

#ifdef SLOW_HARDWARE
    move(x2,y2);
    Card::flipTo(x2,y2,steps);
#else
    int x1 = (int)x();
    int y1 = (int)y();
    double dx = x2 - x1;
    double dy = y2 - y1;

    flipping = TRUE;
    destX = x2;
    destY = y2;
    animSteps = flipSteps;
    setVelocity(dx/animSteps, dy/animSteps-flipLift);
    setAnimated(TRUE);
#endif
}


void CanvasCard::advance(int stage)
{
    if ( stage==1 ) {
	if ( animSteps-- <= 0 ) {
	    scaleX = 1.0;
	    scaleY = 1.0;
	    flipping = FALSE;
	    setVelocity(0,0);
	    setAnimated(FALSE);
	    move(destX,destY); // exact
	} else {
	    if ( flipping ) {
		if ( animSteps > flipSteps / 2 ) {
		    // animSteps = flipSteps .. flipSteps/2 (flip up) -> 1..0
		    scaleX = ((double)animSteps/flipSteps-0.5)*2;
		} else {
		    // animSteps = flipSteps/2 .. 0 (flip down) -> 0..1
		    scaleX = 1-((double)animSteps/flipSteps)*2;
		}
		if ( animSteps == flipSteps / 2-1 ) {
		    setYVelocity(yVelocity()+flipLift*2);
		    setFace( !isFacing() );
		}
	    }
	}
    }
    QCanvasRectangle::advance(stage);
}


void CanvasCard::animatedMove(int x2, int y2, int steps)
{
    destX = x2;
    destY = y2;

    double x1 = x(), y1 = y(), dx = x2 - x1, dy = y2 - y1;

    // Ensure a good speed
    while ( fabs(dx/steps)+fabs(dy/steps) < 5.0 && steps > 4 )
	steps--;

    setAnimated(TRUE);
    setVelocity(dx/steps, dy/steps);

    animSteps = steps;
}

