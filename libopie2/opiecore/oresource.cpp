/*
                             This file is part of the Opie Project

                             Copyright (C) 2005 Dan Williams <drw@handhelds.org>
                             Copyright (C) 2003 Patrick S. Vogt <tille@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
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

#include <opie2/oapplication.h>
#include <opie2/odebug.h>

#include <qpe/applnk.h>

#include "oresource.h"

namespace Opie {
namespace Core {

QImage OResource::loadImage( const QString &name, Scale scale )
{
    // Load image
    QString filename;
    filename.sprintf( "%spics/%s.png", (const char*) oApp->qpeDir(), (const char*) name );
    QImage image( filename );
    if ( image.isNull() )
        odebug << "libopie2 OResource: can't find image " << filename << oendl;

    // Scale image (if necessary)
    if ( scale == SmallIcon )
    {
        // Retrieve size of small icons
        if ( smallIconSize == -1 )
            smallIconSize = AppLnk::smallIconSize();

        // Scale image
        return image.smoothScale( smallIconSize, smallIconSize );
    }
    else if ( scale == BigIcon )
    {
        // Retrieve size of big icons
        if ( bigIconSize == -1 )
            bigIconSize = AppLnk::bigIconSize();

        // Scale image
        return image.smoothScale( bigIconSize, bigIconSize );
    }
    else
        return image;
}

QPixmap OResource::loadPixmap( const QString &name, Scale scale )
{
    QPixmap pixmap;
    pixmap.convertFromImage( loadImage( name, scale ) );
    return pixmap;
}

} // namespace Core
} // namespace Opie



