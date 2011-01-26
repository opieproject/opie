/*
                             This file is part of the Opie Project
             =.              Copyright (c) 2002 Maximilian Reiss <max.reiss@gmx.de>
            .=l.             Copyright (c) 2011 Paul Eggleton <paul.eggleton@linux.intel.com>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
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


#include "bluezapplet.h"

/* OPIE */
#include <opie2/otaskbarapplet.h>
#include <opie2/odebug.h>
#include <opie2/oresource.h>
#include <opie2/oprocess.h>
#include <qpe/version.h>
#include <qpe/applnk.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/config.h>

using namespace Opie::Core;

/* QT */
#include <qapplication.h>
#include <qpoint.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qtimer.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qtextstream.h>

namespace OpieTooth {
    BluezApplet::BluezApplet( QWidget *parent, const char *name ) : QWidget( parent, name )
    {
        setFixedHeight( AppLnk::smallIconSize() );
        setFixedWidth( AppLnk::smallIconSize() );

        bluezOnPixmap = OResource::loadImage( "bluetoothapplet/bluezon", OResource::SmallIcon );
        bluezOffPixmap = OResource::loadImage( "bluetoothapplet/bluezoff", Opie::Core::OResource::SmallIcon );
        bluezDiscoveryOnPixmap = OResource::loadImage( "bluetoothapplet/bluezondiscovery", Opie::Core::OResource::SmallIcon );
        bluezReceiveOnPixmap = OResource::loadImage( "bluetoothapplet/bluezonreceive", Opie::Core::OResource::SmallIcon );

        m_adapter = false;
        m_powered = false;
        bluezDiscoveryActive = false;
        bluezReceiveActive = false;
        bluezReceiveChanged = false;
        doListDevice = false;
        isScanning = false;

        // TODO: determine whether this channel has to be closed at destruction time.
        QCopChannel* chan = new QCopChannel("QPE/Bluetooth", this );
        connect(chan, SIGNAL(received(const QCString&,const QByteArray&) ),
                this, SLOT(slotMessage(const QCString&,const QByteArray&) ) );

        m_statusRequested = true;
        QCopEnvelope e("QPE/Bluetooth", "sendStatus()");
    }

    BluezApplet::~BluezApplet()
    {
    }

    int BluezApplet::position()
    {
        return 6;
    }

    void BluezApplet::setBluezStatus( bool enable )
    {
        if( enable )
            QCopEnvelope e("QPE/Bluetooth", "enableBluetooth()");
        else
            QCopEnvelope e("QPE/Bluetooth", "disableBluetooth()");
    }

    int BluezApplet::setObexRecieveStatus(int d)
    {
        {
            QCopEnvelope e ( "QPE/Obex", "btreceive(int)" );
            e << ( d ? 1 : 0 );
        }
        bluezReceiveActive = (bool)(d != 0);
        bluezReceiveChanged = true;
        return d;
    }

    // receiver for QCopChannel("QPE/Bluetooth") messages.
    void BluezApplet::slotMessage( const QCString& str, const QByteArray& data )
    {
        QDataStream stream ( data, IO_ReadOnly );
         /*
        if ( str == "listDevices()") {
            if (checkBluezStatus()) {
                doListDevice = false;
                if (!btManager) {
                    btManager = new Manager("hci0");
                    connect( btManager,
                        SIGNAL( foundDevices(const QString&, RemoteDevice::ValueList) ),
                        this, SLOT( fillList(const QString&, RemoteDevice::ValueList) ) ) ;
                }
                btManager->searchDevices();
                isScanning = true;
            } else
                doListDevice = true;
        }
        */
        if( str == "statusChange(int)" || ( str == "status(int)" && m_statusRequested ) ) {
            if(str == "status(int)")
                m_statusRequested = false;
            int status;
            stream >> status;
            // FIXME constants
            m_adapter = (( status & 1 ) == 1);
            m_powered = (( status & 2 ) == 2);
            update();
        }
    }

    // Once the hcitool scan is complete, report back.
    /*void BluezApplet::fillList(const QString&, RemoteDevice::ValueList deviceList)
    {
        QCopEnvelope e("QPE/BluetoothBack", "devices(QStringMap)");

        QMap<QString, QString> btmap;

        RemoteDevice::ValueList::Iterator it;
        for( it = deviceList.begin(); it != deviceList.end(); ++it )
            btmap[(*it).name()] = (*it).mac();

        e << btmap;
        isScanning = false;
    }*/

    void BluezApplet::mousePressEvent( QMouseEvent *) {

        QPopupMenu *menu = new QPopupMenu();
        QPopupMenu *signal = new QPopupMenu();
        int ret=0;

        if (m_powered) {
            menu->insertItem( tr("Disable Bluetooth"), 0 );
        }
        else {
            menu->insertItem( tr("Enable Bluetooth"), 1 );
            menu->setItemEnabled(1, m_adapter);
        }

        menu->insertItem( tr("Launch manager"), 2 );

        menu->insertSeparator(6);
        //menu->insertItem( tr("Signal strength"), signal,  5);
        //menu->insertSeparator(8);

        if (m_powered) {
            if (bluezReceiveActive) {
                menu->insertItem( tr("Disable receive"), 3 );
            }
            else {
                menu->insertItem( tr("Enable receive"), 4 );
            }
        }

        QPoint p = mapToGlobal( QPoint(1, -menu->sizeHint().height()-1) );
        ret = menu->exec(p, 0);
        menu->hide();

        switch(ret) {
        case 0:
            setBluezStatus(false);
            break;
        case 1:
            setBluezStatus(true);
            break;
        case 2:
            // start bluetoothmanager
            launchManager();
            break;
        case 3:
            setObexRecieveStatus(0);
            break;
        case 4:
            setObexRecieveStatus(1);
            break;
            //case 7:
            // With table of currently-detected devices.
        }

        delete signal;
        delete menu;
    }


/**
 * Launches the bluetooth manager
 */
    void BluezApplet::launchManager() {
        QCopEnvelope e("QPE/System", "execute(QString)");
        e << QString("bluetooth-manager");
    }

/**
 * Implementation of the paint event
 * @param the QPaintEvent
 */
    void BluezApplet::paintEvent( QPaintEvent* ) {
        QPainter p(this);
        odebug << "paint bluetooth pixmap" << oendl;

        if (m_powered) {
            p.drawPixmap( 0, 0,  bluezOnPixmap );
            if (bluezDiscoveryActive)
                p.drawPixmap( 0, 0,  bluezDiscoveryOnPixmap );
            if (bluezReceiveActive)
                p.drawPixmap( 0, 0,  bluezReceiveOnPixmap );
        }
        else
            p.drawPixmap( 0, 0,  bluezOffPixmap );
    }
};


EXPORT_OPIE_APPLET_v1( OpieTooth::BluezApplet )

