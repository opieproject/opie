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
#include <qapplication.h>
#include <qstring.h>
#include <qfile.h>
#include <qtextstream.h>

#define OPIE_IMPROVE_GUI_LATENCY 1

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
            s >> manu;
            s >> extManu;

            manufacturers.insert( addr, manu );
            manufacturersExt.insert( addr, extManu );
            odebug << "OmanufacturerDB: parse '" << addr << "' as '" << manu << "' (" << extManu << ")" << oendl;
            #ifdef OPIE_IMPROVE_GUI_LATENCY
            if ( qApp ) qApp->processEvents();
            #endif
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

