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

PreviewWidget::PreviewWidget( QWidget * parent, const char * name, WFlags f )
           :QLabel( parent, name, f )
{
    setFixedSize( QSize( 240, 160 ) );
    setBackgroundMode( NoBackground );

    startTimer( 150 );
};


PreviewWidget::~PreviewWidget()
{
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

