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

#ifndef BLUETOOTHAGENT_H
#define BLUETOOTHAGENT_H

#include <qobject.h>
#include <qmap.h>
#include <dbus/qdbusconnection.h>
#include <dbus/qdbusobject.h>

// forward declarations
class QDBusMessage;
class QDBusProxy;
class PinDlg;

class OBluetoothAgent: public QObject, public QDBusObjectBase {
        Q_OBJECT
    public:
        OBluetoothAgent( const QString &adapterPath );
        ~OBluetoothAgent();
        void pairDevice(const QString &bdaddr);
    protected:
        virtual bool handleMethodCall(const QDBusMessage& message);
        void destroyDialog();
    protected slots:
        void pinDialogClosed(bool);
        void slotAsyncReply( int callId, const QDBusMessage& reply );
    private:
        QDBusConnection m_connection;
        QDBusProxy *m_bluezAdapterProxy;
        PinDlg *m_pinDlg;
        QDBusMessage *m_authMsg;
        QMap<int,QString> m_calls;
};


#endif
