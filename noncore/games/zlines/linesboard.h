/***************************************************************************
                          linesboard.h  -  description
                             -------------------
    begin                : Fri May 19 2000
    copyright            : (C) 2000 by Roman Merzlyakov
    email                : roman@sbrf.barrt.ru
    copyright            : (C) 2000 by Roman Razilov
    email                : Roman.Razilov@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef __linesboard_h
#define __linesboard_h

#include <qwidget.h>
#include <qpixmap.h>
#include <qtimer.h>

#include "field.h"
//#include "shotcounter.h"
#include "ballpainter.h"

class LinesBoard : public Field
{
  Q_OBJECT
public:
  LinesBoard( BallPainter * abPainter, QWidget* parent=0, const char* name=0 );
  ~LinesBoard();

  int width();
  int height();
  int wHint();
  int hHint();
//  void doAfterBalls();
  void placeBalls(int nextBalls[BALLSDROP]);
    void undo();

signals:
  void endTurn();
  void endGame();
  void eraseLine(int nb);

private:
  int anim;

  struct Waypoints {
        int x,y;
  } *way;
    int nextBalls[BALLSDROP];

  int animmax;

  //used for running and animation phase
  int painting;
  int animstep;
  int animdelaycount;
  int animdelaystart;
  int direction;


  int nextBallToPlace;
  int jumpingCol;
  int jumpingRow;


  QTimer* timer;
//  ShotCounter* shCounter;
  BallPainter* bPainter;

  void paintEvent( QPaintEvent* );
  void mousePressEvent( QMouseEvent* );

  void AnimStart(int panim);
  void AnimNext();
  int AnimEnd();
  int getAnim(int x, int y ); // returns if the specifyed cell is animated..
  void AnimJump( int col, int row );

  int  erase5Balls();
  bool existPath(int ax, int ay, int bx, int by);
  void  placeBall();

protected slots:
  void timerSlot();

};

#endif
