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


#include "qtetrix.h"

#include <qpe/resource.h>

#include <qapplication.h>
#include <qlabel.h>
#include <qdatetime.h>
#include <qlayout.h>



void drawTetrixButton( QPainter *p, int x, int y, int w, int h,
		       const QColor *color )
{
    QColor fc;
    if ( color ) {
        QPointArray a;
	a.setPoints( 3,  x,y+h-1, x,y, x+w-1,y );
	p->setPen( color->light() );
	p->drawPolyline( a );
	a.setPoints( 3, x+1,y+h-1, x+w-1,y+h-1, x+w-1,y+1 );
	p->setPen( color->dark() );
	p->drawPolyline( a );
	x++;
	y++;
	w -= 2;
	h -= 2;
	fc = *color;
    }
    else
	fc = p->backgroundColor();
    p->fillRect( x, y, w, h, fc );
}


ShowNextPiece::ShowNextPiece( QWidget *parent, const char *name )
    : QFrame( parent, name )
{
    setFrameStyle( QFrame::Panel | QFrame::Sunken );
    xOffset = -1;     // -1 until first resizeEvent.
}

void ShowNextPiece::resizeEvent( QResizeEvent *e )
{
    QSize sz = e->size();
    blockWidth  = (sz.width()  - 3)/5;
    blockHeight = (sz.height() - 3)/6;
    xOffset     = (sz.width()  - 3)/5;
    yOffset     = (sz.height() - 3)/6;
}


void ShowNextPiece::paintEvent( QPaintEvent * )
{
    QPainter p( this );
    drawFrame( &p );
    p.end();			// explicit end() so any slots can paint too
    emit update();
}


void ShowNextPiece::drawNextSquare(int x, int y,QColor *color)
{
    if (xOffset == -1)		// Before first resizeEvent?
        return;

    QPainter paint;
    paint.begin(this);
    drawTetrixButton( &paint, xOffset+x*blockWidth, yOffset+y*blockHeight,
		      blockWidth, blockHeight, color );
    paint.end();
}


QTetrix::QTetrix( QWidget *parent, const char *name, WFlags f )
    : QMainWindow( parent, name, f )
{
    setIcon( Resource::loadPixmap( "tetrix_icon" ) );
    setCaption( tr("Tetrix" ) );

    QTime t = QTime::currentTime();
    TetrixPiece::setRandomSeed( (((double)t.hour())+t.minute()+t.second())/
                                 (24+60+60) );

    QWidget *gameArea = new QWidget( this );
    setCentralWidget( gameArea );

    QGridLayout *gl = new QGridLayout( gameArea, 5, 3, 8 );

    QLabel *l;
    l = new QLabel( tr("Next"), gameArea );
    gl->addWidget( l, 0, 0 );
    showNext    = new ShowNextPiece(gameArea);
    showNext->setBackgroundColor(QColor(0,0,0));
    gl->addWidget( showNext, 0, 1 );

    l = new QLabel( tr("Score"), gameArea );
    gl->addWidget( l, 1, 0 );
    showScore   = new QLabel(gameArea);
    gl->addWidget( showScore, 1, 1 );
    l = new QLabel( tr("Level"), gameArea );
    gl->addWidget( l, 2, 0 );
    showLevel   = new QLabel(gameArea);
    gl->addWidget( showLevel, 2, 1 );
    l = new QLabel( tr("Removed"), gameArea );
    gl->addWidget( l, 3, 0 );
    showLines   = new QLabel(gameArea);
    gl->addWidget( showLines, 3, 1 );

    board = new QTetrixBoard(gameArea);
    board->setBackgroundColor(QColor(0,0,0));
    board->setFixedWidth( 124 );
    gl->addMultiCellWidget( board, 0, 4, 2, 2 );
    gl->addColSpacing( 2, 100 );
    gl->addColSpacing( 1, 35 );
    gl->addRowSpacing( 0, 35 );

    QPushButton *pb = new QPushButton( tr("Start"), gameArea );
    pb->setFocusPolicy( NoFocus );
    connect( pb, SIGNAL( clicked() ), board, SLOT( start() ) );
    gl->addMultiCellWidget( pb, 4, 4, 0, 1 );

    connect( board, SIGNAL(gameOverSignal()), SLOT(gameOver()) );
    connect( board, SIGNAL(drawNextSquareSignal(int,int,QColor*)), showNext,
	     SLOT(drawNextSquare(int,int,QColor*)) );
    connect( showNext, SIGNAL(update()), board, SLOT(updateNext()) );
    connect( board, SIGNAL(updateScoreSignal(int)), showScore,
	     SLOT(setNum(int)) );
    connect( board, SIGNAL(updateLevelSignal(int)), showLevel,
	     SLOT(setNum(int)));
    connect( board, SIGNAL(updateRemovedSignal(int)), showLines,
	     SLOT(setNum(int)));

    showScore->setNum( 0 );
    showLevel->setNum( 0 );
    showLines->setNum( 0 );
    board->revealNextPiece(TRUE);
    board->setFocusPolicy( StrongFocus );
}

void QTetrix::gameOver()
{
}


void QTetrix::quit()
{
    close();
}
