#include <aconf.h>

#include "QPEOutputDev.h"

#include <qapplication.h>
#include <qlabel.h>
#include "qbusybar.h"


QPEOutputDev::QPEOutputDev ( QWidget *parent, const char *name ) : QOutputDev ( parent, name )
{
	m_counter = new QLabel ( this );
	m_counter-> setAlignment ( AlignCenter | SingleLine );

	m_busybar = new QBusyBar ( this );
	m_busybar-> setParameters ( 12, 8, 200 );
	m_busybar-> hide ( );
	
	setHScrollBarMode ( AlwaysOn );
	
	m_isbusy = false;
	
	m_selectiondrag = false;
	
	setFocusPolicy ( WheelFocus );
}

void QPEOutputDev::startPage ( int pn, GfxState *st )
{
	m_selection = QRect ( );
	m_selectiondrag = false;
	
	QOutputDev::startPage ( pn, st );
}


void QPEOutputDev::setPageCount ( int actp, int maxp )
{
	m_counter-> setText ( QString ( "%1 / %2" ). arg ( actp ). arg ( maxp ));
}

void QPEOutputDev::setBusy ( bool b )
{
	if ( b != m_isbusy ) {
		if ( b ) {
			m_busybar-> beginBusy ( );
			m_busybar-> show ( );
			m_counter-> hide ( );
		}
		else {
			m_counter-> show ( );	
			m_busybar-> hide ( );
			m_busybar-> endBusy ( );
		}		
		m_isbusy = b;
	}
}

bool QPEOutputDev::isBusy ( ) const
{
	return m_isbusy;
}

void QPEOutputDev::setHBarGeometry ( QScrollBar &hbar, int x, int y, int w, int h )
{
	int delta = w * 3 / 10;
	
	m_counter-> setGeometry ( x, y, delta, h );
	m_busybar-> setGeometry ( x, y, delta, h );
	hbar. setGeometry ( x + delta, y, w - delta, h );
}


void QPEOutputDev::keyPressEvent ( QKeyEvent *e )
{
	switch ( e-> key ( )) {
		case Key_Left:
			scrollBy ( -10, 0 );
			break;
		case Key_Right:
			scrollBy ( 10, 0 );
			break;
		case Key_Up:
			scrollBy ( 0, -10 );
			break;
		case Key_Down:
			scrollBy ( 0, 10 );
			break;
	
		default:
			QOutputDev::keyPressEvent ( e );
	}
}


void QPEOutputDev::drawContents ( QPainter *p, int clipx, int clipy, int clipw, int cliph )
{
	QOutputDev::drawContents ( p, clipx, clipy, clipw, cliph );

	if ( m_selection. isValid ( )) {
		QRect clip ( clipx, clipy, clipw, cliph );
			
		if ( m_selection. intersects ( clip )) {
			RasterOp rop = p-> rasterOp ( );
		
			p-> setRasterOp ( XorROP );
			p-> fillRect ( m_selection & clip, white );
			p-> setRasterOp ( rop );
		}
	}
}


QRect QPEOutputDev::selection ( ) const
{
	return m_selection;
}


void QPEOutputDev::setSelection ( const QRect &r, bool scrollto )
{
	QRect oldsel = m_selection;
	m_selection = r;

	QArray<QRect> urects = ( QRegion ( oldsel ) ^ QRegion ( m_selection )). rects ( );

	for ( uint i = 0; i < urects. count ( ); i++ )
		repaintContents ( urects [i] );
	
	if ( scrollto ) {
		QPoint c = r. center ( );
	
		ensureVisible ( c. x ( ), c. y ( ), r. width ( ) / 2 + 5, r. height ( ) / 2 + 5 );
	}
	
	if ( !m_selectiondrag )
		emit selectionChanged ( m_selection );
}


void QPEOutputDev::viewportMousePressEvent ( QMouseEvent *e )
{
	if ( e-> button ( ) == LeftButton ) {
		m_selectionstart =  e-> pos ( ) + QPoint ( contentsX ( ), contentsY ( ));
		m_selectioncursor = m_selectionstart;
		m_selectiondrag = true;
		
		setSelection ( QRect ( m_selectionstart, QSize ( 0, 0 )), true );
    }
}

void QPEOutputDev::viewportMouseMoveEvent ( QMouseEvent *e )
{
	if ( e-> state ( ) & LeftButton ) {
		if ( m_selectiondrag ) {
			QPoint to ( e-> pos ( ) + QPoint ( contentsX ( ), contentsY ( )));

			if ( to != m_selectioncursor ) {
				setSelection ( QRect ( m_selectionstart, to ). normalize ( ), false );
				m_selectioncursor = to;
			}
			ensureVisible ( m_selectioncursor. x ( ), m_selectioncursor. y ( ), 5, 5 );
		}
	}
}


void QPEOutputDev::viewportMouseReleaseEvent ( QMouseEvent *e )
{
	if ( e-> button ( ) == LeftButton ) {
		if ( m_selectiondrag ) {
			m_selectiondrag = false;
			
			setSelection ( selection ( ), false ); // emit signal
		} 
		else {
			setSelection ( QRect ( 0, 0, 0, 0 ), false );
		}		
	}
}

