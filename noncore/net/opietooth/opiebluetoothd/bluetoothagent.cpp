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

// local includes
#include "bluetoothagent.h"
#include "bluetoothpindlg.h"

// Qt includes
#include <qvaluelist.h>
#include <qstringlist.h>
#include <qtimer.h>
#include <qmessagebox.h>

// Qt DBUS includes
#include <dbus/qdbusdatalist.h>
#include <dbus/qdbusdatamap.h>
#include <dbus/qdbuserror.h>
#include <dbus/qdbusmessage.h>
#include <dbus/qdbusproxy.h>
#include <dbus/qdbusobjectpath.h>
#include <dbus/qdbusvariant.h>

// Qtopia includes
#include <qpe/qpeapplication.h>
#include <qpe/qcopenvelope_qws.h>

// Opie includes
#include <opie2/odebug.h>
#include <opie2/obluetooth.h>

using namespace Opie::Core;
using namespace Opie::Bluez;

// CreatePairedDevice insists on a different agent to the one used
// for RegisterAgent for none other than internal reasons AFAICT
#define AGENT_OBJECT_PATH        "/org/opie/bluetooth/agent"
#define AGENT_OBJECT_PATH_SEND   "/org/opie/bluetooth/agent_send"

// QMessageBox subclass that allows us to handle authorisation asynchronously
class AuthMessageBox: public QMessageBox {
public:
    AuthMessageBox( const QDBusMessage &authMsg, const QString &name )
        : QMessageBox( tr("Bluetooth"),
            tr("<p>Authorize connection to %1?</p>").arg(name),
            QMessageBox::Information,
            QMessageBox::Yes | QMessageBox::Default,
            QMessageBox::No, 0 ),
        m_authMsg( authMsg )
    {
    };

    void done( int ret ) {
        QMessageBox::done(ret);
        QDBusConnection connection = QDBusConnection::systemBus();
        if( ret == QMessageBox::Yes ) {
            QDBusMessage reply = QDBusMessage::methodReply(m_authMsg);
            connection.send(reply);
        }
        else {
            QDBusError error("org.bluez.Error.Rejected", tr("User rejected authorization"));
            QDBusMessage reply = QDBusMessage::methodError(m_authMsg, error);
            connection.send(reply);
        }
    };
private:
    QDBusMessage m_authMsg;
};


OBluetoothAgent::OBluetoothAgent( const QString &adapterPath )
{
    m_pinDlg = NULL;
    m_msgbox = NULL;

    m_connection = QDBusConnection::systemBus();
    m_bluezAdapterProxy = new QDBusProxy(this);
    m_bluezAdapterProxy->setService("org.bluez");
    m_bluezAdapterProxy->setPath(adapterPath);
    m_bluezAdapterProxy->setInterface("org.bluez.Adapter");
    m_bluezAdapterProxy->setConnection(m_connection);

    QObject::connect(m_bluezAdapterProxy, SIGNAL(asyncReply(int, const QDBusMessage&)),
                    this, SLOT(slotAsyncReply(int, const QDBusMessage&)));

    m_connection.registerObject(AGENT_OBJECT_PATH, this);
    m_connection.registerObject(AGENT_OBJECT_PATH_SEND, this);

    odebug << "Object registered for path " << AGENT_OBJECT_PATH << " on unique name " <<
           m_connection.uniqueName().local8Bit().data() << oendl;
    odebug << "Object registered for path " << AGENT_OBJECT_PATH_SEND << " on unique name " <<
           m_connection.uniqueName().local8Bit().data() << oendl;

    // Register agent
    QValueList<QDBusData> parameters;
    parameters << QDBusData::fromObjectPath(QDBusObjectPath(AGENT_OBJECT_PATH));
    parameters << QDBusData::fromString("DisplayYesNo");
    int callId = m_bluezAdapterProxy->sendWithAsyncReply("RegisterAgent", parameters);
    m_calls[callId] = "RegisterAgent";
}

OBluetoothAgent::~OBluetoothAgent()
{
    m_connection.unregisterObject(AGENT_OBJECT_PATH);
    delete m_bluezAdapterProxy;
    delete m_pinDlg;
}

bool OBluetoothAgent::handleMethodCall(const QDBusMessage& message)
{
    odebug << "OBluetoothAgent::handleMethodCall: interface='" << message.interface().latin1() << "', member='" << message.member().latin1() << "'" << oendl;

    if (message.interface() != "org.bluez.Agent")
        return false;

    if (message.member() == "RequestPinCode") {
        destroyDialog();
        m_pinDlg = new PinDlg(0, "pindlg", FALSE);
        m_pinMsg = new QDBusMessage(message);
        connect( m_pinDlg, SIGNAL(dialogClosed(bool)), this, SLOT(pinDialogClosed(bool)) );

        // Get the device's name and address
        OBluetoothInterface *intf = OBluetooth::instance()->defaultInterface();
        OBluetoothDevice *dev = intf->findDeviceByPath( message[0].toObjectPath() );
        m_pinDlg->setRemoteName( dev->name() );
        m_pinDlg->setBdAddr( dev->macAddress() );
        m_pinDlg->showMaximized();
        return true;
    }
    else if (message.member() == "Authorize") {
        odebug << "Authorize " << message[1].toObjectPath() << oendl;
        OBluetoothInterface *intf = OBluetooth::instance()->defaultInterface();
        OBluetoothDevice *dev = intf->findDeviceByPath( message[0].toObjectPath() );
        m_msgbox = new AuthMessageBox( message, dev->name() );
        m_msgbox->show();
        return true;
    }
    else if (message.member() == "Cancel") {
        destroyDialog();
        QMessageBox::message(tr("Bluetooth"),tr("Authentication was cancelled"));
        return true;
    }

    return false;
}

void OBluetoothAgent::slotAsyncReply( int callId, const QDBusMessage& reply )
{
    QString method = m_calls[callId];

    if( method == "CreatePairedDevice" ) {
        if (reply.type() == QDBusMessage::ReplyMessage) {
            odebug << "CreatePairedDevice succeeded" << oendl;
            QCopEnvelope e("QPE/BluetoothBack", "devicePaired()");
        }
        else {
            if( reply.error().name() == "org.bluez.Error.AlreadyExists" ) {
                odebug << "CreatePairedDevice - already paired" << oendl;
                QCopEnvelope e("QPE/BluetoothBack", "deviceAlreadyPaired()");
            }
            else {
                odebug << "CreatePairedDevice failed: " << reply.error().name() << ": " << reply.error().message() << oendl;
                QCopEnvelope e("QPE/BluetoothBack", "devicePairingFailed()");
            }
        }
    }
    else {
        if (reply.type() == QDBusMessage::ErrorMessage)
            odebug << method << " failed: " << reply.error().name() << ": " << reply.error().message() << oendl;
    }

    m_calls.remove(callId);
}

void OBluetoothAgent::pinDialogClosed( bool accepted )
{
    if( accepted ) {
        QDBusMessage reply = QDBusMessage::methodReply(*m_pinMsg);
        reply << QDBusData::fromString(m_pinDlg->pin());
        m_connection.send(reply);
    }
    else {
        QDBusError error("org.bluez.Error.Canceled", tr("User cancelled"));
        QDBusMessage reply = QDBusMessage::methodError(*m_pinMsg, error);
        m_connection.send(reply);
    }
    destroyDialog();
}

void OBluetoothAgent::destroyDialog()
{
    if( m_pinDlg ) {
        delete m_pinDlg;
        m_pinDlg = NULL;
        delete m_pinMsg;
        m_pinMsg = NULL;
    }
}

void OBluetoothAgent::pairDevice(const QString &bdaddr)
{
    odebug << "pairDevice " << bdaddr << oendl;
    QValueList<QDBusData> parameters;
    parameters << QDBusData::fromString(bdaddr);
    parameters << QDBusData::fromObjectPath(QDBusObjectPath(AGENT_OBJECT_PATH_SEND));
    parameters << QDBusData::fromString("DisplayYesNo");
    int callId = m_bluezAdapterProxy->sendWithAsyncReply("CreatePairedDevice", parameters);
    m_calls[callId] = "CreatePairedDevice";
}
