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

#include "background.h"
#include "desktop.h"

#include <qpe/resource.h>

#include <qpainter.h>


Background::Background( Desktop *d ) :
    QWidget( d, 0, WStyle_Tool | WStyle_Customize )
{
/*
    if ( QPixmap::defaultDepth() < 12 ) {
	setBackgroundColor(QColor(0x20, 0xb0, 0x50));
    } else {
	setBackgroundPixmap( Resource::loadPixmap( "bg" ) );
    }
*/
    setBackgroundMode( PaletteButton );
}

