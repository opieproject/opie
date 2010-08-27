/*
                            This file is part of the Opie Project
                             Copyright (c)  2002 L. Potter <ljp@llornkcor.com>
														 and Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.

              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "oticker.h"

/* OPIE */
#include <opie2/odebug.h>

#include <qpe/config.h>

using namespace Opie::Ui;

OTicker::OTicker( QWidget* parent )
        : QLabel( parent )
{
    setTextFormat( Qt::RichText );
    Config cfg( "qpe" );
    cfg.setGroup( "Appearance" );
    backgroundcolor = QColor( cfg.readEntry( "Background", "#E5E1D5" ) );
    foregroundcolor = QColor( cfg.readEntry( "Text", "#FFFFFF" ) );
    updateTimerTime = 50;
    scrollLength = 1;
}

OTicker::~OTicker()
{}

void OTicker::setBackgroundColor( const QColor& backcolor )
{
    backgroundcolor = backcolor;
    update();
}

void OTicker::setForegroundColor( const QColor& backcolor )
{
    foregroundcolor = backcolor;
    update();
}

void OTicker::setFrame( int frameStyle )
{
    setFrameStyle( frameStyle /*WinPanel | Sunken */ );
    update();
}

void OTicker::setText( const QString& text )
{
    scrollText = text;

    int pixelLen = 0;
    bool bigger = false;
    int contWidth = contentsRect().width();
    int contHeight = contentsRect().height();
    int pixelTextLen = fontMetrics().width( text );
    odebug << "<<<<<<<height " << contHeight << ", width " << contWidth << ", text width " << pixelTextLen << " " << scrollText.length() << "\n" << oendl;
    if ( pixelTextLen < contWidth )
    {
        pixelLen = contWidth;
    }
    else
    {
        bigger = true;
        pixelLen = pixelTextLen;
    }
    QPixmap pm( pixelLen, contHeight );
    pm.fill( backgroundcolor );

    if(bigger)
        pos = contWidth; // reset it everytime the text is changed
    else
        pos = 0;
        
    QPainter pmp( &pm );
    pmp.setPen( foregroundcolor );
    pmp.drawText( 0, 0, pixelTextLen, contHeight, AlignVCenter, scrollText );
    pmp.end();
    scrollTextPixmap = pm;

    killTimers();

    m_scrollBreakWidth = (contWidth / 2); // some break space
    m_scrollTextWidth = scrollTextPixmap.width() + m_scrollBreakWidth; 
    //    odebug << "Scrollupdate " << updateTimerTime << "" << oendl;
    if ( bigger )
        startTimer( updateTimerTime );
    update();
}


void OTicker::timerEvent( QTimerEvent * )
{
    pos = ( pos <= -m_scrollTextWidth ) ? 0 : pos - scrollLength; //1;
    repaint( FALSE );
}

void OTicker::drawContents( QPainter *p )
{
    QRect contRect = contentsRect();
    p->drawPixmap( pos, contRect.y(), scrollTextPixmap );
    int actualWidth = m_scrollTextWidth - m_scrollBreakWidth;
    if ( pos < -(actualWidth - contRect.width()) ) {  // Scrolling
        // Scrolling, draw a break (blank) after text
        QPainter paint( this );
        paint.eraseRect( pos + actualWidth, contRect.y(), m_scrollBreakWidth, contRect.height() );
    }
    if ( pos < -(m_scrollTextWidth - contRect.width()) ) {
        // Scrolling, draw another copy after break width
        p->drawPixmap( pos + m_scrollTextWidth, contRect.y(), scrollTextPixmap );
    }
}

void OTicker::mouseReleaseEvent( QMouseEvent * )
{
    //    odebug << "<<<<<<<>>>>>>>>>" << oendl;
    emit mousePressed();
}

void OTicker::setUpdateTime( int time )
{
    updateTimerTime = time;
}

void OTicker::setScrollLength( int len )
{
    scrollLength = len;
}

void OTicker::resizeEvent( QResizeEvent *e )
{
    setText( scrollText );
    QLabel::resizeEvent( e );
}
