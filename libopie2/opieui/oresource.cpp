/*
                             This file is part of the Opie Project

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

#include "oresource.h"


#ifdef QWS
namespace Resource
{

QPixmap loadPixmap( const QString& pix )
{
    QString filename;
    filename.sprintf( "%s/%s.png", (const char*) oApp->qpeDir(), (const char*) pix );
    QPixmap pixmap( filename );
    if ( pixmap.isNull() )
    {
        odebug << "libopie2 resource: can't find pixmap " << filename << oendl;;
    }
    return pixmap;
};

};

#endif



