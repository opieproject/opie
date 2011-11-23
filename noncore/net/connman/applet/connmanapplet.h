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
class QDialog;

#include <qwidget.h>
#include <qpixmap.h>
#include <dbus/qdbusconnection.h>
#include <dbus/qdbusvariant.h>
#include <dbus/qdbusobject.h>
#include <dbus/qdbusobjectpath.h>
#include <qmap.h>
#include <qdict.h>
#include <qstring.h>
#include <qvaluelist.h>

class ServiceListener: public QObject {
    Q_OBJECT
public:
    ServiceListener( const QDBusObjectPath &path, int seq );
    ~ServiceListener();

signals:
    void serviceStateChanged( const QString &name, const QString &oldstate, const QString &newstate );
    void signalStrength( int strength );

protected slots:
    void slotAsyncReply( int callId, const QDBusMessage& reply );
    void slotDBusSignal( const QDBusMessage& message );

private:
    QDBusProxy *m_proxy;
    QString m_serviceName;
    QString m_state;
    int m_strength;
    int m_seq;
};

class ConnManApplet: public QWidget, public QDBusObjectBase {
    Q_OBJECT
public:
    ConnManApplet( QWidget *parent = 0, const char *name=0 );
    ~ConnManApplet();
    static int position();

protected:
    virtual bool handleMethodCall(const QDBusMessage& message);

protected slots:
    void slotAsyncReply( int callId, const QDBusMessage& reply );
    void slotDBusSignal( const QDBusMessage& message );
    void updateServices();
    void serviceStateChanged( const QString &name, const QString &oldstate, const QString &newstate );
    void signalStrength( int strength );

private:
    void mousePressEvent( QMouseEvent * );
    void paintEvent( QPaintEvent* );
    void launchSettings();
    void managerPropertySet( const QString &prop, const QDBusVariant &propval );
    void enableTechnology( const QString &tech, bool enable );
    void showDialog( const QDBusMessage& message, const QDBusDataMap<QString> &fields );
    void destroyDialog();

private:
    QPixmap m_brokenPix;
    QPixmap m_offlinePix;
    QPixmap m_onlinePix;
    QMap<int,QPixmap> m_strengthPix;
    QDBusConnection m_connection;
    QDBusProxy *m_managerProxy;
    QMap<int,QString> m_calls;
    QString m_state;
    int m_strength;
    QMap<QString,bool> m_techs;
    QValueList<QDBusObjectPath> m_servicePaths;
    QDict<ServiceListener> m_services;
    QDialog *m_agentDlg;
};

#endif

