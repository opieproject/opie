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



#include "connmanapplet.h"

/* OPIE */
#include <opie2/otaskbarapplet.h>
#include <opie2/odebug.h>
#include <opie2/oresource.h>
#include <qpe/version.h>
#include <qpe/applnk.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/config.h>

using namespace Opie::Core;

// Qt DBUS includes
#include <dbus/qdbusproxy.h>
#include <dbus/qdbusdatalist.h>
#include <dbus/qdbusdatamap.h>
#include <dbus/qdbuserror.h>
#include <dbus/qdbusmessage.h>
#include <dbus/qdbusobjectpath.h>
#include <dbus/qdbusvariant.h>

/* QT */
#include <qapplication.h>
#include <qpoint.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qtextstream.h>
#include <qtimer.h>


ServiceListener::ServiceListener( const QDBusObjectPath &path ): QObject( 0, 0 )
{
    QDBusConnection connection = QDBusConnection::systemBus();
    m_proxy = new QDBusProxy(this);
    m_proxy->setService("net.connman");
    m_proxy->setPath(path);
    m_proxy->setInterface("net.connman.Service");
    m_proxy->setConnection(connection);

    QObject::connect(m_proxy, SIGNAL(asyncReply(int, const QDBusMessage&)),
                    this, SLOT(slotAsyncReply(int, const QDBusMessage&)));
    QObject::connect(m_proxy, SIGNAL(dbusSignal(const QDBusMessage&)),
                    this, SLOT(slotDBusSignal(const QDBusMessage&)));

    m_proxy->sendWithAsyncReply("GetProperties", QValueList<QDBusData>());
}

ServiceListener::~ServiceListener()
{
}

void ServiceListener::slotAsyncReply( int callId, const QDBusMessage& reply )
{
    if (reply.type() == QDBusMessage::ReplyMessage && reply.count() == 1) {
        const QDBusDataMap<QString> map = reply[0].toStringKeyMap();
        QDBusDataMap<QString>::ConstIterator it = map.begin();
        for (; it != map.end(); ++it) {
            if( it.key() == "Name" ) {
                m_serviceName = it.data().toVariant().value.toString();
            }
            else if( it.key() == "State" ) {
                m_state = it.data().toVariant().value.toString();
            }
        }
    }
}

void ServiceListener::slotDBusSignal( const QDBusMessage& message )
{
    odebug << "ConnMan: " << message.member() << oendl;
    if( message.member() == "PropertyChanged" ) {
        QString prop = message[0].toString();
        if( prop == "State" ) {
            QString oldState = m_state;
            QString newState = message[1].toVariant().value.toString();
            if( oldState != newState ) {
                emit serviceStateChanged( m_serviceName, oldState, newState );
                m_state = newState;
            }
        }
    }
}



ConnManApplet::ConnManApplet( QWidget *parent, const char *name ) : QWidget( parent, name )
{
    setFixedHeight( AppLnk::smallIconSize() );
    setFixedWidth( AppLnk::smallIconSize() );

    m_brokenPix = OResource::loadImage( "connmanapplet/off", OResource::SmallIcon );
    m_offlinePix = OResource::loadImage( "connmanapplet/disconnected", OResource::SmallIcon );
    m_onlinePix = OResource::loadImage( "connmanapplet/connected", OResource::SmallIcon );

    m_services.setAutoDelete(TRUE);

    m_connection = QDBusConnection::systemBus();
    m_managerProxy = new QDBusProxy(this);
    m_managerProxy->setService("net.connman");
    m_managerProxy->setPath("/");
    m_managerProxy->setInterface("net.connman.Manager");
    m_managerProxy->setConnection(m_connection);

    QObject::connect(m_managerProxy, SIGNAL(asyncReply(int, const QDBusMessage&)),
                    this, SLOT(slotAsyncReply(int, const QDBusMessage&)));
    QObject::connect(m_managerProxy, SIGNAL(dbusSignal(const QDBusMessage&)),
                    this, SLOT(slotDBusSignal(const QDBusMessage&)));

    int callId = m_managerProxy->sendWithAsyncReply("GetProperties", QValueList<QDBusData>());
    m_calls[callId] = "GetProperties";
}

ConnManApplet::~ConnManApplet()
{
}

int ConnManApplet::position()
{
    return 6;
}

void ConnManApplet::mousePressEvent( QMouseEvent *)
{
    QPopupMenu *menu = new QPopupMenu();

    QStringList techs;
    int i=1;
    for( QMap<QString,bool>::Iterator it = m_techs.begin(); it != m_techs.end(); ++it ) {
        QString tech = it.key();
        // Bluetooth seems not to be enable-able and you shouldn't need to
        // control ethernet in this way (just unplug it!)
        if( tech == "bluetooth" || tech == "ethernet" )
            continue;

        if( it.data() )
            menu->insertItem( tr("Disable %1").arg( tech ), i );
        else
            menu->insertItem( tr("Enable %1").arg( tech ), i );
        techs += tech;
        i++;
    }

    menu->insertSeparator();
    menu->insertItem( tr("Launch manager"), 0 );

    QPoint p = mapToGlobal( QPoint(1, -menu->sizeHint().height()-1) );
    int ret = menu->exec(p, 0);
    menu->hide();

    if( ret == 0 ) {
        launchSettings();
    }
    else if( ret>0 ) {
        enableTechnology( techs[ret-1], !m_techs[techs[ret-1]] );
    }

    delete menu;
}


/**
 * Launches the ConnMan manager
 */
void ConnManApplet::launchSettings()
{
    QCopEnvelope e("QPE/System", "execute(QString)");
    e << QString("connmansettings");
}


void ConnManApplet::enableTechnology( const QString &tech, bool enable )
{
    QValueList<QDBusData> params;
    params << QDBusData::fromString( tech );
    QString call;
    if( enable )
        call = "EnableTechnology";
    else
        call = "DisableTechnology";
    int callId = m_managerProxy->sendWithAsyncReply(call, params);
    m_calls[callId] = call;
}


/**
 * Implementation of the paint event
 * @param the QPaintEvent
 */
void ConnManApplet::paintEvent( QPaintEvent* )
{
    QPainter p(this);
    if( m_state == "offline" )
        p.drawPixmap( 0, 0, m_offlinePix );
    else if( m_state == "online" || m_state == "connected" )
        p.drawPixmap( 0, 0, m_onlinePix );
    else
        p.drawPixmap( 0, 0, m_brokenPix );
}

void ConnManApplet::slotAsyncReply( int callId, const QDBusMessage& reply )
{
    QString method = m_calls[callId];
    if (reply.type() == QDBusMessage::ReplyMessage && reply.count() == 1) {
        const QDBusDataMap<QString> map = reply[0].toStringKeyMap();
        QDBusDataMap<QString>::ConstIterator it = map.begin();
        for (; it != map.end(); ++it) {
            managerPropertySet( it.key(), it.data().toVariant() );
        }
    }
}

void ConnManApplet::slotDBusSignal(const QDBusMessage& message)
{
    odebug << "ConnMan: " << message.member() << oendl;
    if( message.member() == "PropertyChanged" ) {
        managerPropertySet( message[0].toString(), message[1].toVariant() );
    }
}

void ConnManApplet::managerPropertySet( const QString &prop, const QDBusVariant &propval )
{
    if( prop == "State" ) {
        QString state = propval.value.toString();
        if( m_state != state ) {
            m_state = state;
            update();
        }
    }
    else if( prop == "AvailableTechnologies" ) {
        QMap<QString,bool> techs;
        QStringList availTechs = propval.value.toList().toQStringList();
        for( QStringList::Iterator it = availTechs.begin(); it != availTechs.end(); ++it ) {
            if( m_techs.contains(*it) )
                techs[*it] = m_techs[*it];
            else
                techs[*it] = false;
        }
        m_techs = techs;
    }
    else if( prop == "EnabledTechnologies" ) {
        for( QMap<QString,bool>::Iterator it = m_techs.begin(); it != m_techs.end(); ++it )
            m_techs[it.key()] = false;

        QStringList enabledTechs = propval.value.toList().toQStringList();
        for( QStringList::Iterator it = enabledTechs.begin(); it != enabledTechs.end(); ++it ) {
            m_techs[*it] = true;
        }
    }
    else if( prop == "Services" ) {
        m_servicePaths = propval.value.toList().toObjectPathList();
        QTimer::singleShot( 0, this, SLOT( updateServices()) );
    }
}

void ConnManApplet::updateServices()
{
    m_services.clear();
    QValueList<QDBusObjectPath>::ConstIterator it2 = m_servicePaths.begin();
    for (; it2 != m_servicePaths.end(); ++it2) {
        ServiceListener *listener = new ServiceListener((*it2));
        QObject::connect(listener, SIGNAL(serviceStateChanged(const QString&, const QString&, const QString&)),
                        this, SLOT(serviceStateChanged(const QString&, const QString&, const QString&)));
        m_services.insert( (*it2), listener );
    }
}

void ConnManApplet::serviceStateChanged( const QString &name, const QString &oldstate, const QString &newstate )
{
    QString msg = "";
    odebug << "serviceStateChanged( " << name << ", " << oldstate << ", " << newstate << " )" << oendl;
    if( newstate == "ready" || ( newstate == "online" && oldstate != "ready" ) ) {
        msg = tr("Connected to %1").arg(name);
    }

    if( !msg.isEmpty() ) {
        QCopEnvelope e("QPE/TaskBar", "message(QString,QString)");
        e << msg;
        e << QString("connmanapplet");
    }
}

EXPORT_OPIE_APPLET_v1( ConnManApplet )

