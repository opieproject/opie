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


float GPS::latitude() const
{
    char buf[256];

    int result = _socket->writeBlock( "p\r\n", 3 );
    if ( result )
    {
        qDebug( "GPS write succeeded" );
        _socket->waitForMore( 20 );
        if ( _socket->canReadLine() )
        {

            int num = _socket->readLine( &buf[0], sizeof buf );
            if ( num )
            {
                qDebug( "GPS got line: %s", &buf );
                return 0.0;
            }
        }
    }
    return -1.0;
}


float GPS::longitute() const
{
}

