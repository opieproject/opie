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


static const int maxGrid = 28;
static const int minGrid = 9;



class Mine : public Qt
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

    Mine( MineField* );
    void paint( QPainter * p, const QColorGroup & cg, const QRect & cr );

    QSize sizeHint() const { return QSize( maxGrid, maxGrid ); }

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
    MineField *field;
    
    static QPixmap* knownField;
    static QPixmap* unknownField;
    static QPixmap* flag_pix;
    static QPixmap* mine_pix;
};

QPixmap* Mine::knownField = 0;
QPixmap* Mine::unknownField = 0;
QPixmap* Mine::flag_pix = 0;
QPixmap* Mine::mine_pix = 0;

Mine::Mine( MineField *f )
{
    mined = FALSE;
    st = Hidden;
    hint = 0;
    field = f;
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

void Mine::paint( QPainter* p, const QColorGroup &cg, const QRect& cr )
{
    int x = cr.x();
    int y = cr.y();
    if ( !knownField ) {
	knownField = new QPixmap( cr.width(), cr.height() );
	QPainter pp( knownField );
	QBrush br( cg.button().dark(115) );
	qDrawWinButton( &pp, cr, cg, TRUE, &br );
    }

    const int pmmarg=cr.width()/5;

    if ( !unknownField ) {
	unknownField = new QPixmap( cr.width(), cr.height() );
	QPainter pp( unknownField );
	QBrush br( cg.button() );
	qDrawWinButton( &pp, cr, cg, FALSE, &br );
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
	p->drawPixmap( x, y, *unknownField );
	break;
    case Empty:
	p->drawPixmap( x, y, *knownField );
	if ( hint > 0 ) {
	    switch( hint ) {
	    case 1:
		p->setPen( blue );
		break;
	    case 2:
		p->setPen( green.dark() );
		break;
	    case 3:
		p->setPen( red );
		break;
	    case 4:
		p->setPen( darkYellow.dark() );
		break;
	    case 5:
		p->setPen( darkMagenta );
		break;
	    case 6:
		p->setPen( darkRed );
		break;
	    default:
		p->setPen( black );
		break;
	    }
	    p->drawText( cr, AlignHCenter | AlignVCenter, QString::number( hint ) );
	}
	break;
    case Mined:
	p->drawPixmap( x, y, *knownField );
	p->drawPixmap( x+pmmarg, y+pmmarg, *mine_pix );
	break;
    case Exploded:
	p->drawPixmap( x, y, *knownField );
	p->drawPixmap( x+pmmarg, y+pmmarg, *mine_pix );
	p->setPen( red );
	p->drawText( cr, AlignHCenter | AlignVCenter, "X" );
	break;
    case Flagged:
	p->drawPixmap( x, y, *unknownField );
	p->drawPixmap( x+pmmarg, y+pmmarg, *flag_pix );
	break;
#ifdef MARK_UNSURE
    case Unsure:
	p->drawPixmap( x, y, *unknownField );
	p->drawText( cr, AlignHCenter | AlignVCenter, "?" );
	break;
#endif
    case Wrong:
	p->drawPixmap( x, y, *unknownField );
	p->drawPixmap( x+pmmarg, y+pmmarg, *flag_pix );
	p->setPen( red );
	p->drawText( cr, AlignHCenter | AlignVCenter, "X" );
	break;
    }

    p->restore();
}

/*
  MineField implementation
*/

MineField::MineField( QWidget* parent, const char* name )
: QScrollView( parent, name )
{
    setState( GameOver );

    setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum ) );
    
    setFocusPolicy( QWidget::NoFocus );

    holdTimer = new QTimer( this );
    connect( holdTimer, SIGNAL( timeout() ), this, SLOT( held() ) );

    flagAction = NoAction;
    ignoreClick = FALSE;
    currRow = currCol = -1;
    minecount=0;
    mineguess=0;
    nonminecount=0;
    cellSize = -1;
}

MineField::~MineField()
{
    for ( int i = 0; i < numCols*numRows; i++ )
	delete mines[i];
    delete[] mines;
}

void MineField::setState( State st )
{
    stat = st;
}

void MineField::setup( int level )
{
    lev = level;
    setState( Waiting );
    //viewport()->setUpdatesEnabled( FALSE );

    int i;
    for ( i = 0; i < numCols*numRows; i++ )
	delete mines[i];
    delete[] mines;

    switch( lev ) {
    case 1:
	numRows = 9 ;
	numCols = 9 ;
	minecount = 12;
	break;
    case 2:
	numRows = 16;
	numCols = 16;
	minecount = 45;
	break;
    case 3:
	numCols = 18;
	numRows = 18;
	minecount = 66 ;
	break;
    }
    mines = new (Mine*)[numRows*numCols];
    for ( i = 0; i < numCols*numRows; i++ )
	mines[i] = new Mine( this );

    
    nonminecount = numRows*numCols - minecount;
    mineguess = minecount;
    emit mineCount( mineguess );
    Mine::paletteChange();

    if ( availableRect.isValid() )
	setCellSize(findCellSize());
    //    viewport()->setUpdatesEnabled( TRUE );
    //viewport()->repaint( TRUE );
    updateContents( 0, 0, numCols*cellSize, numRows*cellSize );
    updateGeometry();
}

void MineField::drawContents( QPainter * p, int clipx, int clipy, int clipw, int cliph ) 
{
    int c1 = clipx / cellSize;
    int c2 = ( clipx + clipw - 1 ) / cellSize;
    int r1 = clipy / cellSize;
    int r2 = ( clipy + cliph - 1 ) / cellSize;
    
    for ( int c = c1; c <= c2 ; c++ ) {
	for ( int r = r1; r <= r2 ; r++ ) {
	    int x = c * cellSize;
	    int y = r * cellSize;
	    Mine *m = mine( r, c );
	    if ( m )
		m->paint( p, colorGroup(), QRect(x, y, cellSize, cellSize ) );
	}
    }
}


// Chicken and egg problem: We need to know how big the parent is
// before we can decide how big to make the table.

void MineField::setAvailableRect( const QRect &r )
{
    availableRect = r;
    int newCellSize = findCellSize();
    if ( newCellSize != cellSize ) {
	viewport()->setUpdatesEnabled( FALSE );
	setCellSize( newCellSize );
	viewport()->setUpdatesEnabled( TRUE );
	viewport()->repaint( TRUE );
    }
}

int MineField::findCellSize()
{
    int w = availableRect.width() - 1;
    int h = availableRect.height() - 1;
    int cellsize;
    
    cellsize = QMIN( w/numCols, h/numRows );
    cellsize = QMIN( QMAX( cellsize, minGrid ), maxGrid );
    return cellsize;
}


void MineField::setCellSize( int cellsize )
{
    cellSize = cellsize;
    
    int w = availableRect.width();
    int h = availableRect.height();
    
    int w2 = cellsize*numCols;
    int h2 = cellsize*numRows;
    
    resizeContents( w2, h2 );
    
    int b = 2;
    
    setGeometry( availableRect.x() + (w-w2)/2, availableRect.y() + (h-h2)/2,
		 QMIN(w,w2+b), QMIN(h,h2+b) );
}


void MineField::placeMines()
{
    int mines = minecount;
    while ( mines ) {
	int col = int((double(rand()) / double(RAND_MAX)) * numCols);
	int row = int((double(rand()) / double(RAND_MAX)) * numRows);

	Mine* m = mine( row, col );

	if ( m && !m->isMined() && m->state() == Mine::Hidden ) {
	    m->setMined( TRUE );
	    mines--;
	}
    }
}


void MineField::updateCell( int r, int c )
{
    updateContents( c*cellSize, r*cellSize, cellSize, cellSize );
}


void MineField::contentsMousePressEvent( QMouseEvent* e )
{
    int c = e->pos().x() / cellSize;
    int r = e->pos().y() / cellSize;
    if ( onBoard( r, c ) )
	cellPressed( r, c );
    else
	currCol = currRow = -1;
}

void MineField::contentsMouseReleaseEvent( QMouseEvent* e )
{
    int c = e->pos().x() / cellSize;
    int r = e->pos().y() / cellSize;
    if ( onBoard( r, c ) && c == currCol && r == currRow )
	cellClicked( r, c );
    
    
    if ( flagAction == FlagNext ) {
	flagAction = NoAction;
    }
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
	    Mine* m = mine( r, c );
	    if ( m && m->isMined() )
		hint++;
	}

    return hint;
}

void MineField::setHint( int row, int col )
{
    Mine *m = mine( row, col );
    if ( !m )
	return;

    int hint = getHint( row, col );

    if ( !hint ) {
	for ( int c = col-1; c <= col+1; c++ )
	    for ( int r = row-1; r <= row+1; r++ ) {
		Mine* m = mine( r, c );
		if ( m && m->state() == Mine::Hidden ) {
		    m->activate( TRUE );
		    nonminecount--;
		    setHint( r, c );
		    updateCell( r, c );
		}
	    }
    }

    m->setHint( hint );
    updateCell( row, col );
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
	Mine* m = mine( row, col );
	if ( !m )
	    return;
	m->setState( Mine::Empty );
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
    Mine* m = mine( row, col );
    if ( !m )
	return;

    bool wasFlagged = m->state() == Mine::Flagged;
    bool wasEmpty =  m->state() == Mine::Empty;
    
    m->activate( flagAction == NoAction );

    if ( m->state() == Mine::Exploded ) {
	emit gameOver( FALSE );
	setState( GameOver );
	return;
    } else if ( m->state() == Mine::Empty ) {
	setHint( row, col );
	if ( !wasEmpty )
	    nonminecount--;
    }

    if ( flagAction != NoAction ) {
	if ( m->state() == Mine::Flagged ) {
	    --mineguess;
	    emit mineCount( mineguess );
	    if ( m->isMined() )
		--minecount;
	} else if ( wasFlagged ) {
	    ++mineguess;
	    emit mineCount( mineguess );
	    if ( m->isMined() )
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
    for ( int c = 0; c < numCols; c++ )
	for ( int r = 0; r < numRows; r++ ) {
	    Mine* m = mine( r, c );
	    if ( !m )
		continue;
	    if ( m->isMined() && m->state() == Mine::Hidden )
		m->setState( Mine::Mined );
	    if ( !m->isMined() && m->state() == Mine::Flagged )
		m->setState( Mine::Wrong );

	    updateCell( r, c );
	}
}

void MineField::paletteChange( const QPalette &o )
{
    Mine::paletteChange();
    QScrollView::paletteChange( o );
}

void MineField::writeConfig(Config& cfg) const
{
    cfg.setGroup("Field");
    cfg.writeEntry("Level",lev);
    QString grid="";
    if ( stat == Playing ) {
	for ( int x = 0; x < numCols; x++ )
	    for ( int y = 0; y < numRows; y++ ) {
		char code='A'+(x*17+y*101)%21; // Reduce the urge to cheat
		const Mine* m = mine( y, x );
		int st = (int)m->state(); if ( m->isMined() ) st+=5;
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
	for ( int x = 0; x < numCols; x++ ) {
	    for ( int y = 0; y < numRows; y++ ) {
		char code='A'+(x*17+y*101)%21; // Reduce the urge to cheat
		int st = (char)(QChar)grid[i++]-code;
		Mine* m = mine( y, x );
		if ( st >= 5 ) {
		    st-=5;
		    m->setMined(TRUE);
		    minecount++;
		    mineguess++;
		}
		m->setState((Mine::MineState)st);
		switch ( m->state() ) {
		  case Mine::Flagged:
		    if (m->isMined())
			minecount--;
		    mineguess--;
		    break;
		  case Mine::Empty:
		    --nonminecount;
		    break;
		default:
		    break;
		}
	    }
	}
	for ( int x = 0; x < numCols; x++ ) {
	    for ( int y = 0; y < numRows; y++ ) {
		Mine* m = mine( y, x );
		if ( m->state() == Mine::Empty )
		    m->setHint(getHint(y,x));
	    }
	}
    }
    setState( Playing );
    emit mineCount( mineguess );
}

