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

#include "StoneField.h"
#include <assert.h>

StoneFieldState::StoneFieldState(const StoneField &stonefield)
{
    field=new unsigned char[stonefield.maxstone];
    for (int i=0;i<stonefield.maxstone;i++)
        field[i]=stonefield.field[i].color;

    colors=stonefield.colors;
    board=stonefield.board;
    score=stonefield.score;
    gameover=stonefield.gameover;
}

StoneFieldState::~StoneFieldState() {
    delete[] field;
}

void
StoneFieldState::restore(StoneField &stonefield) const {
    for (int i=0;i<stonefield.maxstone;i++) {
        stonefield.field[i].color=field[i];
        stonefield.field[i].changed=true;
        stonefield.field[i].marked=false;
    }

    stonefield.colors=colors;
    stonefield.board=board;
    stonefield.score=score;
    stonefield.marked=0;
    stonefield.gameover=gameover;
}

StoneField::StoneField(int width, int height,
                       int colors, unsigned int board,
                       bool undoenabled)
{
//    Q_ASSERT(width>0);
//    Q_ASSERT(height>0);

    if (undoenabled) undolist=new QList<StoneFieldState>;
    else undolist=0;

    sizex=width;
    sizey=height;
    maxstone=sizex*sizey;
    field=new Stone[maxstone];
    newGame(board,colors);
    m_gotBonus= false;
}

StoneField::~StoneField() {
    delete[] field;
    delete undolist;
//    kdDebug() << "~StoneField\n" << endl;
}

int
StoneField::width() const {
    return sizex;
}

int
StoneField::height() const {
    return sizey;
}

void
StoneField::newGame(unsigned int board,int colors) {
//    kdDebug() << "StoneField::newgame board "
//              << board << " colors " << colors << endl;
    if (colors<1) colors=3;
    if (colors>7) colors=7;
    this->colors=colors;
    this->board=board;
    reset();
}

void
StoneField::reset() {
    random.setSeed(board);

    Stone *stone=field;
    for (int i=0;i<maxstone;i++,stone++) {
        stone->color=1+random.getLong(colors);
        stone->marked=false;
        stone->changed=true;
    }

    gameover=-1;
    score=0;
    marked=0;

    if (undolist) {
        undolist->setAutoDelete(true);
        undolist->clear();
    }


    int c[7];
    int j;
    for (j=0;j<7;j++) c[j]=0;

    for (j=0,stone=field;j<maxstone;j++,stone++) {
        c[stone->color]++;
    }
//    kdDebug() << "red    " << c[1] << endl;
//    kdDebug() << "blue   " << c[2] << endl;
//    kdDebug() << "yellow " << c[3] << endl;
//    kdDebug() << "green  " << c[4] << endl;

}

int
StoneField::map(int x,int y) {
	assert (!(x<0||y<0||x>=sizex||y>=sizey));
	return x+y*sizex;
}

int
StoneField::mark(int x,int y,bool force) {
	int index=map(x,y);

	if (index<0) {
		unmark();
		return 0;
	}

	if (field[index].marked) return -1;
	unmark();

	mark(index,field[index].color);

	if (marked==1&&!force) {
		field[index].marked=false;
		marked=0;
	}
	return marked;
}

void
StoneField::mark(int index,unsigned char color) {
	if ( index<0 || index>=maxstone ) return;

	Stone &stone=field[index];

	if (stone.marked) return;

	if (!stone.color || stone.color!=color) return;

	stone.changed=true;
	stone.marked=true;
	marked++;

	// mark left
	if ((index%sizex)!=0) mark(index-1,color);
	// mark right
	if (((index+1)%sizex)!=0) mark(index+1,color);
	// mark upward
	if (index>=sizex) mark(index-sizex,color);
	// mark downward
	if (index<(sizex-1)*sizey) mark(index+sizex,color);
}

void
StoneField::unmark() {
	if (!marked) return;

	Stone *stone=field;
	for (int i=0;i<maxstone;i++,stone++) {
		stone->marked=false;
		stone->changed=true;
	}
	marked=0;
}

int
StoneField::remove(int x,int y,bool force) {
	int index=map(x,y);

	if (index<0) return 0;

	if (!field[index].marked) {
		mark(x,y,force);
	}

	if (!marked) return 0;

	// remove a single stone??
	if (marked==1&&!force) return 0;

	// add current field to undolist
	if (undolist)
		undolist->append(new StoneFieldState(*this));

	// increase score
	if (marked>2)
		score+=(marked-2)*(marked-2);

	// remove marked stones
	Stone *stone=field;
	for (int i=0;i<maxstone;i++,stone++) {
		if (stone->marked) {
			stone->color=0;
			stone->changed=true;
			stone->marked=false;
		}
	}
	int removed=marked;
	marked=0;

	for (int col=0;col<sizex;col++) {
		int i1=col+maxstone-sizex;
		while ( i1>=0 && field[i1].color ) i1-=sizex;
		int i2=i1;
		while (i2>=0) {
			while ( i2>=0 && !field[i2].color ) i2-=sizex;
			while ( i2>=0 && field[i2].color ) {
				field[i1].color=field[i2].color;
				field[i1].changed=true;
				field[i2].color=0;
				field[i2].changed=true;
				i1-=sizex;
				i2-=sizex;
			}
		}
	}

	// find the last column that has something
	int lastcol = sizex;
	while (lastcol > 0 && !field[map(lastcol-1, sizey-1)].color) {
		lastcol--;
	}

	for (int col=0;col<lastcol-1;) {
		bool empty = true;
		for (int row = 0; row < sizey; row++)
			if (field[map(col, row)].color) {
				empty = false;
				break;
			}
		if (!empty) {
			col++;
			continue;
		}
		int nextfullcol = col + 1;
		while (nextfullcol < sizex &&
			   !field[map(nextfullcol, sizey - 1)].color)
			nextfullcol++;

		if (nextfullcol > sizex - 1)
			break; // we're ready

		for (int row=0; row < sizey; row++) {
			int source = map(nextfullcol, row);
			int dest = map(col, row);
			field[dest].color=field[source].color;
			field[dest].changed=true;
			field[source].color=0;
			field[source].changed=true;
		}
	}

	// add a bonus, if field is empty
	if (!field[map(0, sizey-1)].color) {
		score+=1000;
		m_gotBonus= true;
	}

	// gameover is undefined
	gameover=-1;
	return removed;
}

bool StoneField::undoPossible() const {
	return !(!undolist||undolist->isEmpty());
}

int
StoneField::undo(int count) {
	if (!undoPossible())
		return 0;
	if (count <= 0)
		return 0;
     int undocount=1;
     StoneFieldState *state=0;
     undolist->setAutoDelete(true);
     while (--count>0) {
	  if (undolist->count()==1) break;
	  undolist->removeLast();
	  undocount++;
     }
     state=undolist->getLast();
//     Q_ASSERT(state);
     state->restore(*this);
     undolist->removeLast();
     return undocount;
}

bool
StoneField::isGameover() const {
     register int i=maxstone-1;;
     register unsigned char color;

     if (gameover>=0) return (bool)gameover;
     // kdDebug() << "-->gameover" << endl;

     while (i>=0) {
	  // kdDebug() << i << " " << field[i].color << endl;
	  // ignore empty fields
	  while ( i>=0 && field[i].color==0 ) i--;
	  // Wenn Stein gefunden,
	  // dann die Nachbarn auf gleiche Farbe pruefen.
	  while (  i>=0 && (color=field[i].color) ) {
	       // check left
	       if ( (i%sizex)!=0 && field[i-1].color==color)
		    goto check_gameover;
	       // check upward
	       if ( i>=sizex && field[i-sizex].color==color)
		    goto check_gameover;
	       i--;
	  }
     }
 check_gameover:
     gameover=(i<0);
     //  kdDebug() << "<--gameover" << endl;
     return (bool)gameover;
}

bool StoneField::gotBonus() const {
	return m_gotBonus;
}

int
StoneField::getBoard() const {
     return board;
}

int
StoneField::getScore() const {
     return score;
}

int
StoneField::getColors() const {
     return colors;
}

int
StoneField::getMarked() const {
     return marked;
}

int
StoneField::getFieldSize() const {
     return maxstone;
}

struct Stone *
StoneField::getField() const {
     return field;
}







