/***************************************************************************
                          inputbox.h  -  description
                             -------------------
    begin                : Sat Jan 1 2000
    copyright            : (C) 2000 by Jurrien Loonstra
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

#ifndef INPUTBOX_H
#define INPUTBOX_H

#include <qdialog.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include <qstring.h>
#include <qlineedit.h>
class InputBox : public QDialog  {
   Q_OBJECT
public:
	InputBox(QWidget *parent=0, const char *name=0, const char *caption = 0, const char *text=0);
	~InputBox();
	QString getText() const;
private:
	QPushButton *ok, *cancel;
	QLabel *question;
	QLineEdit *input;
};

#endif
