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


// human is not offered a promotion yet

#include <qcanvas.h>
#include <qmainwindow.h>
#include <qlist.h>
#include <qimage.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qregexp.h>

#include <qpe/config.h>
#include <qpe/resource.h>

#include "chess.h"

#define CHESS_DEBUG

int pieceSize = 40;
static QVector < QImage > imgList;
int timeMoves, timeTime;

int BoardView::convertToRank(int r)
{
    r = r / pieceSize;
    if (humanSide == sideWhite)
	r = 8 - r;
    else
	r++;
    return r;
}

char BoardView::convertToFile(int f)
{
    f = f / pieceSize;
    if (humanSide == sideWhite)
	return f + 'a';
    else
	return 'h' - f;
}

int BoardView::convertFromFile(char f)
{
    if (humanSide == sideWhite)
	f = f - 'a';
    else
	f = 'h' - f;
    return f * pieceSize;
}

int BoardView::convertFromRank(int r)
{
    if (humanSide == sideWhite)
	r = 8 - r;
    else
	r--;
    return r * pieceSize;
}

// Pieces
Piece::Piece(QCanvas * canvas, int t):QCanvasRectangle(canvas)
{
    type = t;
    setSize(pieceSize, pieceSize);
    show();
}

Piece *BoardView::newPiece(int t, char f, int r)
{
    Piece *tmpPiece = new Piece(canvas(), t);
    tmpPiece->move(convertFromFile(f), convertFromRank(r));
    list.append(tmpPiece);
    return tmpPiece;
}

void BoardView::deletePiece(Piece * p)
{
    list.remove(p);
    canvas()->update();
}

void Piece::drawShape(QPainter & p)
{
    p.drawImage(int (x()), int (y()), *(imgList[type]));
}

void BoardView::buildImages(QImage theme)
{
    imgList.resize(12);
    int x;
    int y = 0;

    for (int j = 0; j < 2; j++) {
	x = 0;
	for (int i = 0; i < 6; i++) {
	    imgList.insert(i + (j * 6),
			   new QImage(theme.
				      copy(x, y, pieceSize, pieceSize)));
	    x += pieceSize;
	}
	y += pieceSize;
    }
}

void BoardView::readStdout()
{
    QString input( crafty->readStdout() );
#ifdef CHESS_DEBUG
    qDebug("received this string from crafty->\n%s\n", input.latin1());
#endif

    int startPosition = input.find("setboard");
    if (startPosition != -1)
	decodePosition(input.remove(0, startPosition + 9));

    if (input.contains("Black mates")) {
	playingGame = FALSE;
	emit(showMessage("Black mates"));
    } else if (input.contains("White mates")) {
	playingGame = FALSE;
	emit(showMessage("White mates"));
    } else if (input.contains(" resigns")) {
	playingGame = FALSE;
	emit(showMessage("Computer resigns"));
    } else if (input.contains("Draw")) {
	playingGame = FALSE;
	emit(showMessage("Draw"));
    }
}

// this is pretty close to getting done right
// maybe dont use sprites and just draw a picture
// there'll be lots of drawing done anyway
// eg creating pictures for the webpages,
// and presenting options for promotions
void BoardView::decodePosition(const QString & t)
{
    qDebug("decode copped %s \n", t.latin1());

    int count = 0;
    int stringPos = 0;
    for (int file = 0; file < 8; file++) {
	for (int rank = 0; rank < 8; rank++) {
	    if (count)
		count--;
	    else {
		if (t.at(stringPos).isNumber())
		    count = t.at(stringPos).digitValue();
		else {
		    newPiece(t.at(stringPos).latin1(), 'a' + file,
			     rank + 1);
		}
	    }
	}
    }
}

void BoardView::undo()
{
    crafty->writeToStdin("undo\n");
    crafty->writeToStdin("savepos\nclock\n");
}

void BoardView::emitErrorMessage()
{
    if (activeSide != humanSide)
	emit(showMessage("Not your move"));
    else
	emit(showMessage("You are not playing a game"));
}

void BoardView::annotateGame()
{
    crafty->
	writeToStdin
	("savegame game.save\nannotateh game.save bw 0 1.0 1\n");
    emit(showMessage("Annotating game"));
}

Piece *BoardView::findPiece(char f, int r)
{
    QListIterator < Piece > it(list);
    Piece *tmpPiece;
    for (; it.current(); ++it) {
	tmpPiece = it.current();
	if (convertToRank(tmpPiece->x()) == r
	    && convertToFile(tmpPiece->y()) == f)
	    return tmpPiece;
    }
    return 0;
}

void BoardView::newGame()
{
    activeSide = sideWhite;
    emit(showMessage("New game"));
    crafty->writeToStdin("new\n");
    crafty->writeToStdin("savepos\n");
    crafty->writeToStdin("time " +
			 QString::number(timeMoves) +
			 "/" + QString::number(timeTime) + "\n");
    activeSide = sideWhite;
    if (humanSide == sideBlack)
	crafty->writeToStdin("go\n");
}

void BoardView::setTheme(QString filename)
{
    QImage theme = Resource::loadImage(QString("chess/") + filename);
    pieceSize = theme.height() / 2;
    setFrameStyle(QFrame::Plain);
    setFixedSize(8 * pieceSize, 8 * pieceSize);
    canvas()->setBackgroundColor(Qt::red);
    canvas()->resize(8 * pieceSize, 8 * pieceSize);
    whiteSquare = theme.copy(6 * pieceSize, 0, pieceSize, pieceSize);
    activeWhiteSquare = theme.copy(7 * pieceSize, 0, pieceSize, pieceSize);
    blackSquare =
	theme.copy(6 * pieceSize, pieceSize, pieceSize, pieceSize);
    activeBlackSquare =
	theme.copy(7 * pieceSize, pieceSize, pieceSize, pieceSize);
    buildImages(theme);
    drawBackgroundImage(QPoint(-1, -1));
}


// sets the bg to the default background image for the current theme
// also resposible for drawing the "active" marker
void BoardView::drawBackgroundImage(QPoint activeSquare)
{
    bg = QPixmap(8 * pieceSize, 8 * pieceSize);
    QPainter p(&bg);
    bool col = FALSE;
    for (int i = 0; i < 8; i++) {
	for (int j = 0; j < 8; j++) {
	    QPoint point(i * pieceSize, j * pieceSize);
	    if (col) {
		if (point.x() == activeSquare.x()
		    && point.y() == activeSquare.y())
		    p.drawImage(point, activeBlackSquare);
		else
		    p.drawImage(point, blackSquare);
		col = FALSE;
	    } else {
		if (point.x() == activeSquare.x()
		    && point.y() == activeSquare.y())
		    p.drawImage(point, activeWhiteSquare);
		else
		    p.drawImage(point, whiteSquare);
		col = TRUE;
	    }
	}
	col = !col;
    }
    canvas()->setBackgroundPixmap(bg);
    canvas()->update();
}


// Board view widget
void BoardView::contentsMousePressEvent(QMouseEvent * e)
{
    QCanvasItemList cList = canvas()->collisions(e->pos());
    if (activeSide == humanSide && playingGame) {
	if (!activePiece) {
	    if (cList.count()) {
		activePiece = (Piece *) (*(cList.at(0)));
		drawBackgroundImage(QPoint
				    (activePiece->x(), activePiece->y()));
	    }
	} else {
	    if (!(activePiece == (Piece *) (*(cList.at(0))))) {
		char fromFile = convertToFile(activePiece->x());
		int fromRank = convertToRank(activePiece->y());
		char toFile = convertToFile(e->pos().x());
		int toRank = convertToRank(e->pos().y());
		QString moveS;
		moveS.append(fromFile);
		moveS.append(moveS.number(fromRank));
		moveS.append(toFile);
		moveS.append(moveS.number(toRank));
		if ((activePiece->type == wPawn
		     && fromRank == 7 && toRank == 8)
		    || (activePiece->type == bPawn
			&& fromRank == 2 && toRank == 1)) {
		    // offer a promotion
		    emit(showMessage
			 ("you are meant to be offered a promotion here"));
		    char promoteTo = wQueen;	// doesnt matter for now
		    moveS.append(promoteTo);
		    moveS.append("\n");
		    crafty->writeToStdin(moveS.latin1());
		}
	    }
	    activePiece = 0;
	    drawBackgroundImage(QPoint(-1, -1));
	}
    }

    else {
	emitErrorMessage();
    }
}

void BoardView::swapSides()
{
    if (activeSide == humanSide && playingGame) {
	humanSide = !humanSide;
	crafty->writeToStdin("savepos\ngo\n");
    } else
	emitErrorMessage();
}

BoardView::BoardView(QCanvas *c, QWidget *w, const char *name)
    : QCanvasView(c, w, name) {
    humanSide = sideWhite;
    activeSide = sideWhite;
    playingGame = TRUE;
    activePiece = 0;
    list.setAutoDelete(TRUE);
    setCanvas(new QCanvas());
    Config c("Chess", Config::User);
    c.setGroup("Theme");
    QString theme = c.readEntry("imagefile", "simple-28");
    setTheme(theme);
    crafty = new CraftyProcess(this);
    crafty->addArgument("crafty");
    if (!crafty->start()) {
	QMessageBox::critical(0,
			      tr("Could not find crafty chess engine"),
			      tr("Quit"));
	exit(-1);
    }

    connect(crafty, SIGNAL(readyReadStdout()), this, SLOT(readStdout()));
    connect(crafty, SIGNAL(processExited()), this, SLOT(craftyDied()));
//    crafty->writeToStdin("xboard\nics\nkibitz=2\n");
    newGame();
}
