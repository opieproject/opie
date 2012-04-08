/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2012 Paul Eggleton <bluelightning@bluelightning.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this file; see the file COPYING.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/


#ifndef SERVICE_H
#define SERVICE_H

#include <qobject.h>
#include <qmap.h>
#include <qdict.h>
#include <qstring.h>
#include <qvaluelist.h>

#include <dbus/qdbusdata.h>
#include <dbus/qdbusobject.h>
#include <dbus/qdbusobjectpath.h>
#include <dbus/qdbusdatamap.h>

class QDBusProxy;

class ServiceListener: public QObject {
    Q_OBJECT
public:
    ServiceListener( const QDBusObjectPath &path, const QDBusDataMap<QString> &props );
    ~ServiceListener();

    QDBusProxy *proxy();
    QString serviceName();
    QString serviceType();
    QString state();

signals:
    void serviceStateChanged( const QDBusObjectPath &path, const QString &oldstate, const QString &newstate );
    void signalStrength( int strength );

protected slots:
    void slotAsyncReply( int callId, const QDBusMessage& reply );
    void slotDBusSignal( const QDBusMessage& message );
    void sendSignalStrength();

private:
    QDBusProxy *m_proxy;
    QString m_serviceName;
    QString m_type;
    QString m_state;
    int m_strength;
};

#endif
