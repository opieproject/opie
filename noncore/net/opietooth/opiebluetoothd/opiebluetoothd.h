/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2011 Paul Eggleton <bluelightning@bluelightning.org>
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

#ifndef OBLUETOOTHD_H
#define OBLUETOOTHD_H

#include <qobject.h>

// forward declarations
class QDBusMessage;
class QDBusProxy;
class QDBusData;

#define BT_STATUS_ADAPTER       1
#define BT_STATUS_ENABLED       2
#define BT_STATUS_ENABLED_TEMP  4
#define BT_STATUS_DISCOVERABLE  8
#define BT_STATUS_RECEIVE       16

class OBluetoothDaemon: public QObject {
        Q_OBJECT
    public:
        OBluetoothDaemon();
        ~OBluetoothDaemon();
    protected:
        void startBluetooth();
        void stopBluetooth();
        void sendStatus(bool change);
        QString dataToString(const QDBusData &data);
    protected slots:
        void slotAdapterChange();
        void slotDBusConnect();
        void slotMessage(const QCString&, const QByteArray&);
        void slotDBusSignal(const QDBusMessage& message);
        void slotAsyncReply(int callID, const QDBusMessage& reply);
    protected:
        QString m_adapterPath;
        bool m_tempEnabled;
        bool m_powered;
        bool m_discoverEnabled;
        bool m_receiveEnabled;
        int m_propCallID;
        QDBusProxy *m_bluezManagerProxy;
        QDBusProxy *m_bluezAdapterProxy;
};


#endif
