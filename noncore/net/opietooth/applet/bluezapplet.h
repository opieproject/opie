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

#ifndef __BLUEZAPPLET_H__
#define __BLUEZAPPLET_H__

#include <qwidget.h>
#include <qpixmap.h>
#include <qtimer.h>

namespace OpieTooth {
    class Device;

    class BluezApplet : public QWidget {
        Q_OBJECT
    public:
        BluezApplet( QWidget *parent = 0, const char *name=0 );
        ~BluezApplet();

    protected:
        void timerEvent(QTimerEvent *te );

public slots:
    private:
        void mousePressEvent( QMouseEvent * );
        void paintEvent( QPaintEvent* );
        void launchManager();
        int checkBluezStatus();
        int setBluezStatus(int);
        int checkBluezDiscoveryStatus();
        int setBluezDiscoveryStatus(int);
        int sockfd;

    private:
        Device* btDevice;
        QPixmap bluezOnPixmap;
        QPixmap bluezOffPixmap;
        QPixmap bluezDiscoveryOnPixmap;
        int bluezactive;
        int bluezDiscoveryActive;

private slots:


    };
};


#endif

