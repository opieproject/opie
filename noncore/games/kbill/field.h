/***************************************************************************
                          field.h  -  description
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

#ifndef FIELD_H
#define FIELD_H

#include <qwidget.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qtimer.h>

class Field : public QWidget  {
   Q_OBJECT
public: 
	Field(QWidget *parent=0, const char *name=0);
	~Field();
	void stopTimer();
	void startTimer();
	void setPixmap(QPixmap *pix);
protected:
  void mouseReleaseEvent(QMouseEvent *e);
  void mousePressEvent(QMouseEvent *e);
	void enterEvent(QEvent *);
	void leaveEvent(QEvent *);
	void paintEvent(QPaintEvent *);
public slots:
  void Timer();
private:
  QTimer* timer;
	QPixmap *pix;
  bool playing;
};

#endif
