/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Maximilian Reiss <max.reiss@gmx.de>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <,   >  .   <=          redistribute it and/or  modify it under
:=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/


#include "bluezapplet.h"
#include <qapplication.h>

#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/resource.h>
#include <qpe/config.h>

#include <qpoint.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qtimer.h>
#include <qpopupmenu.h>

#include <device.h>

namespace OpieTooth {

    BluezApplet::BluezApplet( QWidget *parent, const char *name ) : QWidget( parent, name ) {
        setFixedHeight( 18 );
        setFixedWidth( 14 );
        bluezOnPixmap = Resource::loadPixmap( "bluetoothapplet/bluezon" );
        bluezOffPixmap = Resource::loadPixmap( "bluetoothapplet/bluezoff" );
        //    bluezDiscoveryOnPixmap = Resource::loadPixmap( "bluetoothapplet/magglass" );
        startTimer(5000);
        btDevice = 0;

    }

    BluezApplet::~BluezApplet() {
    }

    bool BluezApplet::checkBluezStatus() {
        if (btDevice) {
            if (btDevice->isLoaded() ) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    int BluezApplet::setBluezStatus(int c) {

        if (c == 1) {
            btDevice = new Device("/dev/ttySB0", "csr" );
        } else {
            if (btDevice) {
                delete btDevice;
                btDevice = 0;
            }
       }
        return 0;
    }

    int BluezApplet::checkBluezDiscoveryStatus() {
    }

    int BluezApplet::setBluezDiscoveryStatus(int d) {
    }

    void BluezApplet::mousePressEvent( QMouseEvent *) {

        QPopupMenu *menu = new QPopupMenu();
        QPopupMenu *signal = new QPopupMenu();
        int ret=0;

        /* Refresh active state */
        timerEvent(NULL);


        if (bluezactive) {
            menu->insertItem( tr("Disable Bluetooth"), 0 );
        } else {
            menu->insertItem( tr("Enable Bluetooth"), 1 );
        }

        menu->insertItem( tr("Launch manager"), 2 );

        menu->insertSeparator(6);
        menu->insertItem( tr("Signal strength"), signal,  5);
        menu->insertSeparator(8);

        if (bluezDiscoveryActive) {
            menu->insertItem( tr("Disable discovery"), 3 );
        } else {
            menu->insertItem( tr("Enable discovery"), 4 );
        }


        QPoint p = mapToGlobal( QPoint(1, -menu->sizeHint().height()-1) );
        ret = menu->exec(p, 0);

        switch(ret) {
        case 0:
            setBluezStatus(0);
            timerEvent(NULL);
            break;
        case 1:
            setBluezStatus(1);
            timerEvent(NULL);
            break;
        case 2:
            // start bluetoothmanager
            launchManager();
            timerEvent(NULL);
            break;
        case 3:
            setBluezDiscoveryStatus(0);
            timerEvent(NULL);
            break;
        case 4:
            setBluezDiscoveryStatus(1);
            timerEvent(NULL);
            break;
            //case 7:
            // With table of currently-detected devices.
        }

        timerEvent(NULL);
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
 * Refresh timer
 * @param the timer event
 */
    void BluezApplet::timerEvent( QTimerEvent * ) {
        bool oldactive = bluezactive;
        int olddiscovery = bluezDiscoveryActive;

        bluezactive = checkBluezStatus();
        bluezDiscoveryActive = checkBluezDiscoveryStatus();

        if ((bluezactive != oldactive) || (bluezDiscoveryActive != olddiscovery)) {
            paintEvent(NULL);
        }
    }

/**
 * Implementation of the paint event
 * @param the QPaintEvent
 */
    void BluezApplet::paintEvent( QPaintEvent* ) {
        QPainter p(this);
        qDebug("paint bluetooth pixmap");

        p.eraseRect ( 0, 0, this->width(), this->height() );

        if (bluezactive > 0) {
            p.drawPixmap( 0, 1,  bluezOnPixmap );
        } else {
            p.drawPixmap( 0, 1,  bluezOffPixmap );
        }

        if (bluezDiscoveryActive > 0) {
            p.drawPixmap( 0, 1,  bluezDiscoveryOnPixmap );
        }
    }
};
