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

#include "manufacturers.h"

// Qt
#include <qstring.h>
#include <qfile.h>
#include <qtextstream.h>

ManufacturerDB::ManufacturerDB( const QString& filename )
{
    QFile file( filename );
    bool hasFile = file.open( IO_ReadOnly );
    if (!hasFile)
    {
        qDebug( "ManufacturerDB: D'oh! Manufacturer list '%s' not found!", (const char*) filename );
    }
    else
    {
        qDebug( "ManufacturerDB: reading manufacturer list from '%s'...", (const char*) filename );
        QTextStream s( &file );
        QString addr;
        QString manu;
        
        while (!s.atEnd())
        {
            s >> addr;
            s.skipWhiteSpace(); 
            manu = s.readLine();
            qDebug( "ManufacturerDB: read pair %s, %s", (const char*) addr, (const char*) manu );
            manufacturers.insert( addr, manu );
            
        }
    }

}

ManufacturerDB::~ManufacturerDB()
{
}

QString ManufacturerDB::lookup( const QString& macaddr )
{
    return manufacturers[macaddr.upper().left(8)];
}
