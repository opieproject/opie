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

#ifndef _KSAMEWIDGET
#define _KSAMEWIDGET

#include "StoneWidget.h"
#include <kmainwindow.h>

class KToggleAction;

class KSameWidget: public KMainWindow {
     Q_OBJECT
public:
     KSameWidget();
     ~KSameWidget();

private:
     StoneWidget *stone;
     KStatusBar *status;
     KToggleAction *random;
     KAction *restart;
     KAction *undo;

     int multispin_item;


protected:
     void newGame(unsigned int board,int colors);

     virtual void saveProperties(KConfig *conf);
     virtual void readProperties(KConfig *conf);

     bool confirmAbort();

  public slots:
     void sizeChanged();

     /* File Menu */
     void m_new();
     void m_restart();
     void m_load();
     void m_save();
     void m_showhs();
     void m_quit();
     void m_undo();

     /* Options Menu */
     void m_tglboard();

     void gameover();
     void setColors(int colors);
     void setBoard(int board);
     void setScore(int score);
     void setMarked(int m);
     void stonesRemoved(int,int);
    void slotConfigureKeys();

};



#endif
