/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef MINDBREAKER_H
#define MINDBREAKER_H

#include <qwidget.h>
#include <qmainwindow.h>
#include <qimage.h>
#include <qvector.h>
#include <qcanvas.h>
#include <qlabel.h>

static const int panel_height = 26;
static const int panel_width = 180;

static const int title_height = 25;
static const int title_width = 180;

static const int bin_margin = 10;
static const int peg_size = 20;
static const int answerpeg_size = 13;

static const int first_peg_x_diff = 21;
static const int first_peg_y_diff = ((panel_height - peg_size) >> 1);
static const int peg_spacing = 30;

static const int answerpegx = 152;
static const int answerpegy = 2;
static const int answerpeg_diff = 9;

static const int board_height = (title_height + (panel_height * 9));
static const int board_width = (panel_width + (bin_margin * 2) + peg_size);

class Peg;
class QToolButton;

class MindBreakerBoard : public QCanvasView // QWidget
{
    Q_OBJECT
public:
    MindBreakerBoard(QCanvas &c, QWidget *parent=0, const char *name=0, int wFlags=0 );
    ~MindBreakerBoard();

    void getScore(int *, int *);
signals:
    void scoreChanged(int, int);

public slots:
    void clear();
    void resetScore();

protected:
    void contentsMousePressEvent(QMouseEvent *);
    void contentsMouseMoveEvent(QMouseEvent *);
    void contentsMouseReleaseEvent(QMouseEvent *);
    void resizeEvent(QResizeEvent *);

private:
    void drawBackground();
    void checkGuess();
    void checkScores();
    void placeGuessPeg(int pos, int pegId);

    QImage panelImage;
    QImage titleImage;

    Peg *moving;
    Peg *current_highlight;
    QPoint moving_pos;

    // the game stuff 
    int answer[4];
    int current_guess[4];
    int past_guesses[4*9];
    int current_go;

    int null_press;
    QPoint null_point;
    bool copy_press;
    Peg *copy_peg;
    bool game_over;

    int total_turns;
    int total_games;
};

class MindBreaker : public QMainWindow // QWidget
{
    Q_OBJECT
public:
    MindBreaker(QWidget *parent=0, const char *name=0, int wFlags=0 );

public slots:
    void setScore(int, int);

private:
    QCanvas canvas;
    MindBreakerBoard *board;
    QToolButton *score;

};


#endif 
