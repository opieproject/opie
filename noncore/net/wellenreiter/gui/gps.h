/**********************************************************************
** Copyright (C) 2003 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Wellenreiter II.
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

#ifndef GPS_H
#define GPS_H

#include <qobject.h>
#include <qsocket.h>

class GpsLocation
{
  public:
    GpsLocation( const float& lat, const float& lon ) : _latitude(lat), _longitude(lon) {};
    ~GpsLocation() {};
    float latitude() const { return _latitude; };
    float longitude() const { return _longitude; };
    QString dmsPosition() const;
  private:
    float _latitude;
    float _longitude;
};


class GPS : public QObject
{
  Q_OBJECT

  public:
    GPS( QObject* parent = 0, const char * name = "GPS" );
    ~GPS();

    bool open( const QString& host = "localhost", int port = 2947 );
    GpsLocation position() const;

  private:
    QSocket* _socket;
};

#endif // GPS_H
