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
#ifndef MINEFIELD_H
#define MINEFIELD_H

#include <qtable.h>

class Mine;
class Config;

class MineField : public QTable
{
    Q_OBJECT
public:
    MineField( QWidget* parent = 0, const char* name = 0 );
    ~MineField();

    enum State { Waiting, Playing, GameOver };

    State state() const { return stat; }

    void readConfig(Config&);
    void writeConfig(Config&) const;

    int level() const { return lev; }

public slots:
    void setup( int level );

    void showMines();

signals:
    void gameOver( bool won );
    void gameStarted();
    void mineCount( int );
    
protected:
    void paintFocus( QPainter*, const QRect& );
    void viewportMousePressEvent( QMouseEvent* );
    void viewportMouseReleaseEvent( QMouseEvent* );
    void keyPressEvent( QKeyEvent* );
    void keyReleaseEvent( QKeyEvent* );

    int getHint( int row, int col );
    void setHint( Mine* );
    void updateMine( int row, int col );
    void paletteChange( const QPalette & );

protected slots:
    void cellPressed( int row, int col );
    void cellClicked( int row, int col );
    void held();

private:
    State stat;
    void MineField::setState( State st );
    void MineField::placeMines();
    enum FlagAction { NoAction, FlagOn, FlagNext };
    FlagAction flagAction;
    bool ignoreClick;
    int currRow;
    int currCol;
    int minecount;
    int mineguess;
    int nonminecount;
    int lev;
    QTimer *holdTimer;
};

#endif // MINEFIELD_H
