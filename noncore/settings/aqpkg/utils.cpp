/*
                             This file is part of the OPIE Project

               =.            Copyright (c)  2002 Andy Qua <andy.qua@blueyonder.co.uk>
             .=l.                                Dan Williams <drw@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include <sys/vfs.h>

#include "utils.h"
#include "global.h"

Utils :: Utils()
{
}

Utils :: ~Utils()
{
}

QString Utils :: getPathfromIpkFilename( const QString &file )
{
    int p = file.findRev( "/" );
    QString path = "";
    if ( p != -1 )
    	path = file.left( p );

    return path;

}

QString Utils :: getFilenameFromIpkFilename( const QString &file )
{
    int p = file.findRev( "/" );
    QString name = file;
    if ( p != -1 )
    	name = name.mid( p + 1 );


    return name;
}

QString Utils :: getPackageNameFromIpkFilename( const QString &file )
{
    int p = file.findRev( "/" );
    QString name = file;
    if ( p != -1 )
        name = name.mid( p + 1 );
    p = name.find( "_" );
    QString packageName = name.mid( 0, p );
    return packageName;
}

QString Utils :: getPackageVersionFromIpkFilename( const QString &file )
{
    int p = file.findRev( "/" );
    QString name = file;
    if ( p != -1 )
    	name = name.mid( p + 1 );
    p = name.find( "_" ) + 1;
    int p2 = name.find( "_", p );
    QString version = name.mid( p, p2 - p );
    return version;
}


bool Utils :: getStorageSpace( const char *path, long *blockSize, long *totalBlocks, long *availBlocks )
{
    bool ret = false;

    struct statfs fs;
    if ( !statfs( path, &fs ) )
    {
        *blockSize = fs.f_bsize;
        *totalBlocks = fs.f_blocks;
        *availBlocks = fs.f_bavail;
        ret = true;
    }

    return ret;
}

