/***************************************************************************
                          klines.h  -  description
                             -------------------
    begin                : Fri May 19 2000
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
#ifndef KLINES_H
#define KLINES_H

#include <qwidget.h>
#include <qmenubar.h> 
#include <qlabel.h>
#include <qpushbutton.h>
#include <qmainwindow.h>
#include "linesboard.h"
#include "mwidget.h"
#include "prompt.h"

#define LSCORE 0
#define LRECORD 1

class KLines : public QMainWindow
{
  Q_OBJECT
public:
  KLines(QWidget *parent,const char* name, WFlags f);
  ~KLines();

    static QString appName() { return QString::fromLatin1("zlines"); }

public slots:
  void startGame();
  void stopGame();
  void makeTurn();
  void addScore(int ballsErased);
  void switchPrompt();
  void help();
	void undo();
  
protected:
  void resizeEvent( QResizeEvent * );

private slots:

//  bool setSize( int w, int h );
  void setMinSize();

private:
  QMenuBar *menu;
  int idMenuPrompt;
  int idMenuUndo;

  QPopupMenu *game;
  QPopupMenu *edit;
  QToolBar *tool;
  QStatusBar *stat;
  LinesBoard* lsb;
  MainWidget *mwidget;
  LinesPrompt *lPrompt;

  int score, prev_score;

  int nextBalls[BALLSDROP];
	int nextBalls_undo[BALLSDROP];
	int nextBalls_redo[BALLSDROP];
	bool bUndo;

  void searchBallsLine();
  void generateRandomBalls();
  void placeBalls();
  void updateStat();
  void endGame();
	void switchUndo( bool bu );
};

#endif
