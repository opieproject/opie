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

#include "clipboard.h"

#include <qpe/resource.h>

#include <qpainter.h>
#include <qpopupmenu.h>
#include <qwindowsystem_qws.h>


//===========================================================================

ClipboardApplet::ClipboardApplet( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    setFixedWidth( 14 );
    clipboardPixmap = Resource::loadPixmap( "clipboard" );
    menu = 0;
}

ClipboardApplet::~ClipboardApplet()
{
}

void ClipboardApplet::mousePressEvent( QMouseEvent *)
{
    if ( !menu ) {
	menu = new QPopupMenu(this);
	menu->insertItem(tr("Cut"));
	menu->insertItem(tr("Copy"));
	menu->insertItem(tr("Paste"));
	connect(menu, SIGNAL(selected(int)), this, SLOT(action(int)));
    }
    menu->popup(mapToGlobal(QPoint(0,0)));
}

void ClipboardApplet::action(int i)
{
    ushort unicode=0;
    int scan=0;

    if ( i == 0 )
	{ unicode='X'-'@'; scan=Key_X; } // Cut
    else if ( i == 1 )
	{ unicode='C'-'@'; scan=Key_C; } // Copy
    else if ( i == 2 )
	{ unicode='V'-'@'; scan=Key_V; } // Paste

    if ( scan ) {
	qwsServer->processKeyEvent( unicode, scan, ControlButton, TRUE, FALSE );
	qwsServer->processKeyEvent( unicode, scan, ControlButton, FALSE, FALSE );
    }
}

void ClipboardApplet::paintEvent( QPaintEvent* )
{
    QPainter p(this);
    p.drawPixmap( 0, 1, clipboardPixmap );
}


