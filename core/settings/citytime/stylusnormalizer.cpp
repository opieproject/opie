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

#include <qpoint.h>
#include <qtimer.h>

#include "stylusnormalizer.h"

static const int FLUSHTIME = 100;

_StylusEvent::_StylusEvent( const QPoint& newPt )
    : _pt( newPt ),
      _t( QTime::currentTime() )
{
}

_StylusEvent::~_StylusEvent()
{
}

StylusNormalizer::StylusNormalizer( QWidget *parent, const char* name )
    : QWidget( parent, name ),
      _next( 0 ),
      bFirst( true )
{
    // initialize _ptList
    int i;
    for (i = 0; i < SAMPLES; i++ ) {
        _ptList[i].setPoint( -1, -1 );
    }
    _tExpire = new QTimer( this );
    QObject::connect( _tExpire, SIGNAL( timeout() ),
                      this, SLOT( slotAveragePoint() ) );
}

StylusNormalizer::~StylusNormalizer()
{
}

void StylusNormalizer::addEvent( const QPoint& pt )
{
    _ptList[_next].setPoint( pt );
    _ptList[_next++].setTime( QTime::currentTime() );
    if ( _next >= SAMPLES ) {
        _next = 0;
    }
    // make a single mouse click work
    if ( bFirst ) {
        slotAveragePoint();
        bFirst = false;
    }
}

void StylusNormalizer::slotAveragePoint( void )
{
    QPoint pt( 0, 0 );
    QTime tCurr = QTime::currentTime();
    int i,
        size;
    size = 0;
    for ( i = 0; i < SAMPLES; i++ ) {
        if ( ( (_ptList[i]).time().msecsTo( tCurr ) < FLUSHTIME ) &&
             ( _ptList[i].point() != QPoint( -1, -1 ) ) ) {
            pt += _ptList[i].point();
            size++;
        }
    }
    if ( size > 0 )
        emit signalNewPoint( pt /= size );
}

void StylusNormalizer::start( void )
{
    _tExpire->start( FLUSHTIME );
}

void StylusNormalizer::stop( void )
{
    _tExpire->stop();
    bFirst = true;
}