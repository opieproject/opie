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

#include "minesweep.h"
#include "minefield.h"

#include <qpe/resource.h>
#include <qpe/config.h>

#include <qpe/qpetoolbar.h>
#include <qpe/qpemenubar.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qlcdnumber.h>
#include <qmessagebox.h>
#include <qtimer.h>
#include <qpalette.h>
#include <qapplication.h>
#include <qlayout.h>
#include <qlabel.h>

#include <stdlib.h>
#include <time.h>




static const char *pix_new[]={
"20 20 3 1",
"  c None",
"# c #00FF00",
". c #000000",
"                    ",
"       ......       ",
"     ..######..     ",
"    .##########.    ",
"   .############.   ",
"  .##############.  ",
"  .##############.  ",
" .################. ",
" .################. ",
" .################. ",
" .################. ",
" .################. ",
" .################. ",
"  .##############.  ",
"  .##############.  ",
"   .############.   ",
"    .##########.    ",
"     ..######..     ",
"       ......       ",
"                    "};


/* XPM */
static const char * happy_xpm[] = {
"20 20 3 1",
" 	c None",
".	c #ffff3f      ",
"#	c #000000",
"                    ",
"       ######       ",
"     ##......##     ",
"    #..........#    ",
"   #............#   ",
"  #..............#  ",
"  #..............#  ",
" #....##....##....# ",
" #....##....##....# ",
" #................# ",
" #................# ",
" #................# ",
" #...#........#...# ",
"  #.##........##.#  ",
"  #...########...#  ",
"   #...######...#   ",
"    #..........#    ",
"     ##......##     ",
"       ######       ",
"                    "};


/* XPM */
static const char * worried_xpm[] = {
"20 20 3 1",
" 	c None",
".	c #ffff3f",
"#	c #000000",
"                    ",
"       ######       ",
"     ##......##     ",
"    #..........#    ",
"   #............#   ",
"  #..............#  ",
"  #..............#  ",
" #....##....##....# ",
" #....##....##....# ",
" #................# ",
" #................# ",
" #................# ",
" #................# ",
"  #....######....#  ",
"  #..............#  ",
"   #............#   ",
"    #..........#    ",
"     ##......##     ",
"       ######       ",
"                    "};


/* XPM */
static const char * dead_xpm[] = {
"20 20 3 1",
" 	c None",
".	c #ffff3f",
"#	c #000000",
"                    ",
"       ######       ",
"     ##......##     ",
"    #..........#    ",
"   #............#   ",
"  #..............#  ",
"  #..#.#...#.#...#  ",
" #....#.....#.....# ",
" #...#.#...#.#....# ",
" #................# ",
" #................# ",
" #................# ",
" #......####......# ",
"  #....#    #....#  ",
"  #...#......#...#  ",
"   #............#   ",
"    #..........#    ",
"     ##......##     ",
"       ######       ",
"                    "};


class ResultIndicator : private QLabel
{
public:   
    static void showResult( QWidget *ref, bool won );
private:    
    ResultIndicator( QWidget *parent, const char *name, WFlags f)
	:QLabel( parent, name, f ) {}
    
    void timerEvent( QTimerEvent *);
    void center();
    bool twoStage;
    int timerId;
};

void ResultIndicator::showResult( QWidget *ref, bool won )
{
    ResultIndicator *r = new ResultIndicator( ref, 0, WStyle_Customize | WStyle_Tool | WType_TopLevel );
				
    r->setAlignment( AlignCenter );
    r->setFrameStyle( Sunken|StyledPanel );
    if ( won ) {
	r->setText( MineSweep::tr("You won!") );
	r->center();
	r->show();
	r->twoStage = FALSE;
	r->timerId = r->startTimer(1500);
    } else {
	QPalette p( red );
	r->setPalette( p );
	r->setText( MineSweep::tr("You exploded!") );
	r->resize( ref->size() );
	r->move( ref->mapToGlobal(QPoint(0,0)) );
	r->show();
	r->twoStage = TRUE;
	r->timerId =r->startTimer(200);
    }
}

void ResultIndicator::center()
{
    QWidget *w = parentWidget();

    QPoint pp = w->mapToGlobal( QPoint(0,0) ); 
    QSize s = sizeHint()*3;
    pp = QPoint( pp.x() + w->width()/2 - s.width()/2,
		pp.y() + w->height()/ 2 - s.height()/2 );

    setGeometry( QRect(pp, s) );
    
}

void ResultIndicator::timerEvent( QTimerEvent *te )
{
    if ( te->timerId() != timerId )
	return;
    killTimer( timerId );
    if ( twoStage ) {
	center();
	twoStage = FALSE;
	timerId = startTimer( 1000 );
    } else {
	delete this;
    }
}


class MineFrame : public QFrame
{
public:
    MineFrame( QWidget *parent, const char *name = 0 )
	:QFrame( parent, name ) {}
    void setField( MineField *f ) { field = f; }
protected:
    void resizeEvent( QResizeEvent *e ) {
	field->setAvailableRect( contentsRect());
	QFrame::resizeEvent(e); 
    }
private:
    MineField *field;
};



MineSweep::MineSweep( QWidget* parent, const char* name, WFlags f )
: QMainWindow( parent, name, f )
{
    srand(::time(0));
    setCaption( tr("Mine Hunt") );
    setIcon( Resource::loadPixmap( "minesweep/minesweep_icon" ) );

    QPEToolBar *toolBar = new QPEToolBar( this );
    toolBar->setHorizontalStretchable( TRUE );

    QPEMenuBar *menuBar = new QPEMenuBar( toolBar );

    QPopupMenu *gameMenu = new QPopupMenu( this );
    gameMenu->insertItem( tr("Beginner"), this, SLOT( beginner() ) );
    gameMenu->insertItem( tr("Advanced"), this, SLOT( advanced() ) );
    gameMenu->insertItem( tr("Expert"), this, SLOT( expert() ) );

    menuBar->insertItem( tr("Game"), gameMenu );
    
    guessLCD = new QLCDNumber( toolBar );
    toolBar->setStretchableWidget( guessLCD );

    QPalette lcdPal( red );
    lcdPal.setColor( QColorGroup::Background, QApplication::palette().active().background() );
    lcdPal.setColor( QColorGroup::Button, QApplication::palette().active().button() );
    
//    guessLCD->setPalette( lcdPal );
    guessLCD->setSegmentStyle( QLCDNumber::Flat );
    guessLCD->setFrameStyle( QFrame::NoFrame );
    guessLCD->setNumDigits( 2 );
    guessLCD->setBackgroundMode( PaletteButton );
    newGameButton = new QPushButton( toolBar );
    newGameButton->setPixmap( QPixmap( pix_new ) );
    newGameButton->setFocusPolicy(QWidget::NoFocus);
    connect( newGameButton, SIGNAL(clicked()), this, SLOT(newGame()) );
    
    timeLCD = new QLCDNumber( toolBar );
//    timeLCD->setPalette( lcdPal );
    timeLCD->setSegmentStyle( QLCDNumber::Flat );
    timeLCD->setFrameStyle( QFrame::NoFrame );
    timeLCD->setNumDigits( 5 ); // "mm:ss"
    timeLCD->setBackgroundMode( PaletteButton );
    
    setToolBarsMovable ( FALSE );

    addToolBar( toolBar );

    MineFrame *mainframe = new MineFrame( this );
    mainframe->setFrameShape( QFrame::Box );
    mainframe->setFrameShadow( QFrame::Raised );

    mainframe->setLineWidth(2);

    field = new MineField( mainframe );
    mainframe->setField( field );
    QFont fnt = field->font();
    fnt.setBold( TRUE );
    field->setFont( QFont( fnt ) );
    field->setFocus();
    setCentralWidget( mainframe );
    
    connect( field, SIGNAL( gameOver( bool ) ), this, SLOT( gameOver( bool ) ) );
    connect( field, SIGNAL( mineCount( int ) ), this, SLOT( setCounter( int ) ) );
    connect( field, SIGNAL( gameStarted()), this, SLOT( startPlaying() ) );

    timer = new QTimer( this );

    connect( timer, SIGNAL( timeout() ), this, SLOT( updateTime() ) );

    readConfig();
}

MineSweep::~MineSweep()
{
    writeConfig();
}

void MineSweep::gameOver( bool won )
{
    field->showMines();
    if ( won ) {
	newGameButton->setPixmap( QPixmap( happy_xpm ) );
    } else {
	newGameButton->setPixmap( QPixmap( dead_xpm ) );
    }
    ResultIndicator::showResult( this, won );
    timer->stop();
}

void MineSweep::newGame()
{
    newGame(field->level());
}

void MineSweep::newGame(int level)
{
    timeLCD->display( "0:00" );
    field->setup( level );
    newGameButton->setPixmap( QPixmap( pix_new ) );
    timer->stop();
}

void MineSweep::startPlaying()
{
    newGameButton->setPixmap( QPixmap( worried_xpm ) );
    starttime = QDateTime::currentDateTime();
    timer->start( 1000 );
}

void MineSweep::beginner()
{
    newGame(1);
}

void MineSweep::advanced()
{
    newGame(2);
}

void MineSweep::expert()
{
    newGame(3);
}

void MineSweep::setCounter( int c )
{
    if ( !guessLCD )
	return;

    guessLCD->display( c );
}

void MineSweep::updateTime()
{
    if ( !timeLCD )
	return;

    int s = starttime.secsTo(QDateTime::currentDateTime());
    if ( s/60 > 99 )
	timeLCD->display( "-----" );
    else
	timeLCD->display( QString().sprintf("%2d:%02d",s/60,s%60) );
}

void MineSweep::writeConfig() const
{
    Config cfg("MineSweep");
    cfg.setGroup("Panel");
    cfg.writeEntry("Time",
	timer->isActive() ? starttime.secsTo(QDateTime::currentDateTime()) : -1);
    field->writeConfig(cfg);
}

void MineSweep::readConfig()
{
    Config cfg("MineSweep");
    field->readConfig(cfg);
    cfg.setGroup("Panel");
    int s = cfg.readNumEntry("Time",-1);
    if ( s<0 ) {
	newGame();
    } else {
	startPlaying();
	starttime = QDateTime::currentDateTime().addSecs(-s);
	updateTime();
    }
}
