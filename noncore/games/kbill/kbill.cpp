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
#include <qmenubar.h>
#include "kbill.h"
#include "inputbox.h"

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
	file->insertItem(("Quit game"), this, SLOT(Quit()));

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
 	QMessageBox::message( ("Story"), (storystr), 0);
 	field->startTimer();
}

void KBill::Rules() {
 	field->stopTimer();
 	QMessageBox::message(("Rules"), (rulesstr), 0);
 	field->startTimer();
}
