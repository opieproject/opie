/*
                             This file is part of the OPIE Project
               =.            Copyright (c)  2002 Maximilian Reiss <harlekin@handhelds.org>
             .=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:       
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/
#include "calibration.h"

#include <qpainter.h>
#include <qpalette.h>

#define BRD 2

Calibration::Calibration ( QWidget *parent, const char *name, WFlags fl )
	: QWidget ( parent, name, fl )
{
	m_scale = QSize ( 256, 256 );
	m_steps = 5;
	m_dragged = -1;
	m_interval = 5;
	
	m_p [0] = QPoint ( 0, 0 );
	m_p [1] = QPoint ( 255, 255 );
}

Calibration::~Calibration ( )
{
}

void Calibration::setScale ( const QSize &s )
{
	if ( s. width ( ) < 1 || s. height ( ) < 1 )
		return;

	m_scale = s;	
	checkPoints ( );
	
	update ( );
}

QSize Calibration::scale ( ) const
{
	return m_scale;
}

void Calibration::setLineSteps ( int steps )
{
	if ( m_steps < 2 )
		return;

	m_steps = steps;
	update ( );
}

int Calibration::lineSteps ( ) const
{
	return m_steps;
}

void Calibration::setInterval ( int iv )
{
	if ( iv < 1 )
	return;

	m_interval = iv;
//	update ( );
}

int Calibration::interval ( ) const
{
	return m_interval;
}

void Calibration::setStartPoint ( const QPoint &p )
{
	m_p [0] = p;
	checkPoints ( );	
	update ( );
}

QPoint Calibration::startPoint ( ) const
{
	return m_p [0];
}

void Calibration::setEndPoint ( const QPoint &p )
{
	m_p [1] = p;
	checkPoints ( );
	update ( );
}

QPoint Calibration::endPoint ( ) const
{
	return m_p [1];
}

void Calibration::checkPoints ( )
{
	int dx = m_scale. width ( );
	int dy = m_scale. height ( );

	if ( m_p [1]. x ( ) >= dx )
		m_p [1]. setX ( dx - 1 );
	if ( m_p [0]. x ( ) > m_p [1]. x ( ))
		m_p [0]. setX ( m_p [1]. x ( ));

	if ( m_p [1]. y ( ) >= dy )
		m_p [1]. setY ( dy - 1 );
	if ( m_p [0]. y ( ) > m_p [1]. y ( ))
		m_p [0]. setY ( m_p [1]. y ( ));
}


#define SCALEX(x)   (BRD+x*(width()- 2*BRD)/m_scale.width())
#define SCALEY(y)   (BRD+y*(height()-2*BRD)/m_scale.height())


static QRect around ( int x, int y )
{
	return QRect ( x - BRD, y - BRD, 2 * BRD + 1, 2 * BRD + 1 );	
}

void Calibration::mousePressEvent ( QMouseEvent *e )
{
	if ( e-> button ( ) != LeftButton )
		return QWidget::mousePressEvent ( e );

	int olddragged = m_dragged;
	int x [2], y [2];

	m_dragged = -1;
	for ( int i = 0; i < 2; i++ ) {
		x [i] = SCALEX( m_p [i]. x ( ));
		y [i] = SCALEY( m_p [i]. y ( ));

		if (( QABS( e-> x ( ) - x [i] ) <= BRD ) &&
		    ( QABS( e-> y ( ) - y [i] ) <= BRD )) {
			m_dragged = i;
			break;
		}
	}

	if ( m_dragged != olddragged ) {
		QRect r;
	
		if ( olddragged >= 0 )
			r |= around ( x [olddragged], y [olddragged] );	
		if ( m_dragged >= 0 ) 
			r |= around ( x [m_dragged], y [m_dragged] );	
		repaint ( r );
	}	
}

void Calibration::mouseMoveEvent ( QMouseEvent *e )
{
	if ( m_dragged < 0 )
		return;
		
	QPoint n [2];
	
	n [m_dragged]. setX (( e-> x ( ) - BRD ) * m_scale. width ( ) / ( width ( ) - 2 * BRD ));
	n [m_dragged]. setY (( e-> y ( ) - BRD ) * m_scale. height ( ) / ( height ( ) - 2 * BRD ));	
	n [1 - m_dragged] = m_p [1 - m_dragged];
	
	if (( n [0]. x ( ) > n [1]. x ( )) || ( n [m_dragged]. x ( ) < 0 ) || ( n [m_dragged]. x ( ) >= m_scale. width ( )))
		n [m_dragged]. setX ( m_p [m_dragged]. x ( ));
	if (( n [0]. y ( ) > n [1]. y ( )) || ( n [m_dragged]. y ( ) < 0 ) || ( n [m_dragged]. y ( ) >= m_scale. height ( )))
		n [m_dragged]. setY ( m_p [m_dragged]. y ( ));
		
	QRect r;	
	int ox [2], oy [2], nx [2], ny [2];	
	
	for ( int i = 0; i < 2; i++ ) {
		nx [i] = SCALEX( n [i]. x ( ));
		ny [i] = SCALEY( n [i]. y ( ));
		ox [i] = SCALEX( m_p [i]. x ( ));
		oy [i] = SCALEY( m_p [i]. y ( ));
	
		if ( n [i] != m_p [i] )	{
			r |= around ( nx [i], ny [i] );	
			r |= around ( ox [i], oy [i] );	
			m_p [i] = n [i];
			
			if ( i == 0 ) {
				r |= QRect ( 0, 0, nx [0] - 0 + 1, ny [0] - 0 + 1 );
				r |= QRect ( 0, 0, ox [0] - 0 + 1, oy [0] - 0 + 1 );
			}
			else if ( i == 1 ) {
				r |= QRect ( nx [1], ny [1], width ( ) - nx [1], height ( ) - ny [1] ); 
				r |= QRect ( ox [1], oy [1], width ( ) - ox [1], height ( ) - oy [1] ); 
			}
		}
	}
	if ( r. isValid ( )) {
		r |= QRect ( nx [0], ny [0], nx [1] - nx [0] + 1, ny [1] - ny [0] + 1 );
		r |= QRect ( ox [0], oy [0], ox [1] - ox [0] + 1, oy [1] - oy [0] + 1 );
		
		repaint ( r );
	}
}

void Calibration::mouseReleaseEvent ( QMouseEvent *e )
{
	if ( e-> button ( ) != LeftButton )
		return QWidget::mouseReleaseEvent ( e );
	
	if ( m_dragged < 0 )
		return;
	
	int x = SCALEX( m_p [m_dragged]. x ( ));
	int y = SCALEY( m_p [m_dragged]. y ( ));
	m_dragged = -1;
	
	repaint ( around ( x, y ));
}

void Calibration::paintEvent ( QPaintEvent * )
{
	QPainter p ( this );
	QColorGroup g = colorGroup ( );

	int x0 = SCALEX( m_p [0]. x ( ));
	int y0 = SCALEY( m_p [0]. y ( ));
	int x1 = SCALEX( m_p [1]. x ( ));
	int y1 = SCALEY( m_p [1]. y ( ));
			
	int dx = x1 - x0;
	int dy = y1 - y0;
	
	int ex = x0, ey = y0;
	
	p. setPen ( g. highlight ( ));
	
	p. drawLine ( BRD, BRD, ex, BRD );
	p. drawLine ( ex, BRD, ex, ey );
	
	for ( int i = 1; i < m_steps; i++ ) {
		int fx = x0 + dx * i / m_steps;
		int fy = y0 + dy * i / ( m_steps - 1 );
		
		p. drawLine ( ex, ey, fx, ey );
		p. drawLine ( fx, ey, fx, fy );
				
		ex = fx;
		ey = fy;
	}	
	
	p. drawLine ( ex, ey, width ( ) - 1 - BRD, ey );
	
	p. fillRect ( around ( x0, y0 ), m_dragged == 0 ? g. highlightedText ( ) : g. text ( ));
	p. fillRect ( around ( x1, y1 ), m_dragged == 1 ? g. highlightedText ( ) : g. text ( ));
	
	p. setPen ( g. text ( ));
	p. drawText ( QRect ( BRD, BRD, width ( ) - 2*BRD, height() - 2*BRD ), AlignTop | AlignRight, tr( "%1 Steps" ). arg ( m_steps ));
}

