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

#include "sidething.h"

#include <qpe/resource.h>

#include <qrect.h>
#include <qpopupmenu.h>
#include <qpainter.h>


void PopupWithLaunchSideThing::setFrameRect( const QRect &r )
{
    fr = r;
    QPopupMenu::setFrameRect( r );
}


void PopupWithLaunchSideThing::paintEvent( QPaintEvent *event )
{
    QPainter paint( this );

    if ( !contentsRect().contains( event->rect() ) ) {

        QPopupMenu::setFrameRect( fr );
        int oldLW = lineWidth();
        setUpdatesEnabled(FALSE);
        setLineWidth(oldLW);
        setUpdatesEnabled(TRUE);

	paint.save();
	paint.setClipRegion( event->region().intersect( frameRect() ) );
	QPixmap pm( Resource::loadPixmap( sidePixmap ) );
	paint.drawPixmap( 2, fr.height() - pm.height() - 2, pm );
//	Need to draw a filled rectangle that extends the colour from the
//	end of the pixmap up to the top of the popupmenu
//	paint.fillRect();
	drawFrame( &paint );
	paint.restore();
	
    }
    if ( event->rect().intersects( contentsRect() ) /* &&
	 (fstyle & MShape) != HLine && (fstyle & MShape) != VLine */ ) {
        
	QPopupMenu::setFrameRect( QRect(fr.left() + 21, fr.top(), fr.width() - 21, fr.height()) );
        int oldLW = lineWidth();
        setUpdatesEnabled(FALSE);
        setLineWidth(oldLW);
        setUpdatesEnabled(TRUE);
	
	paint.setClipRegion( event->region().intersect( contentsRect() ) );
	drawContents( &paint );
    }
    
}


