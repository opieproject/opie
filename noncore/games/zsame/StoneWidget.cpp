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

#include <stdio.h>
#include <stdlib.h>

#include <qpainter.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qimage.h>
#include <qcursor.h>


#include <qpe/resource.h>

#include <time.h>
#include <assert.h>

#include "StoneWidget.h"



struct StoneSlice {
    QPixmap stone;
};


StoneWidget::StoneWidget( QWidget *parent, int x, int y )
    : QWidget(parent,"StoneWidget"), stonefield(x,y)
{
//    setBackgroundPixmap(QPixmap(locate("wallpaper", "Time-For-Lunch-2.jpg")));
//    QPixmap stonemap(locate("appdata", "stones.png"));

    QPixmap stonemap = Resource::loadPixmap("zsame/stones" );
    assert(!stonemap.isNull());

    slice=0;
    maxslices=30;
    maxcolors=4;

    sizex=x;
    sizey=y;

    stone_width=stonemap.width()/(maxslices+1);
    stone_height=stonemap.height()/maxcolors;

    map = new StoneSlice*[maxcolors];
    QBitmap mask;
    for (int c = 0; c < maxcolors; c++) {
        map[c] = new StoneSlice[maxslices];

        for (int s = 0; s < maxslices; s++) {
            map[c][s].stone.resize(stone_width, stone_height);
            assert(!map[c][s].stone.isNull());
            bitBlt(&map[c][s].stone, 0, 0,
                   &stonemap, stone_width * s,
                   c*stone_height,
                   stone_width,stone_height,CopyROP,false);
            QImage im = map[c][s].stone.convertToImage();
            mask = im.createHeuristicMask();
            map[c][s].stone.setMask(mask);
        }
    }

    field_height=stone_height*sizey;
    field_width=stone_width*sizex;

    setMouseTracking(true);

    // QColor c(115,115,115);
    // setBackgroundColor(c);

    // emit s_sizechanged();
    startTimer( 100 );
    history.setAutoDelete(true);
}

StoneWidget::~StoneWidget() {
    for (int c = 0; c < maxcolors; c++) {
        delete [] map[c];
    }
    delete [] map;

    setMouseTracking(false);
    killTimers();
}

unsigned int
StoneWidget::board() {
    return stonefield.getBoard();
}

int
StoneWidget::score() {
    return stonefield.getScore();
}

int
StoneWidget::marked() {
	return stonefield.getMarked();
}

QSize
StoneWidget::size() {
    return QSize(sizex,sizey);
}

int
StoneWidget::colors() {
    return stonefield.getColors();
}

QSize
StoneWidget::sizeHint () const {
    return QSize(field_width,field_height);
}

void
StoneWidget::newGame(unsigned int board,int colors) {
    stonefield.newGame(board,colors);
    history.clear();
    modified= false;
    emit s_newgame();
    emit s_colors(stonefield.getColors());
    emit s_board(stonefield.getBoard());
}

void
StoneWidget::reset() {
    stonefield.reset();
    history.clear();
    emit s_newgame();
}

void
StoneWidget::unmark() {
    stonefield.unmark();
    emit s_marked(0);
}

bool StoneWidget::undoPossible() const {
    if (stonefield.isGameover()) return false;
    return stonefield.undoPossible();
}

int
StoneWidget::undo(int count) {
    if (stonefield.isGameover()) return 0;

    int ret_val=stonefield.undo(count);

    QPoint p=mapFromGlobal(cursor().pos());
    int x=p.x();
    int y=p.y();
    if (x<0||y<0||x>=field_width||y>=field_height) {
        emit s_score(stonefield.getMarked());
        return ret_val;
    }

    int marked=stonefield.mark(x/stone_width,y/stone_height);
    emit s_marked(marked);
    slice=0;
    emit s_score(stonefield.getScore());
    modified= (stonefield.getScore()>0);
    return ret_val;
}

bool StoneWidget::isGameover() {
    return stonefield.isGameover();
}

bool StoneWidget::hasBonus() {
	return stonefield.gotBonus(); // don't ask me why the names differ... ;-| [hlm]
}

bool StoneWidget::isOriginalBoard() {
	return !modified;
}

void StoneWidget::readProperties(Config *) {
/*    Q_ASSERT(conf);

    history.clear();

    if (!conf->hasKey("Board")||
        !conf->hasKey("Colors")||
        !conf->hasKey("Stones")) {
        return;
    }
    newGame(conf->readNumEntry("Board"),conf->readNumEntry("Colors"));

    QStrList list;
    conf->readListEntry("Stones",list);

    for (const char *item=list.first();item;item=list.next()) {
        int x=-1,y=-1;
        if (sscanf(item,"%02X%02X",&x,&y)!=2) break;
        history.append(new QPoint(x,y));
        stonefield.remove(x,y);
    }
*/
}


void
StoneWidget::saveProperties(Config *) {
/*
    Q_ASSERT(conf);

    QStrList list(true);
    QString tmp;

    for (QPoint *item=history.first();item;item=history.next()) {
        tmp.sprintf("%02X%02X",item->x(),item->y());
        list.append(tmp.ascii());
    }

    conf->writeEntry("Stones",list);
    conf->writeEntry("Board",stonefield.getBoard());
    conf->writeEntry("Colors",stonefield.getColors());
*/
}

void
StoneWidget::timerEvent( QTimerEvent * ) {
    QPoint p=mapFromGlobal(cursor().pos());
    int x=p.x();
    int y=p.y();
    if (x<0||y<0||x>=field_width||y>=field_height)
        stonefield.unmark();
    slice=(slice+1)%maxslices;
    paintEvent(0);
}

void
StoneWidget::paintEvent( QPaintEvent *e ) {

    Stone *stone=stonefield.getField();

    for (int y=0;y<sizey;y++) {
        int cy = y * stone_height;

        for (int x=0;x<sizex;x++) {
            int cx = stone_width * x;

            bool redraw=stone->marked||stone->changed;

            if (!redraw&&e) {
                QRect r(cx,cy,stone_width,stone_height);
                redraw=r.intersects(e->rect());
            }
            if (redraw) {
                stone->changed=false;
                if (stone->color) {

                    int tslice = stone->marked?slice:0;
                    bitBlt(this,cx,cy,
                           &map[stone->color-1][tslice].stone,
                           0, 0,
                           stone_width,stone_height,CopyROP,FALSE);

                } else {
                    erase(cx, cy, stone_width, stone_height);
                }
            }
            stone++;  // naechster Stein.
        }
    }
}

void
StoneWidget::mousePressEvent ( QMouseEvent *e) {

    if (stonefield.isGameover()) return;

    int x=e->pos().x();
    int y=e->pos().y();
    if (x<0||y<0||x>=field_width||y>=field_height) return;

    int sx=x/stone_width;
    int sy=y/stone_height;

    int mar =stonefield.mark(sx, sy);

    if ( mar != -1 ) {
        myMoveEvent(e);
        return;
    }


    if (stonefield.remove(sx, sy)) {
        history.append(new QPoint(sx, sy));

        emit s_remove(sx, sy);

        stonefield.mark(sx,sy);
        emit s_marked(stonefield.getMarked());
		modified= true;

        emit s_score(stonefield.getScore());
        if (stonefield.isGameover()) emit s_gameover();
    }
}

void
StoneWidget::myMoveEvent ( QMouseEvent *e)
{
    return;

    if (stonefield.isGameover()) {
        stonefield.unmark();
        emit s_marked(0);
        return;
    }

    int x=e->pos().x();
    int y=e->pos().y();
    if (x<0||y<0||x>=field_width||y>=field_height) return;

    int marked=stonefield.mark(x/stone_width,y/stone_height);
    if (marked>=0) {
        emit s_marked(marked);
        slice=0;
    }
}








