/***************************************************************************
                          field.cpp  -  description
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

#include <qpainter.h>

#include "objects.h"

#include "field.h"

Field::Field(QWidget *parent, const char *name ) : QWidget(parent,name) {
	setFixedSize(game.scrwidth, game.scrheight);
	setBackgroundColor(white);
	timer = new QTimer(this);

	playing = false;

	connect(timer, SIGNAL(timeout()), SLOT(Timer()));
}

Field::~Field(){
	delete timer;
}

void Field::setPixmap(QPixmap *pix) {
	this->pix = pix;
}

// --------------------------------------------------------

void Field::mousePressEvent(QMouseEvent *e){
	game.button_press(e->x(), e->y());
}

void Field::mouseReleaseEvent(QMouseEvent *e){
	game.button_release(e->x(), e->y());
}

void Field::enterEvent(QEvent *) {
	if (playing && !timer->isActive()) {
		playing = true;
		timer->start(250, FALSE);
	}
}

void Field::leaveEvent(QEvent *) {
	if (timer->isActive() && playing) {
		playing = true;
		timer->stop();
	}
}

void Field::stopTimer() {
	playing = false;
	if (timer->isActive())
		timer->stop();
}

void Field::startTimer() {
	playing = true;
	if (!timer->isActive())
		timer->start(250, FALSE);
}

void Field::Timer(){
	game.update();
}

void Field::paintEvent(QPaintEvent *) {
	bitBlt(this, 0, 0, pix);
}			