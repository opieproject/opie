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

#include "fontmanager.h"
#include <qfile.h>
#include <stdlib.h>
#include <qgfx_qws.h>



/*
    QFontInfo doesn't work in QWS at the moment,
    otherwise we would just have used that to check
    the real values
    
     For now, there are only two Unicode fonts in
     the known universe...
    
*/

bool FontManager::hasUnicodeFont()
{
    QString fontDir = getenv("QTDIR") + QString("/lib/fonts/");

    QString suffix;
    if ( qt_screen->isTransformed() ) {
	suffix += "_t";
	QPoint a = qt_screen->mapToDevice(QPoint(0,0),QSize(2,2));
	QPoint b = qt_screen->mapToDevice(QPoint(1,1),QSize(2,2));
	suffix += QString::number( a.x()*8+a.y()*4+(1-b.x())*2+(1-b.y()) );
    }
    suffix += ".qpf";

    return QFile::exists( fontDir+"cyberbit_120_50"+suffix )
	|| QFile::exists( fontDir+"unifont_160_50"+suffix ) ||
	QFile::exists( fontDir+"arial_140_50" + suffix );
}

QFont FontManager::unicodeFont( Spacing sp )
{
    QString key;
    QString fontName;
    QString fontDir = getenv("QTDIR") + QString("/lib/fonts/");

    int size;
    if ( sp == Proportional ) {
	fontName = "Arial";
	size=14;
	key = "arial_140_50";
    } else {
	fontName = "Unifont";
	size=16;
	key = "unifont_160_50";
    }
	
    QString suffix;
    if ( qt_screen->isTransformed() ) {
	suffix += "_t";
	QPoint a = qt_screen->mapToDevice(QPoint(0,0),QSize(2,2));
	QPoint b = qt_screen->mapToDevice(QPoint(1,1),QSize(2,2));
	suffix += QString::number( a.x()*8+a.y()*4+(1-b.x())*2+(1-b.y()) );
    }
    suffix += ".qpf";

    // if we cannot find it, try the other one
	
    if ( !QFile::exists(fontDir+key+suffix) ) {
	key = (sp == Fixed ) ? "arial_140_50" : "unifont_160_50";
	if ( QFile::exists(fontDir+key+suffix) ) {
	    fontName = (sp == Fixed) ? "Arial" : "Unifont";
	    size = (sp == Fixed) ? 14 : 16;
	} else {
	    key = "cyberbit_120_50";
	    if ( QFile::exists(fontDir+key+suffix) ) {
		fontName = "Cyberbit";
		size = 12;
	    } else {
		fontName = "Helvetica";
		size = 14;
	    }
	}
    }
    return QFont(fontName,size);
}
