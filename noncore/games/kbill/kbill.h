/***************************************************************************
                          kbill.h  -  description
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

#ifndef KBILL_H
#define KBILL_H



#include <qmainwindow.h>
#include <qwidget.h>
#include <qpopupmenu.h>

#include "field.h"

class KBill : public QMainWindow
{
  Q_OBJECT 
  public:
    KBill();
    ~KBill();
		Field* getField();
	private:
		QMenuBar *menu;
		QPopupMenu *file, *help;
  	Field *field;
		int pauseid;
	protected slots:
  	void Quit();
  	void About();
  	void NewGame();
	  void Pause();
	  void WarpTo();
		void Story();
		void Rules();
		void ViewHighScores();

	friend class UI;
};

#endif
