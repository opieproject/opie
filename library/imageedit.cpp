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
#include "imageedit.h"
#include <qpainter.h>

ImageEdit::ImageEdit( QWidget *parent, const char *name)
    : QScrollView( parent, name, WNorthWestGravity | WResizeNoErase ), buffer()
{
    buffer.resize( size() );
    buffer.fill( colorGroup().color( QColorGroup::Base ) );
}

ImageEdit::~ImageEdit()
{
    
}

void ImageEdit::contentsMousePressEvent( QMouseEvent *e )
{
    lastPos = e->pos();
}

void ImageEdit::contentsMouseMoveEvent( QMouseEvent *e )
{
    QPainter pw( viewport() );
    QPainter pb( &buffer );
    pb.drawLine( lastPos, e->pos() );
    pw.drawLine( contentsToViewport( lastPos ),
		 contentsToViewport( e->pos() ) );
    lastPos = e->pos();
}

void ImageEdit::contentsMouseReleaseEvent( QMouseEvent * )
{
}

void ImageEdit::viewportResizeEvent( QResizeEvent *e )
{
    enlargeBuffer(e->size());
}

void ImageEdit::enlargeBuffer( const QSize& sz )
{
    QSize osz = buffer.size();
    QSize nsz( QMAX( osz.width(), sz.width() ), QMAX( osz.height(), sz.height() ) );
    buffer.resize( nsz.width(), nsz.height() );
    // clear new area
    QPainter p( &buffer );
    if ( sz.width() > osz.width() )
	p.fillRect( osz.width(), 0, sz.width() - osz.width(), nsz.height(), colorGroup().color( QColorGroup::Base ) );
    if ( sz.height() > osz.height() )
	p.fillRect( 0, osz.height(), nsz.width(), sz.height() - osz.height(), colorGroup().color( QColorGroup::Base ) );
    p.end();
}

void ImageEdit::drawContents( QPainter *p, int cx, int cy, int cw, int ch )
{
    p->drawPixmap( cx, cy, buffer, cx, cy, cw, ch );
}

void ImageEdit::setPixmap( const QPixmap &pm )
{
    QSize osz = buffer.size();
    if ( pm.width() < osz.width() || pm.height() < osz.height() ) {
	buffer.fill(white);
	enlargeBuffer( pm.size() );
	QPainter p(&buffer);
	p.drawPixmap(0,0,pm);
    } else {
	buffer = pm;
    }
    resizeContents( buffer.width(), buffer.height() );
    viewport()->repaint( FALSE );
}

QPixmap ImageEdit::pixmap() const
{
    return buffer;
}

