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
#ifndef CARD_GAME_H
#define CARD_GAME_H


#include <qpoint.h>
#include "card.h"
#include "cardpile.h"
#include "carddeck.h"
#include "cardgamelayout.h"


class CardGame : public CardGameLayout, public CardDeck
{
public:
    CardGame(int numOfJokers = 0) : CardGameLayout(), CardDeck(numOfJokers) { }
    virtual ~CardGame() { }
    virtual void newGame();
    virtual void mousePress(QPoint p) { Q_UNUSED(p); }
    virtual void mouseRelease(QPoint p) { Q_UNUSED(p); }
    virtual void mouseMove(QPoint p) { Q_UNUSED(p); }
private:
};


#endif

