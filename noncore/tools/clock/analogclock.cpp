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

#include "analogclock.h"

#include <qlayout.h>
#include <qpainter.h>
#include <qtopia/global.h>

#include <math.h>

const double deg2rad = 0.017453292519943295769;	// pi/180

AnalogClock::AnalogClock( QWidget *parent, const char *name )
    : QFrame( parent, name ), clear(false)
{
    setMinimumSize(50,50);
}

QSizePolicy AnalogClock::sizePolicy() const
{
    return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

void AnalogClock::drawContents( QPainter *p )
{

    QRect r = contentsRect();

    if ( r.width() < r.height() ) {
	r.setY( (r.height() - r.width())/2 );
	r.setHeight( r.width() );
    }

    QPoint l1( r.x() + r.width() / 2, r.y() + 2 );
    QPoint l2( r.x() + r.width() / 2, r.y() + 8 );
    QPoint center( r.x() + r.width() / 2, r.y() + r.height() / 2 );

    QBrush bgBrush = colorGroup().brush( QColorGroup::Background );

//    if ( clear ){
        p->fillRect ( r, bgBrush );


        // draw ticks
        p->setPen( QPen( colorGroup ( ). color ( QColorGroup::Text ), 1 ) );
        for ( int i = 0; i < 12; i++ )
            p->drawLine( rotate( center, l1, i * 30 ), rotate( center, l2, i * 30 ) );
//    }else {
//        p->setBrush( bgBrush );
//        drawPointers ( p, r, colorGroup ( ). color ( QColorGroup::Background ), prevTime, &currTime );
//    }

    QColor col = colorGroup().color( QColorGroup::Text );
    p->setBrush( col );
    drawPointers ( p, r, col, currTime );


    prevTime = currTime;
}

void AnalogClock::drawPointers ( QPainter *p, const QRect &r, const QColor &c, const QTime &t, const QTime *t2 )
{
    QPoint center = r. center ( );


    QPoint h1( center. x ( ), r. y ( ) + r. height ( ) / 4 );
    QPoint h2( center. x ( ), center. y ( ) );

    QPoint m1( center. x ( ), r.y() + r.height() / 8 );
    QPoint m2( center. x ( ), center. y ( ) );

    QPoint s1( center. x ( ), r. y ( ) + 8 );
    QPoint s2( center. x ( ), center. y ( ) );


    if ( !t2 || ( t. minute ( ) != t2-> minute ( ) || t. hour ( ) != t2-> hour ( ))) {
        // draw hour pointer
        h1 = rotate( center, h1, 30 * ( t.hour() % 12 ) + t.minute() / 2 );
        h2 = rotate( center, h2, 30 * ( t.hour() % 12 ) + t.minute() / 2 );
        p-> setPen ( QPen ( c, 3 ));
        drawHand( p, h1, h2 );
    }

    if ( !t2 || ( t. minute ( ) != t2-> minute ( ))) {
        // draw minute pointer
        m1 = rotate( center, m1, t.minute() * 6 );
        m2 = rotate( center, m2, t.minute() * 6 );
        p-> setPen ( QPen ( c, 2 ));
        drawHand( p, m1, m2 );
    }

    if ( !t2 || ( t. second ( ) != t2-> second ( ))) {
        // draw second pointer
        s1 = rotate( center, s1, t.second() * 6 );
        s2 = rotate( center, s2, t.second() * 6 );
        p-> setPen ( QPen ( c, 1 ));
        p-> drawLine ( s1, s2 );
    }
}


// Dijkstra's bisection algorithm to find the square root as an integer.

static uint int_sqrt(uint n)
{
    if ( n >= UINT_MAX>>2 ) // n must be in the range 0...UINT_MAX/2-1
	return 2*int_sqrt( n/4 );
    uint h, p= 0, q= 1, r= n;
    while ( q <= n )
	q <<= 2;
    while ( q != 1 ) {
	q >>= 2;
	h= p + q;
	p >>= 1;
	if ( r >= h ) {
	    p += q;
	    r -= h;
	}
    }
    return p;
}

void AnalogClock::drawHand( QPainter *p, QPoint p1, QPoint p2 )
{
    int hw = 7;
    if ( contentsRect().height() < 100 )
	hw = 5;

    int dx = p2.x() - p1.x();
    int dy = p2.y() - p1.y();
    int w = dx*dx+dy*dy;
    int ix,iy;
    w = int_sqrt(w*256);
    iy = w ? (hw * dy * 16)/ w : dy ? 0 : hw;
    ix = w ? (hw * dx * 16)/ w : dx ? 0 : hw;

    // rounding dependent on sign
    int nix, niy;
    if ( ix < 0 ) {
	nix = ix/2;
	ix = (ix-1)/2;
    } else {
	nix = (ix+1)/2;
	ix = ix/2;
    }
    if ( iy < 0 ) {
	niy = iy/2;
	iy = (iy-1)/2;
    } else {
	niy = (iy+1)/2;
	iy = iy/2;
    }

    QPointArray pa(4);
    pa[0] = p1;
    pa[1] = QPoint( p2.x()+iy, p2.y()-nix );
    pa[2] = QPoint( p2.x()-niy, p2.y()+ix );
    pa[3] = p1;

    p->drawPolygon( pa );
}

void AnalogClock::display( const QTime& t )
{
    currTime = t;
    clear = false;
    repaint( false );
    clear = true;
}

QPoint AnalogClock::rotate( QPoint c, QPoint p, int a )
{
    double angle = deg2rad * ( - a + 180 );
    double nx = c.x() - ( p.x() - c.x() ) * cos( angle ) -
		( p.y() - c.y() ) * sin( angle );
    double ny = c.y() - ( p.y() - c.y() ) * cos( angle ) +
		( p.x() - c.x() ) * sin( angle );
    return QPoint( int(nx), int(ny) );
}
