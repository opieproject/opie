/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "previewwidget.h"
#include "zcameraio.h"

#include <opie2/odebug.h>

#include <assert.h>

PreviewWidget::PreviewWidget( QWidget * parent, const char * name, WFlags f )
           :QLabel( parent, name, f )
{
    #ifndef QT_NO_DEBUG
    setBackgroundColor( QColor( 255, 0, 0 ) );
    #else
    setBackgroundMode( NoBackground );
    #endif

    #ifndef QT_NO_DEBUG
    if ( ZCameraIO::instance()->isOpen() ) startTimer( 1500 );
    #else
    if ( ZCameraIO::instance()->isOpen() ) startTimer( 200 );
    #endif
};


PreviewWidget::~PreviewWidget()
{
}


void PreviewWidget::resizeEvent( QResizeEvent* e )
{
    QLabel::resizeEvent( e );
    int w = e->size().width();
    int h = e->size().height();
    ZCameraIO::instance()->setCaptureFrame( e->size().width(),
                                          e->size().height(),
                                          256,
                                          w < h );
}


void PreviewWidget::timerEvent( QTimerEvent*  )
{
    //QDirectPainter fb( this );
    //ZCameraIO::instance()->snapshot( fb.frameBuffer() );

    if ( ZCameraIO::instance()->snapshot( &i ) )
    {
        p.convertFromImage( i );
        setPixmap( p );
    }
}


void PreviewWidget::mousePressEvent( QMouseEvent* )
{
    emit contextMenuRequested();
}


void PreviewWidget::setRefreshingRate( int ms )
{
    killTimers();
    if ( ms )
        startTimer( ms );
}


void PreviewWidget::refresh()
{
    QTimerEvent t( 10 ); // event id is meaningless in this case
    timerEvent( &t );
}
