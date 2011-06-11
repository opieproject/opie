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

#include "opiebluetoothd.h"
#include "bluetoothagent.h"
#include "hciattach.h"
#include "rfkill.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/obluetooth.h>

/* QT */
#include <qapplication.h>
#include <qmap.h>
#include <qfile.h>
#include <qpe/qcopenvelope_qws.h>

/* STD */
#include <stdlib.h>

using namespace Opie::Core;

OBluetoothDaemon::OBluetoothDaemon()
{
    QCopChannel* chan = new QCopChannel("QPE/Bluetooth", this );
    connect(chan, SIGNAL(received(const QCString&,const QByteArray&) ),
            this, SLOT(slotMessage(const QCString&,const QByteArray&) ) );

    m_agent = NULL;
    m_tempEnabled = false;
    m_receiveEnabled = false;

    // Set up hciattach handling if required
    m_hciattach = new OHciAttach();
    if( ! m_hciattach->isConfigured() ) {
        delete m_hciattach;
        m_hciattach = NULL;
    }

    m_rfkill = new ORfKill();

    m_bluetooth = OBluetooth::instance();
    connect( m_bluetooth, SIGNAL( defaultInterfaceChanged( OBluetoothInterface* ) ),
                this, SLOT( defaultInterfaceChanged( OBluetoothInterface* ) ) ) ;
    m_btinterface = m_bluetooth->defaultInterface();
    slotAdapterChange();
    sendStatus(true);

    odebug << "opiebluetoothd started" << oendl;
}

OBluetoothDaemon::~OBluetoothDaemon()
{
    odebug << "opiebluetoothd ended" << oendl;
    delete m_agent;
    delete m_hciattach;
    delete m_rfkill;
}

void OBluetoothDaemon::slotAdapterChange()
{
    disconnect( this, SLOT( propertyChanged( const QString& ) ) );
    if( m_btinterface ) {
        connect( m_btinterface, SIGNAL( propertyChanged( const QString& ) ),
                this, SLOT( propertyChanged( const QString& ) ) ) ;

        odebug << "adapter " << m_btinterface->name() << " connected" << oendl;

        // Reset state variables
        m_tempEnabled = false;

        // Set up agent
        m_agent = new OBluetoothAgent( m_btinterface->adapterPath() );
    }
    else {
        if( m_agent ) {
            // It was enabled previously, dump the old agent
            delete m_agent;
            m_agent = NULL;
            sendStatus(true);
            odebug << "adapter disconnected" << oendl;
        }
    }
}

void OBluetoothDaemon::defaultInterfaceChanged( OBluetoothInterface *intf )
{
    m_btinterface = intf;
    slotAdapterChange();
}

void OBluetoothDaemon::propertyChanged(const QString &prop)
{
    if( prop == "Powered" ) {
        sendStatus(true);
    }
    else if( prop == "Discoverable" ) {
        sendStatus(true);
    }
}

void OBluetoothDaemon::slotMessage( const QCString& msg, const QByteArray& data )
{
    if ( msg == "quitDaemon()" )
        qApp->quit();
    else if ( msg == "enableBluetooth()" ) {
        m_tempEnabled = false;
        startBluetooth();
    }
    else if ( msg == "disableBluetooth()" ) {
        m_tempEnabled = false;
        stopBluetooth();
    }
    else if ( msg == "enableBluetoothTemp()" ) {
        if( ! ( m_btinterface && m_btinterface->isUp() ) ) {
            m_tempEnabled = true;
            startBluetooth();
        }
    }
    else if ( msg == "disableBluetoothTemp()" ) {
        if( m_tempEnabled )
            stopBluetooth();
    }
    else if ( msg == "sendStatus()" )
        sendStatus(false);
    else if ( msg == "listDevices()" ) {
        static bool listing = false;
        if( !listing ) {
            listing = true;
            OBluetoothInterface::DeviceIterator devit = m_btinterface->neighbourhood();
            QMap<QString, QString> devmap;
            for( ; devit.current() ; ++devit ) {
                devmap[devit.current()->name()] = devit.current()->macAddress();
            }
            QCopEnvelope e("QPE/BluetoothBack", "devices(QStringMap)");
            e << devmap;
            listing = false;
        }
    }
    else if ( msg == "pairDevice(QString)" ) {
        if( m_agent ) {
            QDataStream stream ( data, IO_ReadOnly );
            QString bdaddr;
            stream >> bdaddr;
            m_agent->pairDevice(bdaddr);
        }
    }
}

void OBluetoothDaemon::startBluetooth()
{
    bool rfkill = m_rfkill->restore();

    if( m_hciattach )
        m_hciattach->start();

    if( !rfkill ) {
        if( m_btinterface ) {
            odebug << "starting bluetooth" << oendl;
            if( ! m_btinterface->setUp( true ) ) {
                odebug << "startBluetooth: failed to power on adapter" << oendl;
                QCopEnvelope e( "QPE/Bluetooth", "error(QString)" );
                e << tr("Failed to power on adapter");
            }
        }
        else {
            odebug << "startBluetooth: no adapter" << oendl;
            QCopEnvelope e( "QPE/Bluetooth", "error(QString)" );
            e << tr("No Bluetooth adapter available");
        }
    }
}

void OBluetoothDaemon::stopBluetooth()
{
    if( m_hciattach )
        m_hciattach->stop();

    if( ! m_rfkill->kill() ) {
        if( ! m_hciattach ) { // no point doing this if hciattach is in use and was stopped
            if( m_btinterface ) {
                odebug << "stopping bluetooth" << oendl;
                if( ! m_btinterface->setUp( false ) ) {
                    odebug << "stopBluetooth: failed to power down adapter" << oendl;
                    QCopEnvelope e( "QPE/Bluetooth", "error(QString)" );
                    e << tr("Failed to power off adapter");
                }
            }
            else {
                odebug << "stopBluetooth: no adapter" << oendl;
                QCopEnvelope e( "QPE/Bluetooth", "error(QString)" );
                e << tr("No Bluetooth adapter available");
            }
        }
    }
}

void OBluetoothDaemon::sendStatus(bool change)
{
    int status = 0;
    if( m_btinterface ) {
        status |= BT_STATUS_ADAPTER;
        if( m_btinterface->isUp() ) {
            status |= BT_STATUS_ENABLED;
            if( m_tempEnabled )
                status |= BT_STATUS_ENABLED_TEMP;
            if( m_btinterface->discoverable() )
                status |= BT_STATUS_DISCOVERABLE;
            if( m_receiveEnabled )
                status |= BT_STATUS_RECEIVE;
        }
    }
    else if( m_rfkill->isAvailable() )
        status |= BT_STATUS_ADAPTER;  // assume the presence of rfkill indicates built-in bluetooth

    QCString msg;
    if( change )
        msg = "statusChange(int)";
    else
        msg = "status(int)";

    QCopEnvelope e( "QPE/Bluetooth", msg );
    e << status;

    odebug << "sent " << msg << " " << status << oendl;
}
