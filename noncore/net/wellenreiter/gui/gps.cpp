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

/* QT */
#include <qtextstream.h>

/* STD */
#include <stdlib.h>
#include <unistd.h>

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
    double lat = -111.0;
    double lon = -111.0;

    int result = _socket->writeBlock( "p\r\n", 3 );
    _socket->flush();
    if ( result )
    {
        int numAvail = _socket->bytesAvailable();
        qDebug( "GPS write succeeded, %d bytes available for reading...", numAvail );
        if ( numAvail )
        {                     
            int numRead = _socket->readBlock( &buf[0], sizeof buf );
            int numScan = sscanf( &buf[0], "GPSD,P=%lg %lg", &lat, &lon);
            
            if ( numRead < 7 || numScan != 2 )
            {
                qDebug( "GPS read %d bytes succeeded, invalid response: '%s'", numRead, &buf[0] );
                return GpsLocation( -111, -111 );
            }
            else
            {
                return GpsLocation( lat, lon );
            }
        }
    }
    return GpsLocation( -111, -111 );
}


QString GpsLocation::dmsPosition() const
{
    if ( _latitude == -111 || _longitude == -111 )
        return "N/A";
    if ( _latitude == 0.0 && _longitude == 0.0 )
        return "NULL";

    /* compute latitude */

    QString dms = "N";
    if ( _latitude >= 0 ) dms.append( "+" );

    int trunc = int( _latitude );
    float rest = _latitude - trunc;

    float minf = rest * 60;
    int minutes = int( minf );

    rest = minf - minutes;
    int seconds = int( rest * 60 );

    dms.append( QString::number( trunc ) );
    dms.append( "° " );
    dms.append( QString::number( ::abs( minutes ) ) );
    dms.append( "' " );
    dms.append( QString::number( ::abs( seconds ) ) );
    dms.append( "'' " );

    /* compute longitude */

    dms.append( " | W" );
    if ( _longitude > 0 ) dms.append( "+" );

    trunc = int( _longitude );
    rest = _longitude - trunc;

    minf = rest * 60;
    minutes = int( minf );

    rest = minf - minutes;
    seconds = int( rest * 60 );

    dms.append( QString::number( trunc ) );
    dms.append( "° " );
    dms.append( QString::number( ::abs( minutes ) ) );
    dms.append( "' " );
    dms.append( QString::number( ::abs( seconds ) ) );
    dms.append( "'' " );

    return dms;
}
