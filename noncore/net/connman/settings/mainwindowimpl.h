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

#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H

#include <qstring.h>
#include <qmap.h>
#include <qdict.h>
#include <qpixmap.h>

#include <dbus/qdbusconnection.h>
#include <dbus/qdbusobjectpath.h>
#include <dbus/qdbusproxy.h>

#include <opie2/oprocess.h>

#include "mainwindow.h"
#include "service.h"
#include "technology.h"


class MainWindowImpl : public MainWindow
{
    Q_OBJECT

public:
    MainWindowImpl( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~MainWindowImpl();

    static QString appName() { return QString::fromLatin1("connmansettings"); }

protected slots:
    void slotConnectService();
    void slotDisconnectService();
    void slotConfigureService();
    void slotRemoveService();
    void slotScan();
    void slotAsyncReply( int callId, const QDBusMessage& reply );
    void slotServiceAsyncReply( int callId, const QDBusMessage& reply );
    void slotDBusSignal( const QDBusMessage& message );
    void setHostname();
    void slotHostname(Opie::Core::OProcess *proc, char *buffer, int buflen);
    void updateList();
    void serviceStateChanged( const QDBusObjectPath &path, const QString &oldstate, const QString &newstate );

protected:
    void initHostname();
    ServiceListener *selectedService();
    void updateListItem( QListViewItem *item, ServiceListener *service );
    void addServices( const QValueList<QDBusData> &services );
    void removeServices( const QValueList<QDBusObjectPath> &services );

protected:
    QDBusConnection m_connection;
    QDBusProxy *m_managerProxy;
    QMap<int,QString> m_calls;
    QDict<ServiceListener> m_services;
    QDict<TechnologyListener> m_techs;
    QString m_procTemp;
    QValueList<QDBusObjectPath> m_servicePaths;
    QMap<QString,QPixmap> m_stateIcons;
    QMap<QString,QPixmap> m_typeIcons;
    QPixmap m_securedIcon;
    QPixmap m_unsecuredIcon;
};

#endif // MAINWINDOWIMPL_H
