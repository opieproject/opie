/*
                             This file is part of the Opie Project
              =.             (C) 2003-2004 Michael 'Mickey' Lauer <mickey@Vanille.de>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
     ._= =}       :
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

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/qpeapplication.h>
#ifdef OPIE_IMPROVE_GUI_LATENCY
#include <qpe/global.h>
#endif



/* QT */
#include <qapplication.h>
#include <qfile.h>
#include <qtextstream.h>

using namespace Opie::Core;
namespace Opie {
namespace Net  {

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
    if ( qApp ) Global::statusMessage( "Reading Manufacturers..." );
    QString filename( "/etc/manufacturers" );
    odebug << "OManufacturerDB: trying to read " << filename << oendl;
    if ( !QFile::exists( filename ) )
    {
        filename = QPEApplication::qpeDir()+"etc/manufacturers";
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
        int counter = 0;

        while (!s.atEnd())
        {
            s >> addr;
            s >> manu;
            s >> extManu;

            manufacturers.insert( addr, manu );
            manufacturersExt.insert( addr, extManu );
            // odebug << "OmanufacturerDB: parse '" << addr << "' as '" << manu << "' (" << extManu << ")" << oendl;
            counter++;
            if ( counter == 50 )
            {
                if ( qApp ) qApp->processEvents();
                counter = 0;
            }
        }
        odebug << "OManufacturerDB: manufacturer list completed." << oendl;
        if ( qApp ) Global::statusMessage( "Manufacturers Complete..." );
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

}
}

