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

#include "gowidget.h"

#include <qpe/config.h>
#include <qpe/resource.h>

#include <qpainter.h>
#include <qpixmap.h>
#include <qpe/qpetoolbar.h>
#include <qpe/qpemenubar.h>
#include <qpopupmenu.h>
#include <qaction.h>
#include <qapplication.h> //processEvents()
#include <qlabel.h>

//#include <stdio.h>

#include "amigo.h"
#include "goplayutils.h"

static const enum bVal computer_color = BLACK;
    
static int current_handicap = 1;

static QBrush *goBrush;
//static QImage *newBlackStone;
//static QImage *blackStone;
//static QImage *whiteStone;
static QPixmap *newBlackStone;
static QPixmap *blackStone;
static QPixmap *whiteStone;

static bool smallStones = FALSE;

GoMainWidget::GoMainWidget( QWidget *parent, const char* name) :
	QMainWindow( parent, name ) 
{
     setToolBarsMovable( FALSE );
    GoWidget *go = new GoWidget(this);

    setCentralWidget(go);
    toolbar = new QPEToolBar(this);
    toolbar->setHorizontalStretchable( TRUE );
    addToolBar(toolbar);

    QPEMenuBar *mb = new QPEMenuBar( toolbar );
    mb->setMargin(0);
    QPopupMenu *file = new QPopupMenu( this );

    QAction *a = new QAction( tr( "New Game" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), go, SLOT( newGame() ) );
    a->addTo( file );

    a = new QAction( tr( "Pass" ), Resource::loadPixmap( "go/pass" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), go, SLOT( pass() ) );
    a->addTo( file );
    a->addTo( toolbar );


    a = new QAction( tr( "Resign" ), Resource::loadPixmap( "go/reset" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), go, SLOT( resign() ) );
    a->addTo( file );

    a = new QAction( tr( "Two player option" ),  QString::null, 0, this, 0 );
    a->setToggleAction( TRUE );
    connect( a, SIGNAL( toggled(bool) ), go, SLOT( setTwoplayer(bool) ) );
    a->addTo( file );

    mb->insertItem( tr( "Game" ), file );

    QLabel *turnLabel = new QLabel( toolbar );
    turnLabel->setBackgroundMode( PaletteButton );
    connect( go, SIGNAL(showTurn(const QPixmap&)), 
	     turnLabel, SLOT(setPixmap(const QPixmap&)) );

    
    QLabel * scoreLabel = new QLabel( toolbar );
    scoreLabel->setBackgroundMode( PaletteButton );
    connect( go, SIGNAL(showScore(const QString&)), 
	     scoreLabel, SLOT(setText(const QString&)) );

    toolbar->setStretchableWidget( scoreLabel );
    
    go->readConfig();
}

void GoMainWidget::resizeEvent( QResizeEvent * )
{
    //### this won't work because of the text label...
    /*
    if ( width() > height() )
	moveToolBar( toolbar, Left );
    else
	moveToolBar( toolbar, Top );
    */
}

GoWidget *GoWidget::self = 0;

GoWidget::GoWidget( QWidget *parent, const char* name) :
	QWidget( parent, name ) 
{
    if ( self )
	fatal( "Only one Go widget allowed" );
    self = this;
    twoplayer = FALSE;

    
    d = bx = by = 1;
    
    QPixmap pix = Resource::loadPixmap( "go/pine" );
    goBrush = new QBrush( black, pix );
    /*
    QString fn = Resource::findPixmap("Go-black");
    blackStone = new QImage( fn );
    fn = Resource::findPixmap("Go-black-highlight");
    newBlackStone = new QImage( fn );
    fn = Resource::findPixmap("Go-white");
    whiteStone = new QImage( fn );
    */
    blackStone = new QPixmap(Resource::loadPixmap( "go/Go-black" ));
    whiteStone = new QPixmap(Resource::loadPixmap( "go/Go-white" ));
    newBlackStone = new QPixmap(Resource::loadPixmap( "go/Go-black-highlight" ));

    init();
}

GoWidget::~GoWidget()
{
    writeConfig();
}

void GoWidget::writeConfig()
{
    Config cfg("Go");
    cfg.setGroup("Game");
    cfg.writeEntry("TwoPlayer", twoplayer);
    cfg.writeEntry("CurrentPlayer", currentPlayer);
    cfg.writeEntry("NPassed", nPassed);
    QString b;
    for (int i=0; i<19; i++)
	for (int j=0; j<19; j++)
	    b += board[i][j] == BLACK ? 'B' : board[i][j] == WHITE ? 'W' : '.';
    cfg.writeEntry("Board", b);
    cfg.writeEntry("LastX", lastX);
    cfg.writeEntry("LastY", lastY);
    extern int blackPrisoners, whitePrisoners;
    cfg.writeEntry("BlackPrisoners", blackPrisoners);
    cfg.writeEntry("WhitePrisoners", whitePrisoners);
}

void GoWidget::readConfig()
{
    init();
    Config cfg("Go");
    cfg.setGroup("Game");
    twoplayer = cfg.readBoolEntry("TwoPlayer");
    currentPlayer = (bVal)cfg.readNumEntry("CurrentPlayer",1);
    nPassed = cfg.readNumEntry("NPassed",0);
    QString b = cfg.readEntry("Board");
    if ( b.length() == 19*19 )
	for (int i=0; i<19; i++)
	    for (int j=0; j<19; j++) {
		QChar ch = b[j+19*i];
		if ( ch != '.' )
		    GoPlaceStone( ch == 'B' ? BLACK : WHITE, i, j );
	    }
    lastX = cfg.readNumEntry("LastX");
    lastY = cfg.readNumEntry("LastY");
    extern int blackPrisoners, whitePrisoners;
    blackPrisoners = cfg.readNumEntry("BlackPrisoners",0);
    whitePrisoners = cfg.readNumEntry("WhitePrisoners",0);
    reportPrisoners(blackPrisoners,whitePrisoners);
    emit showTurn( currentPlayer == WHITE ? *whiteStone : *blackStone );
}

void GoWidget::resizeEvent( QResizeEvent * )
{
    d = QMIN(width(),height())/19;
    //    int r = (d/2-1);
    bx = (width() - 18*d)/2 ;
    by = (height() - 18*d)/2 ;

    if ( d < 10 && !smallStones ) {
	blackStone->convertFromImage( blackStone->convertToImage().smoothScale(8,8) );
	whiteStone->convertFromImage( whiteStone->convertToImage().smoothScale(8,8) );
	newBlackStone->convertFromImage( newBlackStone->convertToImage().smoothScale(8,8) );
	
	smallStones = TRUE;
    } else if ( d >= 10 && smallStones ) {
	blackStone = new QPixmap(Resource::loadPixmap( "go/Go-black" ));
	whiteStone = new QPixmap(Resource::loadPixmap( "go/Go-white" ));
	newBlackStone = new QPixmap(Resource::loadPixmap( "go/Go-black-highlight" ));
	smallStones = FALSE;
    }
}

void GoWidget::init()
{
    lastX = lastY = newX = newY = -1;
    nPassed = 0;
    for ( int i = 0; i < 19; i++ )
	for ( int j = 0; j < 19; j++ )
	    board[i][j]=-1;
    gameActive = TRUE;
    goRestart(current_handicap);

    if ( twoplayer ) {
	currentPlayer = BLACK;
    } else {
	doComputerMove();
	currentPlayer = WHITE;
    }
    emit showTurn( currentPlayer == WHITE ? *whiteStone : *blackStone );
}

void GoWidget::paintEvent( QPaintEvent *e )
{
    int i,j;

    int r = whiteStone->width()/2;
    
    QPainter p(this);
    p.fillRect( bx - d/2, by - d/2, 19*d, 19*d, *goBrush );

    int xMin = QMAX( x2board(e->rect().left()), 0 );
    int xMax = QMIN( x2board(e->rect().right()), 18 );
    int yMin = QMAX( y2board(e->rect().top()), 0 );
    int yMax = QMIN( y2board(e->rect().bottom()), 18 );

    QColor pine( 255, 186, 89 );
    p.setPen( pine.dark() );
    
    for ( i = xMin; i < xMax+1 ; i ++ ) {
	p.drawLine( bx+i*d, by, bx+i*d, by+18*d );
    }
    for ( j = yMin; j < yMax+1 ; j ++ ) {
	p.drawLine( bx, by+j*d, bx+18*d, by+j*d);
    }

    // dots are at (3,3), (3,9), (3,15) and so on
    p.setBrush( black );
    for ( i = 3; i < xMax+1; i+=6 )
	for ( j = 3; j < yMax+1; j+=6 )
	    p.drawEllipse( bx+i*d-2, by+j*d-2, 5, 5 );
    
    
    for ( i = xMin; i < xMax+1; i++ )
	for ( j = yMin; j < yMax+1; j++ ) {
	    if ( board[i][j] == WHITE || 
		 currentPlayer==WHITE && newX == i && newY == j )
		p.drawPixmap( bx+i*d - r, by+j*d - r, *whiteStone );
	    else if ( i == lastX && j == lastY )
		p.drawPixmap( bx+i*d - r, by+j*d - r, *newBlackStone );
	    else if ( board[i][j] == BLACK ||
		      currentPlayer==BLACK && newX == i && newY == j)
		p.drawPixmap( bx+i*d - r, by+j*d - r, *blackStone );
	}
}

void GoWidget::doMove( int x, int y )
{

    if ( !GoPlaceStone( currentPlayer, x, y ) ) {
	//printf( "Illegal move (%d,%d)\n", x, y );
	return;
    }
    //printf( "you do (%d,%d)\n", x, y );
    nPassed = 0;
    if ( twoplayer )
	currentPlayer = (currentPlayer==WHITE) ? BLACK : WHITE;
    else
	doComputerMove();

    emit showTurn( currentPlayer == WHITE ? *whiteStone : *blackStone );

}

void GoWidget::pass()
{
    if ( !gameActive )
	return;
    nPassed++;
    if ( nPassed >= 2 )
	endGame();
    else if ( !twoplayer )
	doComputerMove();
}

void GoWidget::resign()
{
    if ( gameActive )
	endGame();
}


void GoWidget::newGame()
{
    init();
    update();
}


void GoWidget::endGame()
{
    gameActive = FALSE;

    int w,b;
    CountUp( &w, &b);
    QString s = tr("White %1, Black %2. ").arg(w).arg(b);
    if ( w > b )
	s += tr("White wins.");
    else if ( w < b )
	s += tr("Black wins.");
    else
	s += tr("A draw.");
    emit showScore( s );
}

void GoWidget::doComputerMove()
{
    int ox = lastX;
    int oy = lastY;
    lastX = lastY = -1;
    emit showTurn( *blackStone );
    refresh( ox, oy);
    qApp->processEvents();
    short int x,y;
    if ( genMove( computer_color, &x, &y ) ) {
	lastX = x;
	lastY = y;
	//printf( "I do (%d,%d)\n", x, y );
	GoPlaceStone(computer_color,x,y);
	nPassed = 0;
    } else {
	emit showScore( tr("I pass") );
	nPassed++;
	if ( nPassed >= 2 )
	    endGame();
    }
}

void GoWidget::mousePressEvent( QMouseEvent *me )
{
    if ( !gameActive )
	return;
    int x = x2board(me->x());
    int y = y2board(me->y());
    showStone(x,y,currentPlayer);
}

void GoWidget::mouseMoveEvent( QMouseEvent *me )
{
    if ( !gameActive )
	return;
    int x = x2board(me->x());
    int y = y2board(me->y());
    if ( x != newX || y != newY )
	showStone(x,y,currentPlayer);
}

void GoWidget::showStone( int x, int y, enum bVal c )
{

    if ( newX > -1 ) {
	refresh( newX, newY );
	newY = newX = -1;
    }
    if ( x < 0 || x > 18 || y < 0 || y > 18 ) {
	newX = newY = -1;
	return;
    }
    if ( board[x][y] == -1 && !Suicide( c, x, y ) ) {
	newX = x;
	newY = y;
	refresh(x,y);
    }
    
}

void GoWidget::mouseReleaseEvent( QMouseEvent * )
{
    if ( gameActive && newX > -1 )
	doMove( newX, newY );
    newX = newY = -1;
}

void GoWidget::refresh( int x, int y )
{
    update( bx+d*x-d/2-1, by+d*y-d/2-1, d+2, d+2 );    
}

void GoWidget::removeStone(short x, short y)
{
    board[x][y]=-1;
    refresh( x, y );
}

void GoWidget::placeStone (enum bVal c, short x, short y )
{
    board[x][y]=c;
    refresh( x, y );
}

void GoWidget::reportPrisoners( int blackcnt, int whitecnt )
{
    QString s = tr( "Prisoners: black %1, white %2" ).arg(blackcnt).arg(whitecnt);
    emit showScore( s );
}

void GoWidget::setTwoplayer( bool b )
{
    twoplayer = b;
}

void GoWidget::setHandicap( int h )
{
    current_handicap = h;
}


extern "C" {

void	removestone(short x, short y)
{
    GoWidget::self->removeStone(x,y);
}

void	placestone (enum bVal c, short x, short y )
{
    GoWidget::self->placeStone(c,x,y);
}

void	intrMoveReport(enum bVal c ,char *coord ,char *reason )
{
    qDebug( "intrMoveReport colour %d, %s %s", c, coord, reason );
}

void	intrPrisonerReport( short blackcnt, short whitecnt )
{
    GoWidget::self->reportPrisoners(blackcnt,whitecnt);
}

}

