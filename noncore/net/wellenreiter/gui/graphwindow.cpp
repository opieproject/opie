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

#include "graphwindow.h"

#include <qpainter.h>
#include <qpixmap.h>
#include <qtimer.h>

MFrequencySpectrum::MFrequencySpectrum( int channels, QWidget* parent, const char* name, WFlags f)
                   :QWidget( parent, name,f ), _channels( channels )
{
    _values = new int[_channels];
    _dirty = new bool[_channels];
    for ( int i = 0; i < channels; ++i )
    {   _values[i] = 0;
        _dirty[i] = true;
    }

    // we draw everything on our own
    setBackgroundMode( QWidget::NoBackground );
}


void MFrequencySpectrum::drawTopLine( QPainter* p, int x, int y, int width, const QColor& c )
{
    p->setPen( c.light() );
    p->drawLine( x, y, x+width-1, y );
}


void MFrequencySpectrum::drawBottomLine( QPainter* p, int x, int y, int width, const QColor& c )
{
    p->setPen( c.dark() );
    p->drawLine( x, y, x+width-1, y );
}


void MFrequencySpectrum::drawLine( QPainter* p, int x, int y, int width, const QColor& c )
{
    p->setPen( c.light() );
    p->drawPoint( x++, y );
    p->setPen( c );
    p->drawLine( x, y, x+width-2, y );
    p->setPen( c.dark() );
    p->drawPoint( x+width-1, y );
}


void MFrequencySpectrum::drawBar( QPainter* p, int x, int y, int width, int height, int maxheight )
{
    int h1 = 133; int h2 = 0;
    int s1 = 200; int s2 = 255;
    int v1 = 140; int v2 = 255;

    /*int h1 = 196; int h2 = 194;
    int s1 = 85; int s2 = 15;
    int v1 = 95; int v2 = 237;*/

    QColor c( 120, 60, 200 );
    for ( int i = 0; i < height; ++i )
    {
        int h = (h2-h1)*i/maxheight + h1;
        int s = (s2-s1)*i/maxheight + s1;
        int v = (v2-v1)*i/maxheight + v1;
        if ( i == 0 )
            drawBottomLine( p, x, y-i, width, QColor( h,s,v, QColor::Hsv ) );
        else if ( i == height-1 )
            drawTopLine( p, x, y-i, width, QColor( h,s,v, QColor::Hsv ) );
        else
            drawLine( p, x, y-i, width, QColor( h,s,v, QColor::Hsv ) );
    }
}


void MFrequencySpectrum::paintEvent( QPaintEvent* e )
{
    QPixmap pm( size() );
    QPainter p;
    p.begin( &pm );
    p.drawTiledPixmap( 0, 0, size().width(), size().height(), QPixmap( (const char**) &background ) );

    int xmargin = 5;
    int ymargin = 2;
    int y = size().height() - 2 * ymargin;
    int x = 0;
    int width = ( size().width() - 2 * xmargin ) / _channels;

    for ( int i = 0; i < _channels; ++i )
    {
        if ( _dirty[i] )
        {
            drawBar( &p, xmargin + x, y - ymargin, width-3, _values[i]*y/100, y );
            _dirty[i] = false;
        }
        x+= width;
    }

    p.end();
    bitBlt( this, 0, 0, &pm );
}


void MFrequencySpectrum::mousePressEvent( QMouseEvent* e )
{
    int xmargin = 5;
    int ymargin = 2;
    int y = size().height() - 2 * ymargin;
    int x = 0;
    int width = ( size().width() - 2 * xmargin ) / _channels;

    QPoint pos = e->pos();
    int channel = ( pos.x()-xmargin ) / width;
    int height = 100 - ( pos.y()-ymargin )*100/y;
    if ( channel < 15 ) _values[channel] = height;

}


Legende::Legende( int channels, QWidget* parent, const char* name, WFlags f )
        :QFrame( parent, name, f ), _channels( channels )
{
    setLineWidth( 2 );
    setFrameStyle( Panel + Sunken );
    setFixedHeight( 16 );

}


void Legende::drawContents( QPainter* p )
{
    int xmargin = 5;
    int ymargin = 2;
    int x = 0;
    int width = ( contentsRect().width() - 2 * xmargin ) / _channels;

    for ( int i = 0; i < _channels; ++i )
        p->drawText( xmargin + (width*i), 12, QString().sprintf( "%02d", i+1 ) );
}


MGraphWindow::MGraphWindow( QWidget* parent, const char* name, WFlags f )
             :QVBox( parent, name, f )
{
    spectrum = new MFrequencySpectrum( 14, this );
    legende = new Legende( 14, this );
    startTimer( 50 );
};


void MGraphWindow::testGraph()
{
    static int i = 0;
    spectrum->setValue( i++, 100 );
    if ( i == 14 ) i = 0;
    QTimer::singleShot( 2000, this, SLOT( testGraph() ) );

}


void MGraphWindow::timerEvent( QTimerEvent* e )
{
    for ( int i = 0; i < 14; i++ )
    {
        spectrum->decrease( i, 1 );
    }
    spectrum->repaint();
}


void MGraphWindow::traffic( int channel, int signal )
{
    spectrum->setValue( channel-1, signal );
}

