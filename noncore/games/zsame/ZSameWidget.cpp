/* Yo Emacs, this is -*- C++ -*- */
/*
 *   ksame 0.4 - simple Game
 *   Copyright (C) 1997,1998  Marcus Kreutzberger
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <stdio.h>

#include <opie2/oapplicationfactory.h>

#include <qpe/resource.h>

#include <qtoolbar.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qaction.h>
#include <qmessagebox.h>

#include <kapplication.h>

#include "ZSameWidget.h"

static int default_colors=3;

#define i18n tr


OPIE_EXPORT_APP( OApplicationFactory<ZSameWidget> )



ZSameWidget::ZSameWidget( QWidget* parent, const char* name,  WFlags fl )
    : QMainWindow( parent, name, fl )
{
    setCaption(tr("ZSame"));

    setToolBarsMovable( false );
    QToolBar* con = new QToolBar( this );
    con->setHorizontalStretchable( true );
    QMenuBar* mb = new QMenuBar( con );
    QToolBar* tb = new QToolBar( this );

    QPopupMenu* fileMenu = new QPopupMenu( this );

    QAction* a = new QAction(tr("New Game"), Resource::loadIconSet("new") ,
                             QString::null, 0, this, "new_icon");
    a->addTo( fileMenu );
    a->addTo( tb );
    connect(a, SIGNAL(activated()), this, SLOT(m_new()));

    a = new QAction(tr("Restart This Board"), Resource::loadIconSet("redo"),
                              QString::null, 0, this, "restart_board" );
    a->addTo( fileMenu );
    connect( a, SIGNAL(activated()), this, SLOT(m_restart()));
    restart = a;

    a = new QAction( tr("Undo"), Resource::loadIconSet("undo"),
                     QString::null, 0, this, "undo_action" );
    a->addTo( fileMenu );
    a->addTo( tb );
    connect( a, SIGNAL(activated()), this, SLOT(m_undo()));

    a = new QAction(tr("Quit"), Resource::loadIconSet("quit_icon"),
                              QString::null, 0, this, "quit_action");
    a->addTo( fileMenu );
    a->addTo( tb );
    connect(a, SIGNAL(activated()), this, SLOT(m_quit()));

    mb->insertItem(tr("Game" ), fileMenu );

    int foo[2];
    desktop_widget(foo);
    stone = new StoneWidget(this,foo[0],foo[1]);

    connect( stone, SIGNAL(s_gameover()), this, SLOT(gameover()));

    connect( stone, SIGNAL(s_colors(int)), this, SLOT(setColors(int)));
    connect( stone, SIGNAL(s_board(int)), this, SLOT(setBoard(int)));
    connect( stone, SIGNAL(s_marked(int)), this, SLOT(setMarked(int)));
    connect( stone, SIGNAL(s_score(int)), this, SLOT(setScore(int)));
    connect( stone, SIGNAL(s_remove(int,int)), this, SLOT(stonesRemoved(int,int)));

    connect(stone, SIGNAL(s_sizechanged()), this, SLOT(sizeChanged()));

    sizeChanged();
    setCentralWidget(stone);


    setScore(0);
}

ZSameWidget::~ZSameWidget() {

}

void ZSameWidget::readProperties(Config *conf) {
/*
  Q_ASSERT(conf);
  stone->readProperties(conf);
*/
}

void ZSameWidget::saveProperties(Config *conf) {
/*
  Q_ASSERT(conf);
  stone->saveProperties(conf);
  conf->sync();
*/
}

void ZSameWidget::sizeChanged() {
//	stone->setFixedSize(stone->sizeHint());
}

void ZSameWidget::newGame(unsigned int board,int colors) {
	while (board>=1000000) board-=1000000;
	// kdDebug() << "newgame board " << board << " colors " << colors << endl;
	stone->newGame(board,colors);
	setScore(0);
}

bool ZSameWidget::confirmAbort() {
    return stone->isGameover() ||
        stone->isOriginalBoard() ||
        (QMessageBox::warning(this, i18n("Resign"), i18n("<qt>Do you want to resign?</qt>"),
                              QMessageBox::Yes,
                              QMessageBox::No|QMessageBox::Default|QMessageBox::Escape, 0) == QMessageBox::Yes );
}

void ZSameWidget::m_new() {
    if (confirmAbort())
        newGame(_random(),default_colors);

}

void ZSameWidget::m_restart() {
	if (confirmAbort())
		newGame(stone->board(),default_colors);
}

void ZSameWidget::m_load() {
//  kdDebug() << "menu load not supported" << endl;
}

void ZSameWidget::m_save() {
//  kdDebug() << "menu save not supported" << endl;
}

void ZSameWidget::m_undo() {
//	Q_ASSERT(stone);
	stone->undo();
}


void ZSameWidget::m_showhs() {
/*  Q_ASSERT(stone);
  stone->unmark();
  KScoreDialog d(KScoreDialog::Name | KScoreDialog::Score, this);
  d.addField(Board, i18n("Board"), "Board");
  d.exec();
*/
}

void ZSameWidget::m_quit() {
//  Q_ASSERT(stone);
  stone->unmark();
  qApp->quit();
//  delete this;
}

void ZSameWidget::m_tglboard() {
//	kdDebug() << "toggled" << endl;
}


void ZSameWidget::setColors(int colors) {
//	status->changeItem(i18n("%1 Colors").arg(colors),1);
}

void ZSameWidget::setBoard(int board) {
//	status->changeItem(i18n("Board: %1").arg(board, 6), 2);
}

void ZSameWidget::setMarked(int m) {
//  status->changeItem(i18n("Marked: %1").arg(m, 6),3);
}

void ZSameWidget::stonesRemoved(int,int) {
//	KNotifyClient::event("stones removed",
//	   i18n("%1 stones removed.").arg(stone->marked()));
}

void ZSameWidget::setScore(int score) {
//  status->changeItem(i18n("Score: %1").arg(score, 6),4);
//  undo->setEnabled(stone->undoPossible());
//  restart->setEnabled(!stone->isOriginalBoard());
}

void ZSameWidget::gameover() {
//  kdDebug() << "GameOver" << endl;
  if (stone->hasBonus()) {
	  QMessageBox::information(this,i18n("Game won"),
		  i18n("<qt>You even removed the last stone, great job! "
			   "This gave you a score of %1 in total.</qt>").arg(stone->score()));
  } else {
	  QMessageBox::information(this,i18n("Game over"),
		  i18n("<qt>There are no more removeable stones. "
			   "You got a score of %1 in total.</qt>").arg(stone->score()));
  }
  stone->unmark();
}

void ZSameWidget::desktop_widget(int *f)const{

    QWidget* wid = QApplication::desktop();
    /*  width > height landscape mode */
    if ( wid->width() > wid->height() ) {
        f[0]=15;
        f[1]=9;
    }
    /* normal */
    else{
        f[0]=12;
        f[1]=13;
    }
}


