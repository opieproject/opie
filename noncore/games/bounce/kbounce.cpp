/*
 * Copyright (C) 2000 Stefan Schimanski <1Stein@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License,Life or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qlayout.h>
#include <qtimer.h>
#include <qlcdnumber.h>
#include <qmessagebox.h>
#include <qmainwindow.h>
#include <qpe/qpeapplication.h>

#include "kbounce.h"
#include "game.h"
#include <qlabel.h>

KJezzball::KJezzball() : QMainWindow(0), m_gameWidget( 0 )
{
	setCaption(tr("Bounce"));
	// setup variables
	m_game.level = 1;
	m_game.score = 0;
	m_state = Idle;


	menu = menuBar();
	game = new QPopupMenu;
	game->insertItem(tr("&New game"), this, SLOT(newGame()), Key_N );
	game->insertItem(tr("&Pause game"), this, SLOT(pauseGame()), Key_P );
	game->insertSeparator();
	game->insertItem(tr("&About"), this, SLOT(about()));
	menu->insertItem( tr("&Game"), game );

	// create widgets
	m_view = new QWidget( this, "m_view" );
	setCentralWidget( m_view );

	m_layout = new QGridLayout( m_view );
	m_layout->setSpacing( 0 );
	m_layout->setMargin( 0 );

	ScoreLabel = new QLabel( m_view, "ScoreLabel" );
	ScoreLabel->setText( tr( "Score: 00" ) );
	ScoreLabel->setAlignment( int( QLabel::AlignCenter ) );

	m_layout->addWidget( ScoreLabel, 1, 0 );

	LivesLabel = new QLabel( m_view, "LivesLabel" );
	LivesLabel->setText( tr( "Lives: 0%" ) );
	LivesLabel->setAlignment( int( QLabel::AlignCenter ) );

	m_layout->addWidget( LivesLabel, 1, 2 );

	FilledLabel = new QLabel( m_view, "FilledLabel" );
	FilledLabel->setText( tr( "Filled: 00%" ) );
	FilledLabel->setAlignment( int( QLabel::AlignCenter ) );

	m_layout->addWidget( FilledLabel, 1, 1 );

	TimeLabel = new QLabel( m_view, "TimeLabel" );
	TimeLabel->setText( tr( "Time: 00" ) );
	TimeLabel->setAlignment( int( QLabel::AlignCenter ) );

	m_layout->addWidget( TimeLabel, 1, 3 );

	// create timers
	m_nextLevelTimer = new QTimer( this, "m_nextLevelTimer" );
	connect( m_nextLevelTimer, SIGNAL(timeout()), this, SLOT(switchLevel()) );

	m_gameOverTimer = new QTimer( this, "m_gameOverTimer" );
	connect( m_gameOverTimer, SIGNAL(timeout()), this, SLOT(gameOverNow()) );

	m_timer = new QTimer( this, "m_timer" );
	connect( m_timer, SIGNAL(timeout()), this, SLOT(second()) );

	// create demo game
	createLevel( 1 );
}

void KJezzball::newGame()
{
	// Check for running game
	closeGame();
	if ( m_state==Idle )
	{
		// update displays
		m_game.level = 1;
		m_game.score = 0;

		setCaption(tr("Bounce Level %1").arg(m_game.level));
		ScoreLabel->setText( tr( "Score: %1" ).arg(m_game.score) );

		// start new game
		m_state = Running;

		createLevel( m_game.level );
		startLevel();
	}
}

void KJezzball::about()
{
	QMessageBox::information( this, "About", "Written by: Stefan Schimanski\nPorted by: Martin Imobersteg\n\nThis program is distributed under\nthe terms of the GPL v2." );
}

void KJezzball::closeGame()
{
	if ( m_state!=Idle )
	{
		stopLevel();
		m_state = Idle;
	}
}

void KJezzball::pauseGame()
{
	switch ( m_state )
	{
		case Running:
			m_state = Paused;
			m_gameWidget->display( tr("Game paused.\nPress P to continue!") );
			stopLevel();
			break;

		case Paused:
		case Suspend:
			m_state = Running;
			m_gameWidget->display( QString::null );
			startLevel();
			break;

		case Idle:
			break;
	}
}

void KJezzball::gameOver()
{
	stopLevel();
	m_gameOverTimer->start( 100, TRUE );
}


void KJezzball::gameOverNow()
{
	m_state = Idle;

	QString score;
	score.setNum( m_game.score );
	QMessageBox::information( this, "Game Over", tr("Game Over!\nScore: %1").arg(score) );
}

void KJezzball::focusOutEvent( QFocusEvent *ev )
{
	if ( m_state==Running )
	{
		stopLevel();
		m_state = Suspend;
	}

	QMainWindow::focusOutEvent( ev );
}

void KJezzball::focusInEvent ( QFocusEvent *ev )
{
	if ( m_state==Suspend )
	{
		startLevel();
		m_state = Running;
	}

	QMainWindow::focusInEvent( ev );
}

void KJezzball::second()
{
	m_level.time--;
	TimeLabel->setText( tr( "Time: %1" ).arg(m_level.time) );
	if ( m_level.time<=0 )
	{
		gameOver();
	}
}

void KJezzball::died()
{
	m_level.lifes--;
	LivesLabel->setText( tr( "Lives: %1" ).arg(m_level.lifes) );
	if ( m_level.lifes==0 ) gameOver();
}

void KJezzball::newPercent( int percent )
{
	FilledLabel->setText( tr( "Filled: %1%" ).arg(percent) );
	if ( percent>=75 )
	{
		m_level.score = m_level.lifes*15 + (percent-75)*2*(m_game.level+5);
		nextLevel();
	}
}

void KJezzball::createLevel( int level )
{
	// destroy old game
	if ( m_gameWidget ) delete m_gameWidget;

	m_gameWidget = new JezzGame( level+1, m_view, "m_gameWidget" );

	m_gameWidget->show();
	m_layout->addMultiCellWidget( m_gameWidget, 0, 0, 0, 3 );
	connect( m_gameWidget, SIGNAL(died()), this, SLOT(died()) );
	connect( m_gameWidget, SIGNAL(newPercent(int)), this, SLOT(newPercent(int)) );

	// update displays
	m_level.lifes = level+1;
	LivesLabel->setText( tr( "Lives: %1" ).arg(m_level.lifes) );
	FilledLabel->setText( tr( "Filled: 0%" ) );

	m_level.time = (level+2)*30;
	TimeLabel->setText( tr( "Time: %1" ).arg(m_level.time) );

	m_level.score = 0;
}

void KJezzball::startLevel()
{
	if ( m_gameWidget )
	{
		m_timer->start( 1000 );
		m_gameWidget->start();
	}
}

void KJezzball::stopLevel()
{
	if ( m_gameWidget )
	{
		m_gameWidget->stop();
		m_timer->stop();
	}
}

void KJezzball::nextLevel()
{
	stopLevel();
	m_nextLevelTimer->start( 100, TRUE );
}

void KJezzball::switchLevel()
{
	m_game.score += m_level.score;
	ScoreLabel->setText( tr( "Score: %1" ).arg(m_game.score) );

	QString score;
	score.setNum( m_level.score );

	QString level;
	level.setNum( m_game.level );

	QString foo = QString(
			tr("Successfully cleared more than 75%.\n") +
			tr("%1 points: 15 points per life\n").arg(m_level.lifes*15) +
			tr("%1 points: Bonus\n").arg((m_gameWidget->percent()-75)*2*(m_game.level+5)) +
			tr("%1 points: Total score\n").arg(score) +
			tr("On to level %1.\nYou get %2 lives this time!")).arg(m_game.level+1).arg(m_game.level+2);

	QMessageBox::information( this, "Success", foo );

	m_game.level++;

	createLevel( m_game.level );
	startLevel();
}

void KJezzball::keyPressEvent( QKeyEvent *ev )
{
	if ( ev->key() == Key_Space ||
			ev->key() == Key_Up ||
			ev->key() == Key_Down ||
			ev->key() == Key_Right ||
			ev->key() == Key_Left )
	{
		m_gameWidget->changeCursor();
	}
	else
	{
		ev->ignore();
	}
}
