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
#ifndef CANVAS_CARD_H
#define CANVAS_CARD_H


#include <qpainter.h>
#include <qbitmap.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <qcanvas.h>
#include "cardgame.h"


// ### Just made the number up, is that what you do???
static const int canvasCardId = 2434321;


class CanvasCard : public Card, public QCanvasRectangle
{
public:
    CanvasCard( eValue v, eSuit s, bool f, QCanvas *canvas );
    virtual ~CanvasCard() { canvas()->removeItem(this); }

    int rtti () const { return canvasCardId; }
    void move(QPoint p) { QCanvasItem::move( p.x(), p.y() ); }
    void move(int x, int y) { QCanvasItem::move( x, y ); }
    void animatedMove(int x, int y, int steps = 10);
    void animatedMove() { animatedMove(savedX, savedY); }
    void savePos(void) { savedX = (int)x(); savedY = (int)y(); }
    void moveToPile(int p) { Q_UNUSED(p); }
    void setCardBack(int b);
    
    /*virtual*/ void flipTo(int x, int y, int steps = 8);
    /*virtual*/ void setPos( int x, int y, int z ) { setX( x ); setY( y ); setZ( z ); }
    /*virtual*/ void showCard(void) { show(); }
    /*virtual*/ void redraw(void) { hide(); show(); }
    /*virtual*/ void draw(QPainter &p);

    void advance(int stage);

protected:
    /*virtual*/ void flip(void) { redraw(); }

private:
    int destX, destY;
    int animSteps;
    int flipSteps;
    bool flipping;
    int savedX, savedY;
    int cardBack;
    int oldCardBack;
    double scaleX, scaleY;
    int xOff, yOff;
    static QPixmap *cardsFaces;
    static QPixmap *cardsBacks;
    static QBitmap *cardsChars;
    static QBitmap *cardsSuits;
    static QBitmap *cardsCharsUpsideDown;
    static QBitmap *cardsSuitsUpsideDown;
};


#endif

