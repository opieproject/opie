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

#ifndef GOWIDGET_H
#define GOWIDGET_H

#include <qmainwindow.h>
#include "amigo.h"


class QToolBar;

class GoMainWidget : public QMainWindow
{
    Q_OBJECT
public:
    GoMainWidget( QWidget *parent=0, const char* name=0);
protected:
    void resizeEvent( QResizeEvent * );
private:
    QToolBar *toolbar;
    
};


class QLabel;
class GoWidget : public QWidget
{
    Q_OBJECT
public:
    GoWidget( QWidget *parent=0, const char* name=0);
    ~GoWidget();

    void doMove( int x, int y );
    void doComputerMove();

    void readConfig();
    void writeConfig();

public slots:    
    void pass();
    void resign();
    void newGame();
    void setTwoplayer( bool );
    void setHandicap( int );
signals:
    void showScore( const QString& );
    void showTurn( const QPixmap& );
    
protected:
    void paintEvent( QPaintEvent * );
    void mousePressEvent( QMouseEvent * );
    void mouseMoveEvent( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );
    void resizeEvent( QResizeEvent * );
private:
    void init();
    void removeStone(short x, short y);
    void placeStone (enum bVal c, short x, short y );

    void refresh( int x, int y );
    void showStone( int x, int y, enum bVal );
    void reportPrisoners(int,int);
    
    inline int x2board( int x ) { return (x-bx+d/2)/d; }
    inline int y2board( int y ) { return (y-by+d/2)/d; }

    void endGame();

    bool twoplayer;
    enum bVal currentPlayer;
    bool gameActive;
    int nPassed;
    signed char board[19][19];

    int d; //distance between lines
    int bx; //vertical baseline
    int by; //horizontal baseline
    
    int lastX,lastY;
    int newX,newY;
    
    static GoWidget *self;

    friend void removestone(short x, short y);
    friend void	intrPrisonerReport( short, short );
    friend void placestone(enum bVal c, short x, short y );
};





#endif
