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

#include <qwidget.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qvbox.h>
#include <qpopupmenu.h>

#include <kapplication.h>
#include <kiconloader.h>
#include <kdialogbase.h>
#include <kscoredialog.h>
#include <kstatusbar.h>
#include <knuminput.h>
#include <klocale.h>
#include <knotifyclient.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kmenubar.h>
#include <kconfig.h>
#include <kstdaction.h>
#include <kaction.h>
#include <kdebug.h>
#include <kkeydialog.h>

#include "KSameWidget.h"
#include "StoneWidget.h"
#include "version.h"

static int default_colors=3;

#define Board 	KScoreDialog::Custom1

KSameWidget::KSameWidget() : KMainWindow(0)
{
        setCaption("");
	KStdAction::openNew(this, SLOT(m_new()), actionCollection(), "game_new");
	restart = new KAction(i18n("&Restart This Board"), CTRL+Key_R, this,
						  SLOT(m_restart()), actionCollection(), "game_restart");
       // KStdAction::open(this, SLOT(m_load()), actionCollection(), "game_load");
       // KStdAction::save(this, SLOT(m_save()), actionCollection(), "game_save");
	new KAction(i18n("S&how Highscore"), CTRL+Key_H, this,
				SLOT(m_showhs()), actionCollection(), "game_highscores");
	KStdAction::quit(this, SLOT(m_quit()), actionCollection(), "game_quit");
        KStdAction::keyBindings( this, SLOT( slotConfigureKeys() ), actionCollection() );
	undo = KStdAction::undo(this, SLOT(m_undo()),
							actionCollection(), "edit_undo");

	random = new KToggleAction(i18n("&Random Board"), 0, this,
							   SLOT(m_tglboard()), actionCollection(),
							   "random_board");

	status=statusBar();
	status->insertItem(i18n("Colors: XX"),1,1);
	status->insertItem(i18n("Board: XXXXXX"),2,1);
	status->insertItem(i18n("Marked: XXXXXX"),3,1);
	status->insertItem(i18n("Score: XXXXXX"),4,1);

	stone = new StoneWidget(this,15,10);

	connect( stone, SIGNAL(s_gameover()), this, SLOT(gameover()));

	connect( stone, SIGNAL(s_colors(int)), this, SLOT(setColors(int)));
	connect( stone, SIGNAL(s_board(int)), this, SLOT(setBoard(int)));
	connect( stone, SIGNAL(s_marked(int)), this, SLOT(setMarked(int)));
	connect( stone, SIGNAL(s_score(int)), this, SLOT(setScore(int)));
	connect( stone, SIGNAL(s_remove(int,int)), this, SLOT(stonesRemoved(int,int)));

	connect(stone, SIGNAL(s_sizechanged()), this, SLOT(sizeChanged()));

	sizeChanged();
	setCentralWidget(stone);

	createGUI();

	random->setChecked(true);
	setScore(0);

	if (!kapp->isRestored()) newGame(kapp->random(),default_colors);
	kdDebug() << "test" << endl;
}

KSameWidget::~KSameWidget() {
}

void KSameWidget::slotConfigureKeys()
{
  KKeyDialog::configure( actionCollection(), this );
}

void KSameWidget::readProperties(KConfig *conf) {
  Q_ASSERT(conf);
  stone->readProperties(conf);
}

void KSameWidget::saveProperties(KConfig *conf) {
  Q_ASSERT(conf);
  stone->saveProperties(conf);
  conf->sync();
}

void KSameWidget::sizeChanged() {
	stone->setFixedSize(stone->sizeHint());
}

void KSameWidget::newGame(unsigned int board,int colors) {
	while (board>=1000000) board-=1000000;
	// kdDebug() << "newgame board " << board << " colors " << colors << endl;
	stone->newGame(board,colors);
	setScore(0);
}

bool KSameWidget::confirmAbort() {
	return stone->isGameover() ||
		stone->isOriginalBoard() ||
		(KMessageBox::questionYesNo(this, i18n("Do you want to resign?"),
									i18n("New Game")) == KMessageBox::Yes);
}

void KSameWidget::m_new() {
	if (random->isChecked()) {
		if (confirmAbort())
			newGame(kapp->random(),default_colors);
	} else {
		KDialogBase dlg(this, "boardchooser", true,
		                i18n("Select Board"),
		                KDialogBase::Ok | KDialogBase::Cancel,
		                KDialogBase::Ok);

		QVBox *page = dlg.makeVBoxMainWidget();

		KIntNumInput bno(0, page);
		bno.setRange(0, 1000000, 1);
		bno.setLabel(i18n("Select a board:"));
		bno.setFocus();
		bno.setFixedSize(bno.sizeHint());
		bno.setValue(stone->board());

		if (dlg.exec()) newGame(bno.value(),default_colors);
	}
}

void KSameWidget::m_restart() {
	if (confirmAbort())
		newGame(stone->board(),default_colors);
}

void KSameWidget::m_load() {
  kdDebug() << "menu load not supported" << endl;
}

void KSameWidget::m_save() {
  kdDebug() << "menu save not supported" << endl;
}

void KSameWidget::m_undo() {
	Q_ASSERT(stone);
	stone->undo();
}


void KSameWidget::m_showhs() {
  Q_ASSERT(stone);
  stone->unmark();
  KScoreDialog d(KScoreDialog::Name | KScoreDialog::Score, this);
  d.addField(Board, i18n("Board"), "Board");
  d.exec();
}

void KSameWidget::m_quit() {
  Q_ASSERT(stone);
  stone->unmark();
  kapp->quit();
  delete this;
}

void KSameWidget::m_tglboard() {
	kdDebug() << "toggled" << endl;
}


void KSameWidget::setColors(int colors) {
	status->changeItem(i18n("%1 Colors").arg(colors),1);
}

void KSameWidget::setBoard(int board) {
	status->changeItem(i18n("Board: %1").arg(board, 6), 2);
}

void KSameWidget::setMarked(int m) {
  status->changeItem(i18n("Marked: %1").arg(m, 6),3);
}

void KSameWidget::stonesRemoved(int,int) {
	KNotifyClient::event("stones removed",
	   i18n("%1 stones removed.").arg(stone->marked()));
}

void KSameWidget::setScore(int score) {
  status->changeItem(i18n("Score: %1").arg(score, 6),4);
  undo->setEnabled(stone->undoPossible());
  restart->setEnabled(!stone->isOriginalBoard());
}

void KSameWidget::gameover() {
  kdDebug() << "GameOver" << endl;
  if (stone->hasBonus()) {
	  KNotifyClient::event("game won",
		  i18n("You even removed the last stone, great job! "
			   "This gave you a score of %1 in total.").arg(stone->score()));
  } else {
	  KNotifyClient::event("game over",
		  i18n("There are no more removeable stones. "
			   "You got a score of %1 in total.").arg(stone->score()));
  }
  stone->unmark();
  KScoreDialog d(KScoreDialog::Name | KScoreDialog::Score, this);
  d.addField(Board, i18n("Board"), "Board");

  KScoreDialog::FieldInfo scoreInfo;
  scoreInfo[Board].setNum(stone->board());

  if (d.addScore(stone->score(), scoreInfo))
    d.exec();
}

#include "KSameWidget.moc"
