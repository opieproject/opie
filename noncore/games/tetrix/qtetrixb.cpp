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


#include "qtetrixb.h"
#include "qtetrix.h"
#include <qtimer.h>
#include <qkeycode.h>
#include <qpainter.h>
#include <qdrawutil.h>

const int waitAfterLineTime = 500;

QTetrixBoard::QTetrixBoard( QWidget *p, const char *name )
    : QWidget( p, name )
{
    paint = 0;
    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), SLOT(timeout()) );

    colors[0].setRgb(200,100,100);
    colors[1].setRgb(100,200,100);
    colors[2].setRgb(100,100,200);
    colors[3].setRgb(200,200,100);
    colors[4].setRgb(200,100,200);
    colors[5].setRgb(100,200,200);
    colors[6].setRgb(218,170,  0);

    xOffset          = -1;      // -1 until a resizeEvent is received.
    blockWidth       = 20;
    yOffset          = 30;
    blockHeight      = 20;
    noGame           = TRUE;
    isPaused         = FALSE;
    waitingAfterLine = FALSE;
    updateTimeoutTime();   // Sets timeoutTime
}

void QTetrixBoard::startGame(int gameType,int fillRandomLines)
{
    if ( isPaused )
        return;		// ignore if game is paused
    noGame = FALSE;
    GenericTetrix::startGame( gameType, fillRandomLines );
    // Note that the timer is started by updateLevel!
}


void QTetrixBoard::pause()
{
    if ( noGame )			// game not active
        return;
    isPaused = !isPaused;
    if ( isPaused ) {
	timer->stop();
        hideBoard();
    }
    else
	timer->start(timeoutTime);
    update();
}


void QTetrixBoard::drawSquare(int x,int y,int value)
{
    if (xOffset == -1)    // Before first resizeEvent?
        return;

    const int X = xOffset  + x*blockWidth;
    const int Y = yOffset  + (y - 1)*blockHeight;

    bool localPainter = paint == 0;
    QPainter *p;
    if ( localPainter )
	p = new QPainter( this );    
    else
	p = paint;
    drawTetrixButton( p, X, Y, blockWidth, blockHeight,
		      value == 0 ? 0 : &colors[value-1] );
    /*
    if ( value != 0 ) {
	QColor tc, bc;
	tc = colors[value-1].light();
	bc = colors[value-1].dark();
	p->drawShadePanel( X, Y, blockWidth, blockHeight,
			   tc, bc, 1, colors[value-1], TRUE );
    }
    else
	p->fillRect( X, Y, blockWidth, blockHeight, backgroundColor() );
	*/
    if ( localPainter )
	delete p;
}

void QTetrixBoard::drawNextSquare( int x, int y, int value )
{
    if ( value == 0 )
        emit drawNextSquareSignal (x, y, 0 );
    else
        emit drawNextSquareSignal( x, y, &colors[value-1] );
}

void QTetrixBoard::updateRemoved( int noOfLines )
{
    if ( noOfLines > 0 ) {
        timer->stop();
        timer->start( waitAfterLineTime );
        waitingAfterLine = TRUE;
    }
    emit updateRemovedSignal( noOfLines );
}

void QTetrixBoard::updateScore( int newScore )
{
    emit updateScoreSignal( newScore );
}

void QTetrixBoard::updateLevel( int newLevel )
{
    timer->stop();
    updateTimeoutTime();
    timer->start( timeoutTime );
    emit updateLevelSignal( newLevel );
}

void QTetrixBoard::pieceDropped(int)
{
    if ( waitingAfterLine ) // give player a break if a line has been removed
        return;
    newPiece();
}

void QTetrixBoard::gameOver()
{
    timer->stop();
    noGame = TRUE;
    emit gameOverSignal();
}

void QTetrixBoard::timeout()
{
    if ( waitingAfterLine ) {
	timer->stop();
	waitingAfterLine = FALSE;
	newPiece();
	timer->start( timeoutTime );
    } else {
        oneLineDown();
    }
}

void QTetrixBoard::paintEvent( QPaintEvent *e )
{
    QPainter p( this );
    const char *text = "Press \"Pause\"";
    QRect r = rect();
    paint = &p;				// set widget painter
    qDrawShadePanel( paint, r, colorGroup(), TRUE, 1 );
    if ( isPaused ) {
	p.drawText( r, AlignCenter | AlignVCenter, text );
        return;
    }
    int x1,y1,x2,y2;
    x1 = (r.left() - xOffset) / blockWidth;
    if (x1 < 0)
        x1 = 0;
    if (x1 >= boardWidth())
        x1 = boardWidth() - 1;

    x2 = (r.right() - xOffset) / blockWidth;
    if (x2 < 0)
        x2 = 0;
    if (x2 >= boardWidth())
        x2 = boardWidth() - 1;

    y1 = (r.top() - yOffset) / blockHeight;
    if (y1 < 0)
        y1 = 0;
    if (y1 >= boardHeight())
        y1 = boardHeight() - 1;

    y2 = (r.bottom() - yOffset) / blockHeight;
    if (y2 < 0)
        y2 = 0;
    if (y2 >= boardHeight())
        y2 = boardHeight() - 1;

    updateBoard( x1, y1, x2, y2, TRUE );
    paint = 0;				// reset widget painter
    return;
}

void QTetrixBoard::resizeEvent(QResizeEvent *e)
{
    QSize sz = e->size();
    blockWidth  = QMAX( 3, (sz.width() - 2)/10 );
    blockHeight = QMAX( 3, (sz.height() - 2)/22 );
    
    //aspect correction
    blockHeight = QMIN( blockHeight, blockWidth*4/3 );
    blockWidth = QMIN( blockWidth, blockHeight*5/4 );
    
    xOffset     = 1;
    //yOffset     = 1;
    yOffset     = (sz.height() - 2) - (blockHeight *22);
    setMask( QRect( 0, 0, blockWidth*10 + 2, QWidget::height() ) );
}

void QTetrixBoard::keyPressEvent( QKeyEvent *e )
{
    if ( noGame || isPaused || waitingAfterLine )
        return;
    switch( e->key() ) {
	case Key_Left :
	    moveLeft();
	    break;
	case Key_Right :
	    moveRight();
	    break;
	case Key_Down :
//	    rotateRight();
	    dropDown();
	    break;
	case Key_Up :
	    rotateLeft();
	    break;
	case Key_Space :
	    dropDown();
	    break;
	case Key_D :
	    oneLineDown();
	    break;
        default:
	    return;
    }
    e->accept();
}

void QTetrixBoard::updateTimeoutTime()
{
    timeoutTime = 1000/(1 + getLevel());
}
