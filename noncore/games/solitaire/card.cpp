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

#include "card.h"

#include <qpe/config.h>

#include <qpoint.h>
#include <qlist.h>

/*
Card( eValue v, eSuit s, bool f ) :
    val(v), suit(s), faceUp(f), showing(FALSE), ix(0), iy(0), iz(0), cardPile(NULL) { }
virtual ~Card() { }
eValue getValue() { return val; }
eSuit getSuit() { return suit; }
CardPile *getCardPile() { return cardPile; }
bool isFacing() { return faceUp; }
bool isShowing() { return showing; }
bool isRed() { return ((suit == diamonds) || (suit == hearts)); }
int getX(void) { return ix; }
int getY(void) { return iy; }
int getZ(void) { return iz; }
void setCardPile(CardPile *p) { cardPile = p; }
void setFace(bool f) { faceUp = f; }
void flip(void) { flipTo(getX(), getY()); }
virtual void setPos(int x, int y, int z) { ix = x; iy = y; iz = z; }
virtual void move(int x, int y) { ix = x; iy = y; }
virtual void move(QPoint p) { ix = p.x(); iy = p.y(); }
virtual void flipTo(int x, int y, int steps = 8) { ix = x; iy = y; faceUp = !faceUp; redraw(); Q_UNUSED(steps); }
virtual void showCard(void) { showing = TRUE; }
virtual void hideCard(void) { showing = FALSE; }
virtual void redraw(void) { }
*/


