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
#include <qmainwindow.h>  
#include <qcanvas.h>
#include <qlabel.h>

#include "snake.h"
#include "target.h"
#include "obstacle.h"

// class QCanvas;

class SnakeGame : public QMainWindow {
    Q_OBJECT
 
public:
    SnakeGame(QWidget* parent=0, const char* name=0, WFlags f=0);
    ~SnakeGame();

    void clear(); 
    void createTargets();
    void welcomescreen();

protected:
    virtual void keyPressEvent(QKeyEvent*); 
    virtual void resizeEvent(QResizeEvent *e);

signals:
    void moveFaster();

private slots:
    void newGame();
    void gameOver();
    void wait();
    void levelUp();
    void scoreInc();
 
private:
    void showScore(int);
    QCanvasView* cv;
    QLabel* scorelabel;
    QCanvas canvas;
    QTimer* pauseTimer;
    Snake* snake;
    int last;  
    int level; 
    int stage;
    int targetamount;
    int notargets;
    bool waitover;
    bool gamestopped;
};
