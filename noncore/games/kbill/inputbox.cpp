/***************************************************************************
                          inputbox.cpp  -  description
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

#include "inputbox.h"
#include <qdialog.h>
InputBox::InputBox(QWidget *parent, const char *name, const char *caption, const char *text) : QDialog(parent, name, TRUE) {
// 	setCaption(caption);
// 
// 	question = new QLabel(this);
// 	question->setText(text);
// 	question->setGeometry(10, 10, 240, 50);
// 
// 	input = new QLineEdit(this);
//   input->setGeometry(10, 60, 240, 30);
// 	input->setFocus();
// 	input->setMaxLength(19);
// 
//   ok = new QPushButton( "Ok", this );
//   ok->setGeometry(10, 100, 100,30 );
// 	ok->setDefault(TRUE);
//   connect( ok, SIGNAL(clicked()), SLOT(accept()) );
// 
//   cancel = new QPushButton( "Cancel", this );
//   cancel->setGeometry(150, 100, 100,30 );
//   connect( cancel, SIGNAL(clicked()), SLOT(reject()) );
}

InputBox::~InputBox(){
	delete ok;
	delete cancel;
	delete question;
	delete input;
}

QString InputBox::getText() const{
	return input->text();
}
