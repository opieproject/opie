/*
                             This file is part of the Opie Project
                             Copyright (C) 2011 Paul Eggleton <bluelightning@bluelightning.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include <qpainter.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qapplication.h>
#include "onotifypopup.h"

using namespace Opie::Ui;

#define ROUNDING_FACTOR  20
#define BUFFER_ZONE 4

/**
 * Constructs the notify popup
 *
 * @param parent The parent of this popup.
 * @param name A name of this popup @see QObject
 */
ONotifyPopup::ONotifyPopup( QWidget *parent, const char *name )
    : QWidget( parent, name, ( WStyle_Customize | WStyle_Tool | WStyle_NoBorderEx | WStyle_StaysOnTop ) ),
        m_margin(8,8)
{
    m_ptrheight = 10;

    QColor c;
    c.setNamedColor("#FFEC8B");
    setBackgroundColor( c );
}

ONotifyPopup::~ONotifyPopup()
{
}

/**
 * @internal
 */
void ONotifyPopup::drawBubble( QPainter *painter, bool outline )
{
    // Draw main part of bubble
    QRect box( 0, 0, width(), height() - m_ptrheight );
    painter->drawRoundRect(box, (ROUNDING_FACTOR/(double)box.width())*100, (ROUNDING_FACTOR/(double)box.height())*100 );
    // Draw speech marker
    int ptrtarg = mapFromGlobal(m_pointto).x();
    if( ptrtarg < 0 )
        ptrtarg = 0;
    else if( ptrtarg > width() )
        ptrtarg = width();
    int ptrwidth = m_ptrheight;
    if( outline ) {
        // Cut out for pointer wide end
        painter->eraseRect( m_ptrcentre-(ptrwidth/2),box.bottom(), ptrwidth,1 );
        // Draw pointer
        painter->drawLine( m_ptrcentre-(ptrwidth/2),box.bottom(), ptrtarg,height() );
        painter->drawLine( ptrtarg,height(), m_ptrcentre+(ptrwidth/2),box.bottom() );
    }
    else {
        QPointArray a;
        a.setPoints( 3, m_ptrcentre-(ptrwidth/2),box.bottom(), ptrtarg,height(), m_ptrcentre+(ptrwidth/2),box.bottom() );
        painter->drawPolygon( a );
    }
}

/**
 * @internal
 */
void ONotifyPopup::resizeEvent( QResizeEvent * )
{
    // Generate a shaped mask
    QPixmap pixmap(size());
    QPainter painter(&pixmap);
    painter.fillRect(pixmap.rect(), Qt::white);
    painter.setBrush(Qt::black);
    drawBubble( &painter, false );
    setMask(pixmap.createHeuristicMask());
}

/**
 * @internal
 */
void ONotifyPopup::paintEvent( QPaintEvent * )
{
    QPainter painter( this );
    QRect box( 0, 0, width(), height() - m_ptrheight);
    // Draw bubble
    painter.setBrush( NoBrush );
    painter.setPen( Qt::black );
    drawBubble( &painter, true );
    // Draw text
    painter.drawText( m_margin.width(), m_margin.height(), width()-(m_margin.width()*2), height()-(m_margin.height() * 2), Qt::AlignLeft | Qt::AlignTop | Qt::WordBreak, m_text );
}

/**
 * @internal
 */
void ONotifyPopup::mouseReleaseEvent( QMouseEvent *e )
{
    if( e->button() & LeftButton )
        emit clicked();
}

/**
 * Returns the text caption of the popup
 * @returns the text to be displayed
 */
QString ONotifyPopup::text() const
{
    return m_text;
}

/**
 * Sets the text caption of the popup
 * @param str the text to be displayed
 */
void ONotifyPopup::setText( const QString &str )
{
    m_text = str;
    QFontMetrics fm(font());
    QSize maxSize;
    if( parentWidget() )
        maxSize = parentWidget()->size();
    else
        maxSize = qApp->desktop()->size();
    maxSize.setWidth( maxSize.width() - ((m_margin.width() * 2) + BUFFER_ZONE) );
    maxSize.setHeight( maxSize.height() - (m_margin.height() * 2) );
    QRect r = fm.boundingRect( 0, 0, maxSize.width(), maxSize.height(), Qt::AlignLeft | Qt::AlignTop | Qt::WordBreak, m_text);
    resize(r.width() + (m_margin.width() * 2), r.height() + (m_margin.height() * 2) + m_ptrheight );
}

/**
 * Sets the screen/parent position to point to with the popup
 * @param x the x position to point to
 * @param y the y position to point to
 */
void ONotifyPopup::setPointTo( int x, int y )
{
    m_pointto = QPoint( x, y );
    int parentwidth;
    if( parentWidget() )
        parentwidth = parentWidget()->size().width();
    else
        parentwidth = qApp->desktop()->size().width();
    double b = (double)m_pointto.x() / parentwidth;
    m_ptrcentre = width() * b;
    int margin = ROUNDING_FACTOR / 2 + m_ptrheight / 2;
    if( m_ptrcentre < margin )
        m_ptrcentre = margin;
    else if ( m_ptrcentre > width() - margin )
        m_ptrcentre = width() - margin;
    int newx = m_pointto.x() - m_ptrcentre;
    if( newx < BUFFER_ZONE )
        newx = BUFFER_ZONE;
    if( newx + width() + BUFFER_ZONE > parentwidth )
        newx = parentwidth - (width() + BUFFER_ZONE);
    int newy = m_pointto.y() - height();
    move( newx, newy );
}
