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

#include "info.h"
#include "desktop.h"

#include <qpe/resource.h>
#include <qpe/version.h>

#include <qlayout.h>
#include <qimage.h>
#include <qpainter.h>
#include <qsimplerichtext.h>


Info *desktopInfo = NULL;


Info::Info( Desktop *d ) : Background( d ), needsClear(FALSE), menuHasBeenClicked(FALSE)
{
    QString motd;
    /* eg.
	=
	"<table width=100% cellspacing=0 cellpadding=2>"
	"<tr><td bgcolor=#9090ff><h2>Today&nbsp;&nbsp;&nbsp;&nbsp;<small>June 15, 2001</small></h2></td>"
	"<tr><td bgcolor=#c0c0ff><big><a href=datebook>Appointments</a></big>"
	"<tr><td bgcolor=#e0e0ff>"
	    "<b>8:30am</b> Meeting with John<br>"
	    "<b>1:10pm</b> Lunch with Sharon"
	"<tr><td bgcolor=#c0c0ff><big><a href=todo>Reminders</a></big>"
	"<tr><td bgcolor=#e0e0ff>"
	    "<b>#1</b> Port XMAME to QPE<br>"
	    "<b>#2</b> Flowers for wife"
	"<tr><td bgcolor=#c0c0ff><big><a href=channels>Net channels</a></big>"
	"<tr><td bgcolor=#e0e0ff>"
	    "<b>LinuxDevices:</b><a href=http://www.linuxdevices.com> QPE announcement</a><br>"
	    "<b>Slashdot:</b><a href=http://www.slashdot.org> GPL Examined</a>"
	"</table>";
    */
    info = new QSimpleRichText(motd, QFont("lucidux_sans",10));
    desktopInfo = this;
}


void Info::mouseReleaseEvent( QMouseEvent * )
{
}


void Info::menuClicked( )
{
    QPainter p(this);
    if ( needsClear ) {
	QColor col = colorGroup().color( QColorGroup::Button ).dark( 0 );
	p.fillRect( 5, height() - 24, width() - 5, 20, col );
	needsClear = FALSE;
	menuHasBeenClicked = TRUE;
    }
}


void Info::paintEvent( QPaintEvent *e )
{
    QPainter p(this);

    BrushStyle styles[] = { Dense1Pattern, Dense2Pattern, Dense3Pattern,
			    Dense4Pattern, Dense5Pattern, Dense6Pattern };

    QColor shade = colorGroup().color( QColorGroup::Button ).dark( 110 ); 
    int blend = width() * 3 / 4;
    int step = blend/6;
    p.fillRect( 0, 0, width()-blend, 30, shade );
    for ( int i = 0; i < 6; i++ ) {
	QBrush brush( shade, styles[i] );
	p.fillRect( width()-blend+i*step, 0, step, 30, brush );
    }
    p.setFont( QFont("Helvetica", 24, QFont::Bold) );
    p.setPen( shade.dark( 140 ) );
    p.drawText( 5, 24, "QPE" );
    int pos = 5 + p.fontMetrics().width( "QPE" );
    QFont f("Helvetica", 10, QFont::Bold);
    p.setFont( f );
    p.drawText( pos + 5, 24, QString( "Version " ) + QPE_VERSION );

    if (!menuHasBeenClicked) {
	p.drawText( 5, height()-10, QString( "Click on the " ) );
	int pos = 5 + p.fontMetrics().width( "Click on the " );
	p.drawPixmap( pos, height()-10-14, Resource::loadPixmap( "go" ) );
	p.drawText( pos + 16, height()-10, QString( " logo to start." ) );
	needsClear = TRUE;
    }

    if ( info ) {
	info->setWidth(&p,width()-10);
	info->draw(&p, 5, 35, e->region(), colorGroup());
    }
}


