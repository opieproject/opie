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
#ifndef CARD_PILE_H
#define CARD_PILE_H


#include <qpoint.h>
#include <qlist.h>


enum ePileStackingType {
    pileCascades = 0, pileStacks, pileCascadesOrStacks
};


enum ePileFaceingType {
    pileFaceUp = 0, pileFaceDown, pileFaceUpOrDown
};


class Card;
class Config;


class CardPile : public QList<Card>
{
public:
    CardPile(int x, int y);
    virtual ~CardPile() { }

    int getX() { return pileX; }
    int getY() { return pileY; }
    int getNextX() { return pileNextX; }
    int getNextY() { return pileNextY; }
    int getWidth() { return pileWidth; }
    int getHeight() { return pileHeight; }

    void setX(int x) { pileX = x; }
    void setY(int y) { pileY = y; }
    void setNextX(int x) { pileNextX = x; }
    void setNextY(int y) { pileNextY = y; }
    void setWidth(int width) { pileWidth = width; }
    void setHeight(int height) { pileHeight = height; }
    
    void beginDealing() { dealing = TRUE; }
    void endDealing() { dealing = FALSE; }
    bool isDealing() { return dealing; }
    
    int distanceFromPile(int x, int y);
    int distanceFromNextPos(int x, int y);
    
    Card *cardOnTop() { return getLast(); }
    Card *cardOnBottom() { return getFirst(); }
    Card *cardInfront(Card *c);
    bool kingOnTop();

    bool addCardToTop(Card *c);
    bool addCardToBottom(Card *c);
    bool removeCard(Card *c);
    
    virtual void cardAddedToTop(Card *) { }
    virtual void cardAddedToBottom(Card *) { }
    virtual void cardRemoved(Card *) { }
    virtual bool isAllowedOnTop(Card *) { return FALSE; }
    virtual bool isAllowedOnBottom(Card *) { return FALSE; }
    virtual bool isAllowedToBeMoved(Card *) { return FALSE; }
    virtual QPoint getCardPos(Card *) { return QPoint(pileX, pileY); }
    virtual QPoint getHypertheticalNextCardPos() { return QPoint(pileX, pileY); }

    void writeConfig( Config& cfg, QString name );
    
protected:    
    int pileX, pileY;
    int pileNextX, pileNextY;
    int pileWidth, pileHeight;
    int pileCenterX, pileCenterY;
    int pileRadius;
private:
    bool dealing;
};


#endif

