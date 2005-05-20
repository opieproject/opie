/*
                             This file is part of the Opie Project

                             Copyright (C) 2005 Dan Williams <drw@handhelds.org>
                             Copyright (C) 2003 Michael Lauer <mickey@tm.informatik.uni-frankfurt.de>
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

#ifndef ORESOURCE_H
#define ORESOURCE_H

#include <qimage.h>
#include <qpixmap.h>

namespace Opie {
namespace Core {

class OResource
{
    public:
        /**
         * Image scaling options.
         *
         * NoScale   - no scaling of image will be performed
         * SmallIcon - image will be scaled based using SmallIconSize value
         *           for width and height
         * BigIcon   - image will be scaled based on BigIconSize value for
         *             width and height
         *
         * Note: SmallIconSize and BigIconSize are run-time configuration
         *       options defined in qpe.conf (Appearance section)
         */
        enum Scale { NoScale = 0, SmallIcon, BigIcon };

        /**
         * Constructor
         */
        OResource() {}

        /**
         * @fn loadImage( const QString &name, Scale scale = NoScale )
         * @brief Load specified image.
         *
         * @param name - name of pixmap image to load
         * @param scale - scaling (if any) to preform on image
         *
         * @return QImage containing image loaded (and scaled if appropriate)
         **/
        static QImage loadImage( const QString &name, Scale scale = NoScale );

        /**
         * @fn loadPixmap( const QString &name, Scale scale = NoScale )
         * @brief Load specified image.
         *
         * @param name - name of pixmap image to load
         * @param scale - scaling (if any) to preform on image
         *
         * @return QPixmap containing image loaded (and scaled if appropriate)
         **/
        static QPixmap loadPixmap( const QString &name, Scale scale = NoScale );

        /**
         * @fn findPixmap( const QString &name )
         * @brief Retrieve fully qualified filename of image.
         *
         * @param name - name of pixmap image to retrieve filename of
         *
         * @return QString containing fully qualified filename of image
         *         (Null string if image is not found)
         **/
        static QString findPixmap( const QString &name );
};

} // namespace Core
} // namespace Opie

#endif
