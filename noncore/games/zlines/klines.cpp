/***************************************************************************
                          klines.cpp  -  description
                             -------------------
    begin                : Fri May 19 2000
    copyright            : (C) 2000 by Roman Merzlyakov
    email                : roman@sbrf.barrt.ru
    copyright            : (C) 2000 by Roman Razilov
    email                : Roman.Razilov@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* changes
21.05.2000    Roman Razilov     Menu game/Next
*/
//
// The implementation of the KLines widget
//

#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <qapplication.h>


#include "klines.h"


/*
   Creates the KLines widget and sets saved options (if any).
 */

KLines::KLines(QWidget *par, const char* n, WFlags fl) : QMainWindow(par,n,fl)
{
	time_t t;
	time(&t);
	srand((unsigned int)t + getpid());

	setCaption(QString("ZLines"));

	mwidget = new MainWidget(this);
	setCentralWidget( mwidget );

	lsb = mwidget->GetLsb();
	lPrompt = mwidget->GetPrompt();

	menu = menuBar();
	game = new QPopupMenu;
	edit = new QPopupMenu;

	game->insertItem(tr("&New game"), this, SLOT(stopGame()), CTRL+Key_N );
	game->insertSeparator();
	game->insertItem(tr("Ne&xt"), this, SLOT(makeTurn()), Key_N );
	game->insertSeparator();
	idMenuPrompt = game->insertItem( tr("&Show next"), this, SLOT(switchPrompt()), CTRL+Key_P );
	game->setCheckable(true);
	game->setItemChecked(idMenuPrompt, lPrompt->getState());
	game->insertSeparator();
	game->insertItem(tr("&Quit"), qApp, SLOT(quit()), CTRL+Key_Q );

	idMenuUndo = edit->insertItem(tr("Und&o"), this, SLOT(undo()), CTRL+Key_Z );

	menu->insertItem( tr("&Game"), game );
	menu->insertItem( tr("&Edit"), edit );

	menu->show();

	score = 0;
	prev_score = 0;

	mwidget->setMessage(tr("Points: 0"));

	startGame();
}

/*
   Saves the options and destroys the KLines widget.
 */

KLines::~KLines()
{
}

/*
   Resize event of the KLines widget.
 */

void KLines::resizeEvent( QResizeEvent *e )
{
	QMainWindow::resizeEvent(e);
}

void KLines::setMinSize()
{
	//  setMinimumSize( gr->wHint(), gr->hHint() + menu->height() + stat->height() +
	//      tool->height() );
}

void KLines::startGame()
{
	score = 0;
	prev_score = 0;
	bUndo=TRUE;

	lsb->clearField();
	generateRandomBalls();
	placeBalls();
	generateRandomBalls();
	edit->setItemEnabled(idMenuUndo, FALSE );
	updateStat();
}
void KLines::stopGame()
{
	debug("Klines::stopGame");
	endGame();
}

void KLines::searchBallsLine()
{
}

void KLines::generateRandomBalls()
{

	for( int i = 0 ; i < BALLSDROP ; i++ )
	{
		nextBalls_undo[i] = nextBalls[i];		
		nextBalls[i] = bUndo ?
			rand() % NCOLORS:
			nextBalls_redo[i];
	}
	lPrompt->SetBalls(nextBalls);
}

void KLines::placeBalls()
{
	lsb->placeBalls(nextBalls);
	debug("exit from placeBalls");
}

void KLines::undo()
{
	debug("Undo");
	if (!bUndo)
		return;
	for( int i = 0 ; i < BALLSDROP ; i++ )
	{
		nextBalls_redo[i] = nextBalls[i];		
		nextBalls[i] = nextBalls_undo[i];
	}
	lPrompt->SetBalls(nextBalls);
	lsb->undo();
	switchUndo(FALSE);
}

void KLines::makeTurn()
{
	placeBalls();
	generateRandomBalls();
	switchUndo(TRUE);
}

void KLines::addScore(int ballsErased)
{   if(ballsErased >= 5){
	score += 2*ballsErased*ballsErased - 20*ballsErased + 60 ;
	if( !lPrompt->getState() ) score+= 1;
	updateStat();
						};
}

void KLines::updateStat()
{
	mwidget->setMessage(tr(" Score: %1 ").arg(score));
}

void KLines::endGame()
{
	startGame();
}

void KLines::switchPrompt()
{
	lPrompt->setPrompt(!lPrompt->getState());
	game->setItemChecked(idMenuPrompt, lPrompt->getState());
}

void KLines::switchUndo(bool bu)
{
	bUndo = bu;
	edit->setItemEnabled(idMenuUndo, bUndo );
}

void KLines::help()
{
	//  KApplication::getKApplication()->invokeHTMLHelp("", "");
}


