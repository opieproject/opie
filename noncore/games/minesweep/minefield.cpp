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
#include "minefield.h"

#include <qpe/config.h>

#include <qpainter.h>
#include <qdrawutil.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qtimer.h>

#include <stdlib.h>

static const char *pix_flag[]={
"13 13 3 1",
"# c #000000",
"x c #ff0000",
". c None",
".............",
".............",
".....#xxxxxx.",
".....#xxxxxx.",
".....#xxxxxx.",
".....#xxxxxx.",
".....#.......",
".....#.......",
".....#.......",
".....#.......",
"...#####.....",
"..#######....",
"............."};

static const char *pix_mine[]={
"13 13 3 1",
"# c #000000",
". c None",
"a c #ffffff",
"......#......",
"......#......",
"..#.#####.#..",
"...#######...",
"..##aa#####..",
"..##aa#####..",
"#############",
"..#########..",
"..#########..",
"...#######...",
"..#.#####.#..",
"......#......",
"......#......"};

class Mine : public QTableItem
{
public:
    enum MineState {
	Hidden = 0,
	Empty,
	Mined,
	Flagged,
#ifdef MARK_UNSURE
	Unsure,
#endif
	Exploded,
	Wrong
    };

    Mine( QTable* );
    void paint( QPainter * p, const QColorGroup & cg, const QRect & cr, bool selected );
    EditType editType() const { return Never; }
    QSize sizeHint() const { return QSize( 12, 12 ); }

    void activate( bool sure = TRUE );
    void setHint( int );

    void setState( MineState );
    MineState state() const { return st; }

    bool isMined() const { return mined; }
    void setMined( bool m ) { mined = m; }

    static void paletteChange();

private:
    bool mined;
    int hint;

    MineState st;

    static QPixmap* knownField;
    static QPixmap* unknownField;
    static QPixmap* flag_pix;
    static QPixmap* mine_pix;
};

QPixmap* Mine::knownField = 0;
QPixmap* Mine::unknownField = 0;
QPixmap* Mine::flag_pix = 0;
QPixmap* Mine::mine_pix = 0;

Mine::Mine( QTable *t )
: QTableItem( t, Never, QString::null )
{
    mined = FALSE;
    st = Hidden;
    hint = 0;
}

void Mine::activate( bool sure )
{
    if ( !sure ) {
	switch ( st ) {
	case Hidden:
	    setState( Flagged );
	    break;
	case Flagged:
#ifdef MARK_UNSURE
	    setState( Unsure );
	    break;
	case Unsure:
#endif
	    setState( Hidden );
	default:
	    break;
	}
    } else if ( st == Flagged ) {
	return;
    } else {
	if ( mined ) {
	    setState( Exploded );
	} else {
	    setState( Empty );
	}
    }
}

void Mine::setState( MineState s )
{
    st = s;
}

void Mine::setHint( int h )
{
    hint = h;
}

void Mine::paletteChange()
{
    delete knownField;
    knownField = 0;
    delete unknownField;
    unknownField = 0;
    delete mine_pix;
    mine_pix = 0;
    delete flag_pix;
    flag_pix = 0;
}

void Mine::paint( QPainter* p, const QColorGroup &cg, const QRect& cr, bool )
{
    if ( !knownField ) {
	knownField = new QPixmap( cr.width(), cr.height() );
	QPainter pp( knownField );
	QBrush br( cg.button().dark(115) );
	qDrawWinButton( &pp, QRect(0,0,cr.width(), cr.height())/*cr*/, cg, TRUE, &br );
    }

    const int pmmarg=cr.width()/5;

    if ( !unknownField ) {
	unknownField = new QPixmap( cr.width(), cr.height() );
	QPainter pp( unknownField );
	QBrush br( cg.button() );
	qDrawWinButton( &pp, QRect(0,0,cr.width(), cr.height())/*cr*/, cg, FALSE, &br );
    }

    if ( !flag_pix ) {
	flag_pix = new QPixmap( cr.width()-pmmarg*2, cr.height()-pmmarg*2 );
	flag_pix->convertFromImage( QImage(pix_flag).smoothScale(cr.width()-pmmarg*2, cr.height()-pmmarg*2) );
    }

    if ( !mine_pix ) {
	mine_pix = new QPixmap( cr.width()-pmmarg*2, cr.height()-pmmarg*2 );
	mine_pix->convertFromImage( QImage(pix_mine).smoothScale(cr.width()-pmmarg*2, cr.height()-pmmarg*2) );
    }

    p->save();

    switch(st) {
    case Hidden:
	p->drawPixmap( 0, 0, *unknownField );
	break;
    case Empty:
	p->drawPixmap( 0, 0, *knownField );
	if ( hint > 0 ) {
	    switch( hint ) {
	    case 1:
		p->setPen( blue );
		break;
	    case 2:
		p->setPen( green );
	    case 3:
		p->setPen( red );
		break;
	    default:
		p->setPen( darkMagenta );
		break;
	    }
	    p->drawText( QRect( 0, 0, cr.width(), cr.height() ), AlignHCenter | AlignVCenter, QString().setNum( hint ) );
	}
	break;
    case Mined:
	p->drawPixmap( 0, 0, *knownField );
	p->drawPixmap( pmmarg, pmmarg, *mine_pix );
	break;
    case Exploded:
	p->drawPixmap( 0, 0, *knownField );
	p->drawPixmap( pmmarg, pmmarg, *mine_pix );
	p->setPen( red );
	p->drawText( QRect( 0, 0, cr.width(), cr.height() ), AlignHCenter | AlignVCenter, "X" );
	break;
    case Flagged:
	p->drawPixmap( 0, 0, *unknownField );
	p->drawPixmap( pmmarg, pmmarg, *flag_pix );
	break;
#ifdef MARK_UNSURE
    case Unsure:
	p->drawPixmap( 0, 0, *unknownField );
	p->drawText( QRect( 0, 0, cr.width(), cr.height() ), AlignHCenter | AlignVCenter, "?" );
	break;
#endif
    case Wrong:
	p->drawPixmap( 0, 0, *unknownField );
	p->drawPixmap( pmmarg, pmmarg, *flag_pix );
	p->setPen( red );
	p->drawText( QRect( 0, 0, cr.width(), cr.height() ), AlignHCenter | AlignVCenter, "X" );
	break;
    }

    p->restore();
}

/*
  MineField implementation
*/

MineField::MineField( QWidget* parent, const char* name )
: QTable( parent, name )
{
    setState( GameOver );
    setShowGrid( FALSE );
    horizontalHeader()->hide();
    verticalHeader()->hide();
    setTopMargin( 0 );
    setLeftMargin( 0 );

    setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum ) );
    
    setSelectionMode( QTable::NoSelection );
    setFocusPolicy( QWidget::NoFocus );

    setCurrentCell( -1, -1 );

    connect( this, SIGNAL( pressed( int, int, int, const QPoint& ) ), this, SLOT( cellPressed( int, int ) ) );
    connect( this, SIGNAL( clicked( int, int, int, const QPoint& ) ), this, SLOT( cellClicked( int, int ) ) );

    holdTimer = new QTimer( this );
    connect( holdTimer, SIGNAL( timeout() ), this, SLOT( held() ) );

    flagAction = NoAction;
    ignoreClick = FALSE;
    currRow = currCol = 0;
    minecount=0;
    mineguess=0;
    nonminecount=0;
}

MineField::~MineField()
{
}

void MineField::setState( State st )
{
    stat = st;
}


void MineField::setup( int level )
{
    lev = level;
    setState( Waiting );
    viewport()->setUpdatesEnabled( FALSE );

    int cellsize;

    int x; 
    int y;
    for ( x = 0; x < numCols(); x++ )
	for ( y = 0; y < numRows(); y++ )
	    clearCell( y, x );

    switch( lev ) {
    case 1:
	setNumRows( 9 );
	setNumCols( 9 );
	minecount = 12;
	cellsize = 21;
	break;
    case 2:
	setNumRows( 16 );
	setNumCols( 16 );
	minecount = 45;
	cellsize = 14;
	break;
    case 3:
	setNumRows( 18 );
	setNumCols( 18 );
	minecount = 66 ;
	cellsize = 12;
	break;
    }
    nonminecount = numRows()*numCols() - minecount;
    mineguess = minecount;
    emit mineCount( mineguess );
    Mine::paletteChange();

    for ( y = 0; y < numRows(); y++ )
	setRowHeight( y, cellsize );
    for ( x = 0; x < numCols(); x++ )
	setColumnWidth( x, cellsize );
    for ( x = 0; x < numCols(); x++ )
	for ( y = 0; y < numRows(); y++ )
	    setItem( y, x, new Mine( this ) );

    updateGeometry();
    viewport()->setUpdatesEnabled( TRUE );
    viewport()->repaint( TRUE );
}


void MineField::placeMines()
{
    int mines = minecount;
    while ( mines ) {
	int col = int((double(rand()) / double(RAND_MAX)) * numCols());
	int row = int((double(rand()) / double(RAND_MAX)) * numRows());

	Mine* mine = (Mine*)item( row, col );

	if ( mine && !mine->isMined() && mine->state() == Mine::Hidden ) {
	    mine->setMined( TRUE );
	    mines--;
	}
    }
}

void MineField::paintFocus( QPainter*, const QRect& )
{
}

void MineField::viewportMousePressEvent( QMouseEvent* e )
{
    QTable::viewportMousePressEvent( e );
}

void MineField::viewportMouseReleaseEvent( QMouseEvent* e )
{
    QTable::viewportMouseReleaseEvent( e );
    if ( flagAction == FlagNext ) {
	flagAction = NoAction;
    }
}

void MineField::keyPressEvent( QKeyEvent* e )
{
#if defined(Q_WS_QWS) || defined(_WS_QWS_)
    flagAction = ( e->key() == Key_Up ) ? FlagOn : NoAction;
#else
    flagAction = ( ( e->state() & ShiftButton ) ==  ShiftButton ) ? FlagOn : NoAction;
#endif
}

void MineField::keyReleaseEvent( QKeyEvent* )
{
    flagAction = NoAction;
}

int MineField::getHint( int row, int col )
{
    int hint = 0;
    for ( int c = col-1; c <= col+1; c++ )
	for ( int r = row-1; r <= row+1; r++ ) {
	    Mine* mine = (Mine*)item( r, c );
	    if ( mine && mine->isMined() )
		hint++;
	}

    return hint;
}

void MineField::setHint( Mine* mine )
{
    if ( !mine )
	return;

    int row = mine->row();
    int col = mine->col();
    int hint = getHint( row, col );

    if ( !hint ) {
	for ( int c = col-1; c <= col+1; c++ )
	    for ( int r = row-1; r <= row+1; r++ ) {
		Mine* mine = (Mine*)item( r, c );
		if ( mine && mine->state() == Mine::Hidden ) {
		    mine->activate( TRUE );
		    nonminecount--;
		    setHint( mine );
		    updateCell( r, c );
		}
	    }
    }

    mine->setHint( hint );
    updateCell( row, col );
}

/*
 state == Waiting means no "hold"

 
*/
void MineField::cellPressed( int row, int col )
{
    if ( state() == GameOver ) 
	return;
    currRow = row;
    currCol = col;
    if ( state() == Playing )
	holdTimer->start( 150, TRUE );
}

void MineField::held()
{
    flagAction = FlagNext;
    updateMine( currRow, currCol );
    ignoreClick = TRUE;
}

/*
  Only place mines after first click, since it is pointless to
  kill the player before the game has started.
*/

void MineField::cellClicked( int row, int col )
{
    if ( state() == GameOver )
	return;
    if ( state() == Waiting ) {
	Mine* mine = (Mine*)item( row, col );
	if ( !mine )
	    return;
	mine->setState( Mine::Empty );
	nonminecount--;
	placeMines();
	setState( Playing );
	emit gameStarted();
	updateMine( row, col );
    } else { // state() == Playing
	holdTimer->stop();
	if ( ignoreClick )
	    ignoreClick = FALSE;
	else
	    updateMine( row, col );
    }
}

void MineField::updateMine( int row, int col )
{
    Mine* mine = (Mine*)item( row, col );
    if ( !mine )
	return;

    bool wasFlagged = mine->state() == Mine::Flagged;
    bool wasEmpty =  mine->state() == Mine::Empty;
    
    mine->activate( flagAction == NoAction );

    if ( mine->state() == Mine::Exploded ) {
	emit gameOver( FALSE );
	setState( GameOver );
	return;
    } else if ( mine->state() == Mine::Empty ) {
	setHint( mine );
	if ( !wasEmpty )
	    nonminecount--;
    }

    if ( flagAction != NoAction ) {
	if ( mine->state() == Mine::Flagged ) {
	    --mineguess;
	    emit mineCount( mineguess );
	    if ( mine->isMined() )
		--minecount;
	} else if ( wasFlagged ) {
	    ++mineguess;
	    emit mineCount( mineguess );
	    if ( mine->isMined() )
		++minecount;
	}
    }

    updateCell( row, col );

    if ( !minecount && !mineguess || !nonminecount ) {
	emit gameOver( TRUE );
	setState( GameOver );
    }
}

void MineField::showMines()
{
    for ( int c = 0; c < numCols(); c++ )
	for ( int r = 0; r < numRows(); r++ ) {
	    Mine* mine = (Mine*)item( r, c );
	    if ( !mine )
		continue;
	    if ( mine->isMined() && mine->state() == Mine::Hidden )
		mine->setState( Mine::Mined );
	    if ( !mine->isMined() && mine->state() == Mine::Flagged )
		mine->setState( Mine::Wrong );

	    updateCell( r, c );
	}
}

void MineField::paletteChange( const QPalette &o )
{
    Mine::paletteChange();
    QTable::paletteChange( o );
}

void MineField::writeConfig(Config& cfg) const
{
    cfg.setGroup("Field");
    cfg.writeEntry("Level",lev);
    QString grid="";
    if ( stat == Playing ) {
	for ( int x = 0; x < numCols(); x++ )
	    for ( int y = 0; y < numRows(); y++ ) {
		char code='A'+(x*17+y*101)%21; // Reduce the urge to cheat
		Mine* mine = (Mine*)item( y, x );
		int st = (int)mine->state(); if ( mine->isMined() ) st+=5;
		grid += code + st;
	    }
    }
    cfg.writeEntry("Grid",grid);
}

void MineField::readConfig(Config& cfg)
{
    cfg.setGroup("Field");
    lev = cfg.readNumEntry("Level",1);
    setup(lev);
    flagAction = NoAction;
    ignoreClick = FALSE;
    currRow = currCol = 0;
    QString grid = cfg.readEntry("Grid");
    if ( !grid.isEmpty() ) {
	int i=0;
	minecount=0;
	mineguess=0;
	for ( int x = 0; x < numCols(); x++ ) {
	    for ( int y = 0; y < numRows(); y++ ) {
		char code='A'+(x*17+y*101)%21; // Reduce the urge to cheat
		int st = (char)(QChar)grid[i++]-code;
		Mine* mine = (Mine*)item( y, x );
		if ( st >= 5 ) {
		    st-=5;
		    mine->setMined(TRUE);
		    minecount++;
		    mineguess++;
		}
		mine->setState((Mine::MineState)st);
		switch ( mine->state() ) {
		  case Mine::Flagged:
		    if (mine->isMined())
			minecount--;
		    mineguess--;
		    break;
		  case Mine::Empty:
		    --nonminecount;
		}
	    }
	}
	for ( int x = 0; x < numCols(); x++ ) {
	    for ( int y = 0; y < numRows(); y++ ) {
		Mine* mine = (Mine*)item( y, x );
		if ( mine->state() == Mine::Empty )
		    mine->setHint(getHint(y,x));
	    }
	}
    }
    setState( Playing );
    emit mineCount( mineguess );
}

