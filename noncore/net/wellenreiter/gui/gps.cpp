/**********************************************************************
** Copyright (C) 2003 Michael 'Mickey' Lauer.  All rights reserved.
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

#include "gps.h"
#include <unistd.h>

#include <qtextstream.h>

GPS::GPS( QObject* parent, const char * name )
    :QObject( parent, name )
{
    qDebug( "GPS::GPS()" );
    _socket = new QSocket( this, "gpsd commsock" );
}


GPS::~GPS()
{
    qDebug( "GPS::~GPS()" );
}


bool GPS::open( const QString& host, int port )
{
    _socket->connectToHost( host, port );
}


GpsLocation GPS::position() const
{
    char buf[256];

    int result = _socket->writeBlock( "p\r\n", 3 );
    _socket->flush();
    if ( result )
    {
        int numAvail = _socket->bytesAvailable();
        qDebug( "GPS write succeeded, %d bytes available for reading...", numAvail );
        if ( numAvail )
        {
            QTextStream stream( _socket );

            QString str;
            stream.readRawBytes( &buf[0], 7 );
            float lat = -111.111;
            stream >> lat;
            stream.skipWhiteSpace();
            float lon = -111.111;
            stream >> lon;
            stream.readRawBytes( &buf[0], 200 ); // read and discard the stuff until EOF

            return GpsLocation( lat, lon );
        }
    }
    return GpsLocation( -1.0, -1.0 );
}

