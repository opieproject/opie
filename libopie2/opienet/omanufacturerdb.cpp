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

#include "omanufacturerdb.h"

/* OPIE CORE */
#include <opie2/odebug.h>

/* QT */
#include <qstring.h>
#include <qfile.h>
#include <qtextstream.h>

OManufacturerDB* OManufacturerDB::_instance = 0;

OManufacturerDB* OManufacturerDB::instance()
{
    if ( !OManufacturerDB::_instance )
    {
        odebug << "OManufacturerDB::instance(): creating OManufacturerDB..." << oendl;
       _instance = new OManufacturerDB();
    }
    return _instance;
}


OManufacturerDB::OManufacturerDB()
{
    QString filename( "/etc/manufacturers" );
    odebug << "OManufacturerDB: trying to read " << filename << oendl;
    if ( !QFile::exists( filename ) )
    {
        filename = "/opt/QtPalmtop/etc/manufacturers";
        odebug << "OManufacturerDB: trying to read " << filename << oendl;
        if ( !QFile::exists( filename ) )
        {
            filename = "/usr/share/wellenreiter/manufacturers";
            odebug << "OManufacturerDB: trying to read " << filename << oendl;
        }
    }

    QFile file( filename );
    bool hasFile = file.open( IO_ReadOnly );
    if (!hasFile)
    {
        owarn << "OManufacturerDB: no valid manufacturer list found." << oendl;
    }
    else
    {
        odebug << "OManufacturerDB: found manufacturer list in " << filename << oendl;
        QTextStream s( &file );
        QString addr;
        QString manu;
        QString extManu;
        while (!s.atEnd())
        {
            s >> addr;
            if ( !addr ) // read nothing!?
            {
                continue;
            }
            else
            if ( addr[0] == '#' )
            {
                continue;
            }
            s.skipWhiteSpace();
            s >> manu;
            s.skipWhiteSpace();
            s >> extManu;
            if ( extManu[0] == '#' ) // we have an extended manufacturer
            {
                s.skipWhiteSpace();
                extManu = s.readLine();
                odebug << "OManufacturerDB: read " << extManu << " as extended manufacturer string" << oendl;
                manufacturersExt.insert( addr, extManu );
            }
            else
                s.readLine();
            odebug << "OManufacturerDB: read tuple " << addr << ", " << manu << oendl;
            manufacturers.insert( addr, manu );
        }
    }

}


OManufacturerDB::~OManufacturerDB()
{
}


const QString& OManufacturerDB::lookup( const QString& macaddr ) const
{
    return manufacturers[macaddr.upper().left(8)];
}


const QString& OManufacturerDB::lookupExt( const QString& macaddr ) const
{
    QMap<QString,QString>::ConstIterator it = manufacturersExt.find( macaddr.upper().left(8) );
    return it == manufacturersExt.end() ? lookup( macaddr ) : *it;
}

