/***************************************************************************
                          UI.h  -  description
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
#ifndef UI_H
#define UI_H

#include <qpe/qpeapplication.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qstring.h>

#include "Picture.h"
#include "MCursor.h"

#include "field.h"
#include "kbill.h"

class UI {
public:
	~UI();
	void restart_timer();
	void kill_timer();

	void initialize(int *argc, char **argv);
	void make_mainwin();
	void make_windows() {};
	void popup_dialog (int dialog);

	void set_cursor (int cursor);
	void load_cursors();
	void graph_init();
	void clear();
	void refresh();
	void draw (Picture picture, int x, int y);
	void draw_centered (Picture picture);
	void draw_line (int x1, int y1, int x2, int y2);
	void draw_str (char *str, int x, int y);

	void set_pausebutton (int action);
	int MainLoop();

	void update_scorebox(int level, int score);
	void update_hsbox(char *str);
private:
	MCursor defaultcursor, downcursor;

	QPEApplication *app;
	QTimer* timer;

	Field* field;
	KBill* main;
	
	QPixmap *pix;
	QPainter paint;

	QString scorestr, highscorestr;
};

#endif
