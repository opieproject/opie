/***************************************************************************
                          kbill.cpp  -  description
                             -------------------
    begin                : Thu Dec 30 16:55:55 CET 1999
    copyright            : (C) 1999 by Jurrien Loonstra
    email                : j.h.loonstra@st.hanze.nl
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qmessagebox.h>
#include <qmultilinedit.h>
#include <qmenubar.h>
#include <qtextbrowser.h>
#include <qfont.h>
#include <qwidget.h>
#include "kbill.h"
#include "inputbox.h"
#include "helpdialog.h"
#include "objects.h"
#include "Strings.h"

KBill::KBill() : QMainWindow() {
	setCaption("kBill");
	file = new QPopupMenu();
	file->insertItem(("New game"), this, SLOT(NewGame()));
	pauseid = file->insertItem(("Pause game"), this, SLOT(Pause()));
	//these are dissabled until I fix them
	//file->insertItem(("Warp to level..."), this, SLOT(WarpTo()));
	//file->insertItem(("View high scores"), this, SLOT(ViewHighScores()));
	

	help = new QPopupMenu();
	help->insertItem(("Story of kBill"), this, SLOT(Story()));
	help->insertItem(("Rules"), this, SLOT(Rules()));


	menu = new QMenuBar(this);
	menu->insertItem(("&File"), file);
	menu->insertSeparator();
	menu->insertItem(("&Help"), help);

	field = new Field(this);
	//setView(field);
	//setMainWidget(field);
	//setMenu(menu);
}

KBill::~KBill() {
}

Field* KBill::getField() {
	return field;
}

// -----------------------------------------------------------------------

void KBill::Quit() {
 	field->stopTimer();
 	qApp->quit();

}

void KBill::About(){
// 	field->stopTimer();
// 	AboutBox about(this);
// 	about.exec();
// 	field->startTimer();
}

void KBill::NewGame() {
 	field->stopTimer();
// 	if (KMsgBox::yesNo(this, i18n("New Game"), i18n(newgamestr), KMsgBox::QUESTION) == 1)
 		game.start(1);
// 	else
 		field->startTimer();
}

void KBill::Pause() {
 	field->stopTimer();
	QMessageBox::message(("Pause Game"), (pausestr), 0);
 	field->startTimer();
}

void KBill::WarpTo() {
/* 	field->stopTimer();
 	InputBox b(this, 0, "Warp To Level", warpstr);
 	bool status  = b.exec() == 1;
 	field->startTimer();
 	if (status) {
 		bool ok;
		int level = b.getText().toUInt(&ok);
 		if (ok) {
 			field->startTimer();
 			game.warp_to_level(level);
 			return;
 		}
 	}*/
}

void KBill::ViewHighScores() {
	//ui.popup_dialog(Game::HIGHSCORE);
}

void KBill::Story() {
	field->stopTimer();
 	HelpDialog *stryDialog = new HelpDialog(this,"helpdialog",1);
 	QString stryString = "<b>The Story</b><p>Yet again, the fate of the world rests in your hands!  An evil computer hacker, known only by his handle 'Bill', has created the ultimate computer virus.  A virus so powerful that it has the power to transmute an ordinary computer into a toaster oven.  (oooh!) 'Bill' has cloned himself into a billion-jillion micro-Bills.  Their sole purpose is to deliver the nefarious virus, which has been cleverly diguised as a popular operating system. As System Administrator and Exterminator, your job is to keep Bill from succeeding at his task.";
 	stryDialog->setCaption("The story of KBill");
 	stryDialog->TextBrowser1->setText(stryString);
 	stryDialog->resize(200,200);
 	stryDialog->show();
	 field->startTimer();
}

void KBill::Rules() {
 	field->stopTimer();
	HelpDialog *rulesDialog = new HelpDialog(this,"helpdialog",1);
 	rulesDialog->setCaption("The rules of KBill");
	QString rulesStr = "<b>The Rules</b><p>kBill has been painstakingly designed and researched in order to make it as easy to use for the whole family as it is for little Sally. Years - nay - days of beta testing and consulting with the cheapest of human interface designers have resulted in a game that is easy to use, yet nothing at all like a Macintosh.<p><UL><LI>Whack the Bills (click)</LI><LI>Restart the computer (click)</LI><LI>Pick up stolen OSes & return (drag) them to their respective computers</LI><LI>Drag the bucket to extinguish sparks</LI><LI>Scoring is based on total uptime, with bonuses for killing Bills.</LI></UL><P>As for the rest, you can probably work it out for yourself.  We did, so it can't be too hard";
 	rulesDialog->TextBrowser1->setText(rulesStr);
 	rulesDialog->resize(200,200);
 	rulesDialog->show(); 	
	field->startTimer();
}
