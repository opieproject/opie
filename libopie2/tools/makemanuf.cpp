/*
                             This file is part of the Opie Project
                             (C) 2003 Michael 'Mickey' Lauer <mickey@Vanille.de>
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

#define private public
#include <opie2/omanufacturerdb.h>

#include <qdatastream.h>
#include <qfile.h>

int main( int argc, char** argv )
{
    if ( argc < 2 )
    {
        qDebug( "Usage: ./makemanuf <output>" );
        return -1;
    }

    OManufacturerDB* db = OManufacturerDB::instance();

    QFile f( argv[1] );
    if ( !f.open( IO_WriteOnly ) )
    {
        qDebug( "Can't open file %s", argv[1] );
        return -1;
    }

    QDataStream ds( &f );
    ds << db->manufacturers;
    ds << db->manufacturersExt;

    qDebug( "ManufacturerDB successfully written to %s", argv[1] );

    return 0;
}

