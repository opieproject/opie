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
#ifndef CHESS_H
#define CHESS_H

#include <qwidget.h>
#include <qcanvas.h>
#include <qmainwindow.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qstack.h>
#include <qvector.h>
#include <qpe/process.h>

#define wPawn 'P'
#define	wKnight 'N'
#define wBishop 'B'
#define wRook 'R'
#define wQueen 'Q'
#define wKing 'K'
#define bPawn 'p'
#define bKnight 'n'
#define bBishop 'b'
#define bRook 'r'
#define bQueen 'q'
#define bKing 'k'
#define NONE N

#define sideWhite 0
#define sideBlack 1


class Piece:public QCanvasRectangle {
  public:
    Piece(QCanvas *, int);
    ~Piece() {
    };

    char type;

  protected:
    void drawShape(QPainter &);
};


class CraftyProcess : public Process {
    public:
	CraftyProcess(QObject *parent) : Process( parent ) { qDebug("CraftyProcess functions not implemented"); }
	~CraftyProcess() { }
	bool start() { qDebug("CraftyProcess functions not implemented"); return FALSE; }
	const char *readStdout() { qDebug("CraftyProcess functions not implemented"); return "Blah"; }
	void writeToStdin(const char *) { qDebug("CraftyProcess functions not implemented"); }
};


class BoardView:public QCanvasView {
  Q_OBJECT public:
     BoardView(QCanvas *, QWidget *, const char *);
    ~BoardView() {
    };

  protected:
    void contentsMousePressEvent(QMouseEvent *);

  signals:
    void showMessage(const QString &);

    public slots:void readStdout();
    void craftyDied() {
	qFatal("Crafty died unexpectedly\n");
    };
    void newGame();
    void setTheme(QString);
    void swapSides();
    void undo();
    void annotateGame();

  private:
    CraftyProcess * crafty;
    QList < Piece > list;
    Piece *activePiece;

    void revertLastMove();
    void emitErrorMessage();
    void drawBackgroundImage(QPoint activeSquare);

    void buildImages(QImage);

    char convertToFile(int);
    int convertToRank(int);
    int convertFromFile(char);
    int convertFromRank(int);

    void decodePosition(const QString & t);

    Piece *findPiece(char f, int r);
    Piece *newPiece(int, char, int);
    void deletePiece(Piece *);

    int pieceSize;
    QPixmap bg;
    QImage whiteSquare, blackSquare, activeWhiteSquare, activeBlackSquare;

    bool humanSide;
    bool activeSide;
    bool playingGame;
};

#endif
