/* Yo Emacs, this is -*- C++ -*- */
/*
 *   ksame 0.4 - simple Game
 *   Copyright (C) 1997,1998  Marcus Kreutzberger
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef _STONEFIELD
#define _STONEFIELD

#include <krandomsequence.h>
#include <qlist.h>

struct Stone {
     unsigned char color;
     bool changed;
     bool marked;
};

class StoneField;
class StoneWidget;

class StoneFieldState {
private:
     unsigned char *field;

     int colors;
     unsigned int board;
     unsigned int score;
	int gameover;

public:
     StoneFieldState(const StoneField &stonefield);
     ~StoneFieldState();
     void restore(StoneField &stonefield) const;
};


class StoneField {
	friend class StoneFieldState;
	friend class StoneWidget;
private:

	int sizex;
	int sizey;
	int maxstone;

	struct Stone *field;

	int colors;
	unsigned int board;
	unsigned int score;
	mutable int gameover;
	bool m_gotBonus;
	int marked;

        KRandomSequence random;
	QList<StoneFieldState> *undolist;
public:
	StoneField(int width=15,int height=10,
			   int colors=3,unsigned int board=0,
			   bool undoenabled=true);
	~StoneField();

	int width() const;
	int height() const;

	void newGame(unsigned int board,int colors);

	void reset();


	int mark(int x,int y,bool force=false);
	void unmark();

	int remove(int x,int y,bool force=false);

	int undo(int count=1);

	bool isGameover() const;
	bool gotBonus() const;
	bool undoPossible() const;
	int getBoard() const;
	int getScore() const;
	int getColors() const;
	int getMarked() const;

protected:
	int getFieldSize() const;
	struct Stone *getField() const;

	int map(int x,int y);
	void mark(int index,unsigned char color);
};

#endif



