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
#ifndef PIXMAPLOADER_H
#define PIXMAPLOADER_H

#include <QImage>
#include <QBitmap>
#include <QIcon>
#include <QStringList>

class Resource
{
public:
    Resource() {}

    static QImage loadImage( const QString &name);

    static QPixmap loadPixmap( const QString &name );
    static QBitmap loadBitmap( const QString &name );
    static QString findPixmap( const QString &name );

    static QIcon loadIconSet( const QString &name );

    static QString findSound( const QString &name );
    static QStringList allSounds();
};

#ifdef OPIE_INTERNAL_LIBRARY_BUILD
extern bool qpe_fast_findPixmap;
#endif


// Inline for compatibility with SHARP ROMs
inline QIcon Resource::loadIconSet( const QString &pix )
{

#ifdef OPIE_INTERNAL_LIBRARY_BUILD
    /*
     * disable the slow load
     */
    bool oldMode = qpe_fast_findPixmap;
    qpe_fast_findPixmap = true;
#endif

    QPixmap dpm = loadPixmap( pix + "_disabled" );
    QPixmap pm = loadPixmap( pix );
    QIcon is( pm );
    if ( !dpm.isNull() )
	is.setPixmap( dpm, pm.width() <= 22 ? QIcon::Small : QIcon::Large, QIcon::Disabled );

#ifdef OPIE_INTERNAL_LIBRARY_BUILD
    qpe_fast_findPixmap = oldMode;
#endif

    return is;
}


#endif
