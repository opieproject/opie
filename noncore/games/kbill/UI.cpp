/***************************************************************************
                          UI.cc  -  description
                             -------------------
    begin                : Thu Dec 30 1999
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
#include "kbill.h"
#include "objects.h"
#include "Strings.h"
#ifdef KDEVER
#include <kapplication.h>
#endif
#include <qpixmap.h>
#include <qmessagebox.h>
#include <qnamespace.h>

#include "inputbox.h"

/**************************/
/* Timer control routines */
/**************************/

UI::~UI() {
	paint.end();
	delete pix;
}

void UI::restart_timer() {
	field->startTimer();
}

void UI::kill_timer() {
	field->stopTimer();
}

/*******************/
/* Window routines */
/*******************/

void UI::initialize(int *argc, char **argv) {
	 #ifdef KDEVER
	 app = new KApplication(*argc, argv, "kbill");
	 #endif
	 app = new QPEApplication(*argc, argv);
}

void UI::graph_init() {
	pix = new QPixmap(Game::scrwidth, Game::scrheight);
	paint.begin(pix, field);
	paint.setPen(QPen(Qt::black, 3));
}

void UI::make_mainwin() {
	main = new KBill();
	app->showMainWidget(main,true);
	main->showMaximized();
	field = main->getField();
}

void UI::popup_dialog (int dialog) {
	kill_timer();
	switch (dialog) {
	case Game::ENDGAME:
			QMessageBox::message(("Endgame"), QT_TR_NOOP(endgamestr));
			break;
	case Game::HIGHSCORE:
		//	QMessageBox::message(("HighScore"), highscorestr);
			break;
	case Game::ENTERNAME: {
			InputBox b(main, 0, ("Enter Name"), QT_TR_NOOP(enternamestr));
			bool state = b.exec() == 2;
		  char str[20], *nl;
	    strcpy(str, b.getText());
	    if (!str[0] || state)
				strcpy(str, "Anonymous");
	    else if ((nl = strchr(str, '\n')))
				*nl = '\0';
	    if (strlen(str) > 20)
				str[20] = 0;  /* truncate string if too long */
//	    scores.recalc(str);
			}
	    break;
	case Game::SCORE:
			QMessageBox::message(("Score"), scorestr);
			break;
	}
	restart_timer();
}

/*********************/
/* Graphics routines */
/*********************/

void UI::set_cursor(int cursor) {
	QCursor *cur;
	switch (cursor) {
	case Game::BUCKETC:
		cur = bucket.cursor.cursor;
		break;
	case Game::DOWNC:
		cur = downcursor.cursor;
		break;
	case Game::DEFAULTC:
		cur = defaultcursor.cursor;
		break;
	default:
		cur = OS.cursor[cursor].cursor;
	}
	field->setCursor(*cur);
}

void UI::load_cursors() {
	defaultcursor.load("hand_up", MCursor::SEP_MASK);
	field->setCursor(*defaultcursor.cursor);
	downcursor.load("hand_down", MCursor::SEP_MASK);
}

void UI::clear() {
	paint.eraseRect(0, 0, field->width(), field->height());
}

void UI::refresh() {
	paint.flush();
	field->setPixmap(pix);
	field->repaint(FALSE);
}

void UI::draw (Picture pict, int x, int y) {
	paint.drawPixmap(x, y, *pict.pix);
}

void UI::draw_centered (Picture pict) {
	draw(pict, (field->width() - pict.width) / 2, (field->height() - pict.height) / 2);
}

void UI::draw_line(int x1, int y1, int x2, int y2) {
	paint.drawLine(x1, y1, x2, y2);

}

void UI::draw_str(char *str, int x, int y) {
	paint.drawText(x, y, str);
}


/******************/
/* Other routines */
/******************/

void UI::set_pausebutton (int action) {
	main->file->setItemEnabled(main->pauseid, action);
}


int UI::MainLoop() {
  return app->exec();
}

void UI::update_hsbox(char *str) {
	highscorestr = str;	
}

void UI::update_scorebox(int level, int score) {
	scorestr.sprintf ("%s %d:\n%s: %d", QT_TR_NOOP("After Level"), level, QT_TR_NOOP("Your score"), score);
}
