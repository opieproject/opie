/*
 * Copyright (C) 2000 Stefan Schimanski <1Stein@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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

#ifndef KJEZZBALL_H_INCLUDED
#define KJEZZBALL_H_INCLUDED

#include <qmainwindow.h>
#include <qmenubar.h>
#include <qlabel.h>

class JezzGame;
class QLCDNumber;
class QGridLayout;

class KJezzball : public QMainWindow
{
   Q_OBJECT

public:
   KJezzball();

public slots:
   void newGame();
   void pauseGame(); 
   void closeGame();
   void about();

protected slots:   
   void died();
   void newPercent( int percent ); 
   void second();
   void switchLevel();
   void gameOverNow();
      		
protected:
   void createLevel( int level );
   void startLevel();
   void stopLevel();
   void nextLevel();
   void gameOver();
   void initXMLUI();

   void focusOutEvent( QFocusEvent * );
   void focusInEvent ( QFocusEvent * );
	void keyPressEvent( QKeyEvent *ev );

   JezzGame *m_gameWidget;
   QWidget *m_view;
   QGridLayout *m_layout;
   QLCDNumber *m_levelLCD;
   QLCDNumber *m_lifesLCD;
   QLCDNumber *m_scoreLCD;
   QLCDNumber *m_percentLCD;
   QLCDNumber *m_timeLCD;

   QTimer *m_timer;
   QTimer *m_nextLevelTimer;
   QTimer *m_gameOverTimer;

   enum { Idle, Running, Paused, Suspend } m_state;

   struct
   {
       int lifes;
       int time;
       int score;
   } m_level;

   struct
   {
       int level;
       int score;
   } m_game;
private:
   QMenuBar *menu;
   QPopupMenu *game;
   QLabel* ScoreLabel;
   QLabel* LivesLabel;
   QLabel* FilledLabel;
   QLabel* TimeLabel;

};

#endif
