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

#include "snake.h"
#include "target.h"
#include "codes.h"

#include <qpe/resource.h>

#include <qregexp.h>

static int Piecekey[4][4] = { {6, 0, 4, 3 }, {0, 6, 2, 1 }, { 1, 3, 5, 0 }, {2, 4, 0, 5 } };

Snake::Snake(QCanvas* c)
{
   canvas = c; 
   score = 0;
   snakelist.setAutoDelete(true);
   autoMoveTimer = new QTimer(this);
   connect( autoMoveTimer, SIGNAL(timeout()), this, SLOT(moveSnake()) );
   createSnake();
}

void Snake::createSnake()
{
   snakeparts = new QCanvasPixmapArray();
   QString s0 = Resource::findPixmap("snake/s0001");
   s0.replace(QRegExp("0001"),"%1");
   snakeparts->readPixmaps(s0, 15);
 
   grow = 0;
   last = Key_Right;
 
   QCanvasSprite* head = new QCanvasSprite(snakeparts, canvas );
   head->setFrame(7);
   snakelist.insert(0, head);
   head->show();
   head->move(34, 16);
 
   QCanvasSprite* body = new QCanvasSprite(snakeparts, canvas );
   body->setFrame(6);
   snakelist.append( body );
   body->show();
   body->move(18, 16);
 
   QCanvasSprite* end = new QCanvasSprite(snakeparts, canvas );
   end->setFrame(11);
   snakelist.append( end );
   end->show();
   end->move(2, 16);
 
   currentdir = right; 
   speed = 250;
   autoMoveTimer->start(speed);
   moveSnake();
}

void Snake::increaseSpeed()
{
   if (speed > 150) 
       speed = speed - 5;
   autoMoveTimer->start(speed);
}

void Snake::go(int newkey)
{
   // check key is a direction
   if (!( (newkey == Key_Up) || (newkey == Key_Left) || 
          (newkey == Key_Right) || (newkey == Key_Down) ))
        return;
   // check move is possible   
   if ( ((currentdir == left) && ((newkey == Key_Right) || (newkey == Key_Left)) ) ||
        ((currentdir == right) && ((newkey == Key_Left) || (newkey == Key_Right)) ) ||
        ((currentdir == up) && ((newkey == Key_Down) || (newkey == Key_Up)) ) ||
        ((currentdir == down) && ((newkey == Key_Up) || (newkey == Key_Down)) ) )
       return;
   else {
       Snake::changeHead(newkey);
       Snake::moveSnake();
   }
}   

void Snake::move(Direction dir)
{
   autoMoveTimer->start(speed);
   int x = 0;
   int y = 0;
   newdir = dir;
   switch (dir) {
      case right: x = 16; break;
      case left: x = -16; break;
      case down: y = 16; break;
      case up: y = -16; break;
   }
   int index = lookUpPiece(currentdir, newdir);
   QCanvasSprite* sprite = new QCanvasSprite(snakeparts, canvas );
   sprite->setFrame(index);
   snakelist.insert(1, sprite);
   sprite->move(snakelist.first()->x(), snakelist.first()->y() );

   snakelist.first()->moveBy(x, y);
   if (grow <= 0)
      changeTail();
   else
      grow--;
   sprite->show();
 
   currentdir = dir;
}                

void Snake::changeTail()
{
   snakelist.removeLast();
 
   double lastx = snakelist.last()->x();
   double prevx = snakelist.prev()->x();
   int index = 0;
 
     if ( prevx == lastx ) {  //vertical
         if ( snakelist.prev()->y() > snakelist.last()->y() )
              index = 13;
         else
              index = 14;
     } else {  //horizontal
         if (snakelist.prev()->x() > snakelist.last()->x() )
              index = 11;
         else
              index = 12;
     }
 
     snakelist.last()->setFrame(index);
}
 
void Snake::changeHead(int lastkey)
{
   int index = 0;
   last = lastkey;
 
   switch (last)
   {
      case Key_Up: index = 10; break;
      case Key_Left: index = 8;  break;
      case Key_Right: index = 7; break;
      case Key_Down: index = 9;  break;
   }
 
    if (index) {
       snakelist.first()->setFrame(index);
    }
}

// returns an integer corresponding to a particular type of snake piece
int Snake::lookUpPiece(Direction currentdir, Direction newdir)
{
   return Piecekey[currentdir][newdir];
}
 
void Snake::extendSnake()
{
   grow++;
}         

void Snake::moveSnake()
{
   switch (last)
   {
      case Key_Up: move(up); break;
      case Key_Left: move(left);  break;
      case Key_Right: move(right); break;
      case Key_Down: move(down);  break;
    }
    detectCrash();
}

void Snake::detectCrash()
{
    QCanvasSprite* head = snakelist.first();
    QCanvasItem* item;
    QCanvasItemList l=head->collisions(FALSE);
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
       item = *it;
       // check if snake hit target
       if ( (item->rtti()== 1500 ) && (item->collidesWith(head)) ) {
              Target* target = (Target*) item;
              target->done();
              emit targethit();
              extendSnake();
              setScore(5); 
              return;
       }
       // check if snake hit obstacles
       if ( (item->rtti()==1600) && (item->collidesWith(head)) ) {
             emit dead();
             autoMoveTimer->stop();
             return;
       }
    }
    //check if snake hit itself
    for (uint i = 3; i < snakelist.count(); i++) {
       if (head->collidesWith(snakelist.at(i)) ) {
            emit dead(); 
            autoMoveTimer->stop();
            return;
       }
   }
   //check if snake hit edge
   if ( (head->x() > canvas->width()-5) || (head->y() > canvas->height()-10)
         || (head->x() <2) || (head->y() <-5) ) {
        emit dead(); 
        autoMoveTimer->stop();
        return; 
   }
} 

void Snake::setScore(int amount)
{ 
   score = score + amount;
   emit scorechanged();
}

int Snake::getScore()
{
   return score;
}

Snake::~Snake()
{
    autoMoveTimer->stop();
}
