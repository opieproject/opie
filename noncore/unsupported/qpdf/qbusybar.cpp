#include <qapplication.h>
#include <qtimer.h>
#include <qpainter.h>

#include "qbusybar.h"



QBusyBar::QBusyBar ( QWidget *parent, const char *name, int flags ) : QWidget ( parent, name, flags | WRepaintNoErase )
{
	m_busy = 0;

	m_div = 0;
	m_pos = 0;
	m_fade = 0;
	m_fadecols = 0;
	m_speed = 500;
	
	m_timer = new QTimer ( this );	
	connect ( m_timer, SIGNAL( timeout ( )), this, SLOT( slotTimeout ( )));	

	setParameters ( 12, 8, 200 );
}

void QBusyBar::setParameters ( int d, int s, int v )	
{	
	bool running = m_timer-> isActive ( );
	
	if ( running )
		m_timer-> stop ( );

	m_div = d;
	m_speed = v;

	delete [] m_fadecols;
	m_fade = s;
	m_fadecols = new QColor [m_fade];

	int rt, gt, bt;
	int rf, gf, bf;

	colorGroup ( ). color ( QColorGroup::Highlight ). rgb ( &rf, &gf, &bf );
	colorGroup ( ). color ( QColorGroup::Background ). rgb ( &rt, &gt, &bt );
	
	for ( int i = 0; i < s; i++ ) 
		m_fadecols [i]. setRgb ( rf + ( rt - rf ) * i / s, gf + ( gt - gf ) * i / s, bf + ( bt - bf ) * i / s );
		
	if ( running ) {
		m_pos = 0;
		m_timer-> start ( m_speed );
	}
}

QBusyBar::~QBusyBar ( )
{
}

bool QBusyBar::isBusy ( ) const
{
	return m_busy;
}

void QBusyBar::beginBusy ( )
{
	setBusy ( true );
}

void QBusyBar::endBusy ( )
{
	setBusy ( false );
}

void QBusyBar::setBusy ( bool b )
{
	int busy = m_busy + ( b ? 1 : -1 );
	
	if ( busy < 0 )
		busy = 0;
	
	if (( m_busy == 0 ) && ( busy > 0 )) { // Changed state	to on
		m_pos = 0;
		m_timer-> start ( m_speed );
		update ( );		
	}
	else if (( m_busy > 0 ) && ( busy == 0 )) { // Changed state to off
		m_timer-> stop ( );
		update ( );	
	}
	
	m_busy = busy;
}

void QBusyBar::slotTimeout ( )
{
	m_pos++;
	m_pos %= ( 2 * ( m_fade + m_div ));

	update ( );
}

void QBusyBar::paintEvent ( QPaintEvent *e )
{
	QPainter p ( this );

	QRect clip = e-> rect ( );

	int x  = 0;
	int dx = width ( ) / m_div;
	int y  = clip. top ( ); 
	int dy = clip. height ( );

	if ( m_busy ) {
		int dir = ( m_pos < ( m_fade + m_div )) ? 1 : -1;
		int pos = ( dir > 0 ) ? m_pos : ( 2 * ( m_div + m_fade )) - m_pos - m_fade - 1;

		for ( int i = 0; i < m_div; i++ ) {
			int ind = ( pos - i ) * dir;
			if (( ind < 0 ) || ( ind >= m_fade ))
				ind = m_fade - 1;
			
			if ((( x + dx ) > clip. left ( )) || ( x < clip. right ( )))
				p. fillRect ( x, y, ( i < ( m_div - 1 )) ? dx : width ( ) - x, dy, m_fadecols [ind] );	
			x += dx;
		}
	}
	else {
		p. fillRect ( e-> rect ( ), m_fadecols [m_fade - 1] );
	}		
}



