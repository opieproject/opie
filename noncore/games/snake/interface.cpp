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

#include "interface.h"

#include <qpe/resource.h>

#include <qpe/qpetoolbar.h>
#include <qtoolbutton.h>
#include <qstyle.h>
#include <qapplication.h>
#include <qmessagebox.h>

SnakeGame::SnakeGame(QWidget* parent, const char* name, WFlags f) :
    QMainWindow(parent,name,f),
    canvas(232, 258)
{
    setCaption( tr("Snake") );
    QPixmap bg = Resource::loadPixmap("grass");
    canvas.setBackgroundPixmap(bg);
    canvas.setUpdatePeriod(100);
    snake = 0;

    cv = new QCanvasView(&canvas, this);

    pauseTimer = new QTimer(this);
    connect(pauseTimer, SIGNAL(timeout()), this, SLOT(wait()) );

    setToolBarsMovable( FALSE );

    QPEToolBar* toolbar = new QPEToolBar( this);
    toolbar->setHorizontalStretchable( TRUE );

    QPixmap newicon = Resource::loadPixmap("ksnake");
    setIcon(newicon);
    (void)new QToolButton(newicon, tr("New Game"), 0,
                            this, SLOT(newGame()), toolbar, "New Game");

    scorelabel = new QLabel(toolbar);
    showScore(0);
    scorelabel->setBackgroundMode( PaletteButton );
    scorelabel->setAlignment( AlignRight | AlignVCenter | ExpandTabs );
    toolbar->setStretchableWidget( scorelabel );

    setFocusPolicy(StrongFocus);

    setCentralWidget(cv);

    QTimer::singleShot( 16, this, SLOT(welcomescreen()) ); 
    gamestopped = true; 
    waitover = true;
}

SnakeGame::~SnakeGame()
{
   delete snake;
}

void SnakeGame::resizeEvent(QResizeEvent *)
{
    QSize s = centralWidget()->size();
    int fw = style().defaultFrameWidth();
    canvas.resize( s.width() - fw - 2, s.height() - fw - 2);
}

void SnakeGame::welcomescreen()
{
   QCanvasText* title = new QCanvasText(tr("SNAKE!"), &canvas);
   title->setColor(yellow);
   title->setFont( QFont("times", 18, QFont::Bold) );  
   int w = title->boundingRect().width();
   title->move(canvas.width()/2 -w/2, canvas.height()/2-110);
   title->show();
   QCanvasPixmapArray* titlearray = new QCanvasPixmapArray(Resource::findPixmap("title"));
   QCanvasSprite* titlepic = new QCanvasSprite(titlearray, &canvas);
   titlepic->move(canvas.width()/2 - 33, canvas.height()/2-85);
   titlepic->show(); 
   QCanvasText* instr = new QCanvasText(tr("Use the arrow keys to guide the\n"
				  "snake to eat the mouse. You must not\n"
			          "crash into the walls, edges or its tail."),
					&canvas);
   w = instr->boundingRect().width();
   instr->move(canvas.width()/2-w/2, canvas.height()/2-20);
   instr->setColor(white);
   instr->show();
   QCanvasText* cont = new QCanvasText(tr("Press Any Key To Start"), &canvas);
   w = cont->boundingRect().width();
   cont->move(canvas.width()/2-w/2, canvas.height()-20);
   cont->setColor(yellow);
   cont->show(); 

}

void SnakeGame::newGame()
{   
    clear();
    snake = new Snake(&canvas);
    connect(snake, SIGNAL(dead()), this, SLOT(gameOver()) );
    connect(snake, SIGNAL(targethit()), this, SLOT(levelUp()) );
    connect(snake, SIGNAL(scorechanged()), this, SLOT(scoreInc()) );
    connect(this, SIGNAL(moveFaster()), snake, SLOT(increaseSpeed()) );
    last = 0;
    targetamount = 1;
    notargets = 1;
    level = 1;
    stage = 1;
    showScore(0);
    gamestopped = false;
    waitover = true;
    int y = canvas.height()-50;
    (void)new Obstacle(&canvas, 32);
    (void)new Obstacle(&canvas, y);
    createTargets();
}


void SnakeGame::showScore(int score)
{
    scorelabel->setText(tr("     Score :    %1   ").arg(score) );
}


void SnakeGame::scoreInc()
{
   showScore( snake->getScore() );
}

void SnakeGame::levelUp()
{
   notargets--;
   if (notargets == 0) {
        stage++;
       if (stage == 3) {
          level++;
          emit moveFaster();
          targetamount++;
          stage = 0;
       }
       createTargets();
   }
}

void SnakeGame::createTargets()
{  
   for (int i = 0; i < targetamount; i++)
       (void)new Target(&canvas);
   notargets = targetamount;
}

void SnakeGame::clear()
{
   delete snake;
   snake = 0;
   QCanvasItemList l = canvas.allItems();
   for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        delete *it;
   }  
}

void SnakeGame::gameOver()
{
   int score = snake->getScore();
   QString scoreoutput="";
   scoreoutput.setNum(score);
   QCanvasText* gameover = new QCanvasText(tr("GAME OVER!\n Your Score: %1").arg( scoreoutput), &canvas); 
 
   gameover->setZ(100);
   gameover->setColor(yellow);
   gameover->setFont( QFont("times", 18, QFont::Bold) );
   int w = gameover->boundingRect().width();
   gameover->move(canvas.width()/2 -w/2, canvas.height()/2 -50);
   gameover->show();
   gamestopped = true;
   waitover = false;
   pauseTimer->start(1500);
}

void SnakeGame::wait()
{
   waitover = true;
   pauseTimer->stop();
   QCanvasText* cont = new QCanvasText(tr("Press Any Key to Begin a New Game."),
                                       &canvas);         
   cont->setZ(100);
   cont->setColor(white);
   int w = cont->boundingRect().width();
   cont->move(canvas.width()/2 -w/2, canvas.height()/2);
   cont->show();
}

void SnakeGame::keyPressEvent(QKeyEvent* event)
{ 
   if (gamestopped) {
        if (waitover) 
           newGame();
        else 
           return;
   }
   else { 
       int newkey = event->key();
       snake->go(newkey);
   }
}

