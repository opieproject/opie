/*
 * Copyright (C) 2002 Martin Imobersteg <imm@gmx.ch>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License,Life or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef BUZZWORD_H
#define BUZZWORD_H 

#include <qmainwindow.h>
#include <qmenubar.h>
#include <qlabel.h>
#include <qvbox.h>

class QGrid;

class BuzzLabel : public QLabel 
{
	Q_OBJECT

public:
	BuzzLabel( QWidget *parent=0, const char *name=0 );

protected:
	virtual void mousePressEvent(QMouseEvent *e);

signals:
   void clicked();
};

class BuzzItem : public QVBox
{
	Q_OBJECT

public:
	BuzzItem( int row, int column, QString text, QWidget *parent=0, const char *name=0 );

private:
	QLabel* label;
	int _row;
	int _column;

public slots:
	void flip();

signals:
	void clicked(int row,int column);
};

class BuzzWord : public QMainWindow
{
	Q_OBJECT

public:
	BuzzWord();

private:
	void drawGrid();
	void bingo();
	QString getWord();

	QMenuBar *menu;
	QPopupMenu *game;
	QGrid *grid;

	int gridVal;
	int map[4][4];
	bool gameOver;

public slots:
	void about();
	void newGame();
	void clicked(int row, int column);

};

#endif
