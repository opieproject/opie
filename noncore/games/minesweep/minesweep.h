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
#ifndef MINESWEEP_H
#define MINESWEEP_H

#include <qmainwindow.h>
#include <qdatetime.h>

class MineField;
class QLCDNumber;
class QPushButton;

class MineSweep : public QMainWindow
{
    Q_OBJECT
public:
    MineSweep( QWidget* parent = 0, const char* name = 0, WFlags f = 0 );
    ~MineSweep();

public slots:
    void gameOver( bool won );
    void newGame();

protected slots:
    void setCounter( int );
    void updateTime();

    void beginner();
    void advanced();
    void expert();

private slots:
    void startPlaying();

private:
    void readConfig();
    void writeConfig() const;

    void newGame(int);
    MineField* field;
    QLCDNumber* guessLCD;
    QLCDNumber* timeLCD;
    QPushButton* newGameButton;
    
    QDateTime starttime;
    QTimer* timer;
};

#endif // MINESWEEP_H

