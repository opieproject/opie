/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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

#include "cannon.h"
#include "base.h"
#include "helicopter.h"

#include <qpe/sound.h>

#include <qmainwindow.h>
#include <qtimer.h>
#include <qlabel.h>

class QCanvas;
class Helicopter;

//enum Direction{
//      left, right, up, down };   

class ParaShoot : public QMainWindow {
    Q_OBJECT
 
public:
    ParaShoot(QWidget* parent=0, const char* name=0, WFlags f=0);
    
    void clear();
    void gameOver();
    int mancount;
    void levelUp();
    void moveFaster();

protected:
    virtual void keyPressEvent(QKeyEvent*);
    virtual void keyReleaseEvent(QKeyEvent*);
    virtual void resizeEvent(QResizeEvent *e);
 
private slots:
    void increaseScore(int);
    void newGame();
    void play();
    void wait();

private:
    void showScore( int score, int level );
    QCanvasView* pb;
    QCanvas canvas;
    Cannon* cannon;
    Base* base;
    QCanvasText* gameover;
    QLabel* levelscore;
    int nomen;
    int level;
    int oldscore;
    int updatespeed;
    QTimer* autoDropTimer;
    QTimer* pauseTimer;
    bool gamestopped;
    bool waitover;
    Sound fanfare;
    int score;
    int lastcannonkey;
};           
