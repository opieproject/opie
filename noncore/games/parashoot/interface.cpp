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
#include "man.h"

#include <qpe/resource.h>

#include <qlabel.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qstyle.h>
#include <qpe/qpetoolbar.h>
#include <qtoolbutton.h>
 
ParaShoot::ParaShoot(QWidget* parent, const char* name, WFlags f) :
    QMainWindow(parent,name,f),
    canvas(232, 258),
    fanfare("level_up"),
    score(0)
{
    canvas.setAdvancePeriod(80);
    QPixmap bg = Resource::loadPixmap("parashoot/sky");
    canvas.setBackgroundPixmap(bg);

    pb = new QCanvasView(&canvas, this);
    pb->setFocus();

    setToolBarsMovable( FALSE );

    QPEToolBar* toolbar = new QPEToolBar(this);
    toolbar->setHorizontalStretchable( TRUE );

    setCaption( tr("ParaShoot") );
    QPixmap newicon = Resource::loadPixmap("parashoot/manicon");
    setIcon(newicon);
    new QToolButton(newicon, tr("New Game"), 0,
                           this, SLOT(newGame()), toolbar, "New Game");

    levelscore = new QLabel(toolbar);
    levelscore->setBackgroundMode( PaletteButton );
    levelscore->setAlignment( AlignRight | AlignVCenter | ExpandTabs );
    toolbar->setStretchableWidget( levelscore );
    showScore(0,0);
    
    setCentralWidget(pb);

    autoDropTimer = new QTimer(this);
    connect (autoDropTimer, SIGNAL(timeout()), this, SLOT(play()) );
  
    pauseTimer = new QTimer(this);
    connect(pauseTimer, SIGNAL(timeout()), this, SLOT(wait()) ); 

    setFocusPolicy(StrongFocus);

    newGame();
}


void ParaShoot::resizeEvent(QResizeEvent *)
{
    QSize s = centralWidget()->size();
    int fw = style().defaultFrameWidth();
    canvas.resize( s.width() - fw - 2, s.height() - fw - 2);
}


void ParaShoot::showScore( int score, int level )
{
    levelscore->setText(tr("     Level: %1       Score: %2   ").arg(score).arg(level) );
}


void ParaShoot::newGame()
{
    clear();
    if (pauseTimer->isActive()) 
	pauseTimer->stop();
    clear();
    Man::setManCount(0);
    score = 0;
    Bullet::setShotCount(0);
    Bullet::setNobullets(0);
    nomen = 2;
    Bullet::setLimit(nomen);
    level = 0;
    updatespeed = 80;
    showScore(0,0);
    gamestopped = false;
    Helicopter::deleteAll();
    waitover = true;
    base = new Base(&canvas);
    cannon = new Cannon(&canvas);
    connect( cannon, SIGNAL(score(int)), this, SLOT(increaseScore(int)));
    autoDropTimer->start(100);
}


void ParaShoot::clear()
{
   autoDropTimer->stop();
//   QCanvasItem* item;
   QCanvasItemList l = canvas.allItems();
   for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
        delete *it; 
   }
}

void ParaShoot::gameOver()
{
    QCanvasItem* item;
    QCanvasItemList l = canvas.allItems();
    for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) { 
         item = *it;
         if ((item->rtti()==1500) || (item->rtti()==1600) || item->rtti()==1900)
         item->setAnimated(false);
    }
    autoDropTimer->stop();
    Helicopter::silenceAll();

    int shots = Bullet::getShotCount();

    int shotsFired = cannon->shotsFired();
    if ( shotsFired == 0 ) 
	shotsFired = 1;
    QCanvasText* gameover = new QCanvasText(
		     tr( "       GAME OVER!\n"
			 "       Your Score:  %1\n"
			 " Parachuters Killed: %2\n"
			 "        Accuracy: %3% " ).arg(score).arg(shots).arg(shots * 100 / shotsFired ), 
		                     &canvas);
    gameover->setColor(red);
    gameover->setFont( QFont("times", 18, QFont::Bold) );
    gameover->move(canvas.width()/2 -110, canvas.height()/2 -50);
    gameover->setZ(500);
    gameover->show();
    gamestopped = true;
    waitover = false;
    pauseTimer->start(3000);
}

void ParaShoot::wait()
{
   waitover = true;
   pauseTimer->stop();
}

void ParaShoot::play()
{   
     if (Man::getManCount() < nomen ) {
          new Man(&canvas);
     }
     if (Base::baseDestroyed()) {
          gameOver();
          return;
     }
}

void ParaShoot::increaseScore(int x)
{
    score += x;
    if ( score / 150 != (score-x) / 150 )
       levelUp(); 
    showScore(level,score);
}

void ParaShoot::levelUp()
{
    level++; 
    int stage = level % 3;
    switch(stage) {
      case 0:
	nomen++;
	Bullet::setLimit(nomen);
	fanfare.play();
	break;
      case 1:
	new Helicopter(&canvas);
	break; 
      case 2:
	moveFaster();
	fanfare.play();
	break; 
      default: return;
    }
}

void ParaShoot::moveFaster()
{
   if (updatespeed > 50)
       updatespeed = updatespeed-5;
   else
       updatespeed = updatespeed-3;
   canvas.setAdvancePeriod(updatespeed);
}

void ParaShoot::keyPressEvent(QKeyEvent* event)
{
    if (gamestopped) {
	if (waitover)
	    newGame();
	else 
	    return;
    } else {
	switch(event->key()) {
	  case Key_Up:
	  case Key_F1:
	  case Key_F9:
	  case Key_Space:
	    cannon->shoot();
	    break;
	  case Key_Left:
	    cannon->pointCannon(Cannon::Left);
	    lastcannonkey=Key_Left;
	    break;
	  case Key_Right:
	    cannon->pointCannon(Cannon::Right);
	    lastcannonkey=Key_Right;
	    break;
	  default:
	    return;
	}
    }
}

void ParaShoot::keyReleaseEvent(QKeyEvent* event)
{
    if ( lastcannonkey == event->key() )
	cannon->pointCannon(Cannon::NoDir);
}
