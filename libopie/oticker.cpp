/*
                            This file is part of the Opie Project
                             Copyright (c)  2002 L. Potter <ljp@llornkcor.com>
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
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/config.h>

#include <qwidget.h>
#include <qpixmap.h>
#include <qbutton.h>
#include <qpainter.h>
#include <qframe.h>
#include <qlayout.h>
#include <qdir.h>
#include <stdlib.h>
#include <stdio.h>

#include "oticker.h"

OTicker::OTicker( QWidget* parent )
        : QFrame( parent ) {

//    setFrameStyle( NoFrame/*WinPanel | Sunken */);

    Config cfg("qpe");
    cfg.setGroup("Appearance");
    backgroundcolor = QColor( cfg.readEntry( "Background", "#E5E1D5" ) );
    foregroundcolor= Qt::black;
}

OTicker::~OTicker() {
}

void OTicker::setBackgroundColor(QColor backcolor) {
    backgroundcolor = backcolor;
}

void OTicker::setForegroundColor(QColor backcolor) {
    foregroundcolor = backcolor;
}

void OTicker::setFrame(int frameStyle) {
    setFrameStyle( frameStyle/*WinPanel | Sunken */);
}

void OTicker::setText( const QString& text ) {
    pos = 0; // reset it everytime the text is changed
    scrollText = text;

    int pixelLen = fontMetrics().width( text );
    QPixmap pm( pixelLen, contentsRect().height() );
//    pm.fill( QColor( 167, 212, 167 ));
 
    pm.fill(backgroundcolor);
    QPainter pmp( &pm );
    pmp.setPen(foregroundcolor );
    pmp.drawText( 0, 0, pixelLen, contentsRect().height(), AlignVCenter, scrollText );
    pmp.end();
    scrollTextPixmap = pm;

    killTimers();
    if ( pixelLen > contentsRect().width() )
        startTimer( 50 );
    update();
}


void OTicker::timerEvent( QTimerEvent * ) {
    pos = ( pos <= 0 ) ? scrollTextPixmap.width() : pos - 1;
    repaint( FALSE );
}

void OTicker::drawContents( QPainter *p ) {
    int pixelLen = scrollTextPixmap.width(); 
    p->drawPixmap( pos, contentsRect().y(), scrollTextPixmap );
    if ( pixelLen > contentsRect().width() ) // Scrolling
        p->drawPixmap( pos - pixelLen, contentsRect().y(), scrollTextPixmap );
}

void OTicker::mouseReleaseEvent( QMouseEvent * ) {
//    qDebug("<<<<<<<>>>>>>>>>");
    emit mousePressed();
}
