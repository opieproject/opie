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

#ifndef GPS_H
#define GPS_H

#include <qobject.h>
#include <qsocket.h>

class GPS : public QObject
{
  Q_OBJECT

  public:
    GPS( QObject* parent = 0, const char * name = "GPS" );
    ~GPS();

    bool open( const QString& host = "localhost", int port = 2947 );
    float latitude() const;
    float longitute() const;

  private:
    QSocket* _socket;
};

#endif // GPS_H
