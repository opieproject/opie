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


#ifndef CONNMANAPPLET_H
#define CONNMANAPPLET_H

class QDBusProxy;

#include <qwidget.h>
#include <qpixmap.h>
#include <dbus/qdbusconnection.h>
#include <dbus/qdbusvariant.h>
#include <qmap.h>
#include <qstring.h>

class ConnManApplet: public QWidget {
    Q_OBJECT
public:
    ConnManApplet( QWidget *parent = 0, const char *name=0 );
    ~ConnManApplet();
    static int position();

protected slots:
    void slotAsyncReply( int callId, const QDBusMessage& reply );
    void slotDBusSignal( const QDBusMessage& message );

private:
    void mousePressEvent( QMouseEvent * );
    void paintEvent( QPaintEvent* );
    void launchSettings();
    void managerPropertySet( const QString &prop, const QDBusVariant &propval );
    void enableTechnology( const QString &tech, bool enable );

private:
    QPixmap m_brokenPix;
    QPixmap m_offlinePix;
    QPixmap m_onlinePix;
    QDBusConnection m_connection;
    QDBusProxy *m_managerProxy;
    QMap<int,QString> m_calls;
    QString m_state;
    QMap<QString,bool> m_techs;

};

#endif

