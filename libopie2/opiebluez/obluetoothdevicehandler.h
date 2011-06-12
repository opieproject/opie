/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2011 Paul Eggleton <bluelightning@bluelightning.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
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

#ifndef OBLUETOOTHDEVICE_HANDLER_H
#define OBLUETOOTHDEVICE_HANDLER_H

#include <qmap.h>
#include <qlist.h>
#include <qobject.h>

class QDBusProxy;
class QDBusMessage;
class QDBusData;
class QDBusError;

namespace Opie {
namespace Bluez {

class OBluetoothInterface;
class OBluetoothDevice;
class DeviceHandlerPool;

class DeviceHandler: public QObject {
    Q_OBJECT
public:
    DeviceHandler( OBluetoothDevice *dev, const QString &dbusInterface, DeviceHandlerPool *pool = NULL );
    ~DeviceHandler();
    OBluetoothDevice *device();
    QString dbusInterface();
protected slots:
    virtual void slotAsyncReply( int callId, const QDBusMessage& reply );
protected:
    void sendAsync( const QString& method, const QValueList<QDBusData>& params );
    OBluetoothDevice *m_dev;
    DeviceHandlerPool *m_pool;
    QDBusProxy *m_devProxy;
    QMap<int,QString> m_calls;
};

class DeviceHandlerPool: public QObject {
    Q_OBJECT
    friend class DeviceHandler;
public:
    DeviceHandlerPool( QObject *parent = NULL );
    DeviceHandler *getHandler( OBluetoothDevice *dev, const QString &dbusInterface );
signals:
    void success( DeviceHandler *handler, const QString &method, const QString &retval );
    void failure( DeviceHandler *handler, const QString &method, const QString &error, const QString &message );
protected:
    void addHandler( DeviceHandler *handler );
    void handlerSuccess( DeviceHandler *handler, const QString &method, const QString &retval );
    void handlerError( DeviceHandler *handler, const QString &method, const QString &error, const QString &message );
    QList<DeviceHandler> m_handlers;
};

class InputDeviceHandler: public DeviceHandler {
public:
    InputDeviceHandler( OBluetoothDevice *dev, DeviceHandlerPool *pool = NULL );
    void connect();
    void disconnect();
    static QString dbusInterface();
};

class SerialDeviceHandler: public DeviceHandler {
public:
    SerialDeviceHandler( OBluetoothDevice *dev, DeviceHandlerPool *pool = NULL );
    void connect( const QString &pattern );
    void disconnect();
    static QString dbusInterface();
};

class NetworkDeviceHandler: public DeviceHandler {
public:
    NetworkDeviceHandler( OBluetoothDevice *dev, DeviceHandlerPool *pool = NULL );
    void connect( const QString &pattern );
    void disconnect();
    static QString dbusInterface();
};

}
}

#endif
