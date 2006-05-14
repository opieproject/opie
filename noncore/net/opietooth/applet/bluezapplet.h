/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Maximilian Reiss <max.reiss@gmx.de>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <,   >  .   <=           redistribute it and/or  modify it under
:=1 )Y*s>-.--   :            the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
     ._= =}       :
    .%+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|     MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>:      PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .     .:        details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=            this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef __BLUEZAPPLET_H__
#define __BLUEZAPPLET_H__

#include <qwidget.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <manager.h>
#include <opie2/oprocess.h>

namespace OpieTooth {
    class Device;

    class BluezApplet : public QWidget {
        Q_OBJECT
    public:
        BluezApplet( QWidget *parent = 0, const char *name=0 );
        ~BluezApplet();
        static int position();
    protected:
        void timerEvent(QTimerEvent *te );

public slots:
        void fillList( const QString& device, RemoteDevice::ValueList list );

    private:
        void mousePressEvent( QMouseEvent * );
        void paintEvent( QPaintEvent* );
        void launchManager();
        bool checkBluezStatus();
        int setBluezStatus(int, bool sync = false);
        int checkBluezDiscoveryStatus();
        int setBluezDiscoveryStatus(int);

    private:
        Device* btDevice;
        Manager *btManager;
        QPixmap bluezOnPixmap;
        QPixmap bluezOffPixmap;
        QPixmap bluezDiscoveryOnPixmap;
        bool bluezactive;
        bool bluezDiscoveryActive;
        bool doListDevice; //If I have to list devices after bringing BT up?
        bool isScanning; //If I'm scanning devices
        bool m_wasOn; //If BT was started by menu?
    protected:
        bool m_sync; //If we have to bring BT synchronously

private slots:
        void slotMessage( const QCString& , const QByteArray& );
        /**
         * Reacts on device up
         * @param name device name
         * @param up if device was brought up
         */
        void slotDevice(const QString&, bool);
    };
};


#endif

