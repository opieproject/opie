/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "omanufacturerdb.h"

/* QT */
#include <qstring.h>
#include <qfile.h>
#include <qtextstream.h>

OManufacturerDB* OManufacturerDB::_instance = 0;

OManufacturerDB* OManufacturerDB::instance()
{
    if ( !OManufacturerDB::_instance )
    {
        qDebug( "OManufacturerDB::instance(): creating OManufacturerDB..." );
       _instance = new OManufacturerDB();
    }
    return _instance;
}


OManufacturerDB::OManufacturerDB()
{
    QString filename( "/etc/manufacturers" );
    qDebug( "OManufacturerDB: trying to read '%s'...", (const char*) filename );
    if ( !QFile::exists( filename ) )
    {
        filename = "/opt/QtPalmtop/etc/manufacturers";
        qDebug( "OManufacturerDB: trying to read '%s'...", (const char*) filename );
        if ( !QFile::exists( filename ) )
        {
            filename = "/usr/share/wellenreiter/manufacturers";
            qDebug( "OManufacturerDB: trying to read '%s'...", (const char*) filename );
        }
    }

    QFile file( filename );
    bool hasFile = file.open( IO_ReadOnly );
    if (!hasFile)
    {
        qWarning( "OManufacturerDB: no valid manufacturer list found.", (const char*) filename );
    }
    else
    {
        qDebug( "OManufacturerDB: found manufacturer list in '%s'...", (const char*) filename );
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
                #ifdef DEBUG
                qDebug( "OManufacturerDB: read '%s' as extended manufacturer string", (const char*) extManu );
                #endif
                manufacturersExt.insert( addr, extManu );
            }
            else
                s.readLine();
            #ifdef DEBUG
            qDebug( "ManufacturerDB: read tuple %s, %s", (const char*) addr, (const char*) manu );
            #endif
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

