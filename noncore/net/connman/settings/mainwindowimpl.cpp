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


#include "mainwindowimpl.h"
#include "serviceedit.h"
#include "service.h"

/* Opie */
#include <opie2/odebug.h>
#include <opie2/oresource.h>
#include <qpe/qpedialog.h>
#include <qpe/qpeapplication.h>
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

/* Qt */
#include <qwidget.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qheader.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qregexp.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
#include <qpe/qcopenvelope_qws.h>
#endif

/* Std */
#include <stdio.h>

// is this always right?
#define _HOSTFILE "/etc/hostname"
#define _IRDANAME "/proc/sys/net/irda/devname"


MainWindowImpl::MainWindowImpl( QWidget* parent, const char* name, WFlags fl )
        : MainWindow( parent, name, Qt::WStyle_ContextHelp )
{
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

    m_services.setAutoDelete(TRUE);

    m_typeIcons["ethernet"] = OResource::loadPixmap( "connmansettings/wired", OResource::SmallIcon );
    m_typeIcons["wifi"] = OResource::loadPixmap( "connmansettings/wireless", OResource::SmallIcon );
    m_stateIcons["online"] = OResource::loadPixmap( "up", OResource::SmallIcon );
    m_stateIcons["ready"] = m_stateIcons["online"];
    m_stateIcons["idle"] = OResource::loadPixmap( "down", OResource::SmallIcon );
    m_stateIcons["association"] = OResource::loadPixmap( "forward", OResource::SmallIcon );
    m_stateIcons["configuration"] = OResource::loadPixmap( "fastforward", OResource::SmallIcon );
    m_stateIcons["failure"] = OResource::loadPixmap( "reset", OResource::SmallIcon );

    int callId = m_managerProxy->sendWithAsyncReply("GetServices", QValueList<QDBusData>());
    m_calls[callId] = "GetServices";
    callId = m_managerProxy->sendWithAsyncReply("GetTechnologies", QValueList<QDBusData>());
    m_calls[callId] = "GetTechnologies";

    serviceList->header()->hide();

    // Not yet implemented
    addServiceButton->setEnabled(false);

    QObject::connect(connectServiceButton, SIGNAL(clicked()), this, SLOT(slotConnectService()));
    QObject::connect(disconnectServiceButton, SIGNAL(clicked()), this, SLOT(slotDisconnectService()));
    QObject::connect(configureServiceButton, SIGNAL(clicked()), this, SLOT(slotConfigureService()));
    QObject::connect(removeServiceButton, SIGNAL(clicked()), this, SLOT(slotRemoveService()));
    QObject::connect(scanButton, SIGNAL(clicked()), this, SLOT(slotScan()));

    initHostname();
}

MainWindowImpl::~MainWindowImpl()
{
}

void MainWindowImpl::slotAsyncReply( int callId, const QDBusMessage& reply )
{
    QString method = m_calls[callId];
    if (reply.type() == QDBusMessage::ReplyMessage && reply.count() == 1) {
        if (method == "GetServices") {
            m_services.clear();
            QValueList<QDBusData> services = reply[0].toList().toQValueList();
            for( QValueList<QDBusData>::ConstIterator it = services.begin(); it != services.end(); ++it ) {
                QValueList<QDBusData> memberList;
                memberList = (*it).toStruct();
                ServiceListener *listener = new ServiceListener( memberList[0].toObjectPath(), 
                                                                    memberList[1].toStringKeyMap() );
                QObject::connect(listener, SIGNAL(serviceStateChanged(const QDBusObjectPath&, const QString&, const QString&)),
                                this, SLOT(serviceStateChanged(const QDBusObjectPath&, const QString&, const QString&)));
                //QObject::connect(listener, SIGNAL(signalStrength(int)),
                //                this, SLOT(signalStrength(int)));
                m_services.insert(memberList[0].toObjectPath(), listener);
            }
            updateList();
        }
        else if (method == "GetTechnologies") {
            m_techs.clear();
            QValueList<QDBusData> techs = reply[0].toList().toQValueList();
            for( QValueList<QDBusData>::Iterator it = techs.begin(); it != techs.end(); ++it ) {
                QValueList<QDBusData> memberList;
                memberList = (*it).toStruct();
                TechnologyListener *tech = new TechnologyListener( memberList[0].toObjectPath(), 
                                                                   memberList[1].toStringKeyMap() );
                m_techs.insert( memberList[0].toObjectPath(), tech );
            }
        }
    }
}

void MainWindowImpl::slotServiceAsyncReply( int callId, const QDBusMessage& reply )
{
}

void MainWindowImpl::slotDBusSignal(const QDBusMessage& message)
{
    if( message.member() == "ServicesAdded" ) {
        int callId = m_managerProxy->sendWithAsyncReply("GetServices", QValueList<QDBusData>());
        m_calls[callId] = "GetServices";
    }
    else if( message.member() == "ServicesRemoved" ) {
        int callId = m_managerProxy->sendWithAsyncReply("GetServices", QValueList<QDBusData>());
        m_calls[callId] = "GetServices";
    }
    else if( message.member() == "TechnologyAdded" ) {
        TechnologyListener *tech = new TechnologyListener( message[0].toObjectPath(),
                                                           message[1].toStringKeyMap() );
        m_techs.insert( message[0].toObjectPath(), tech );
    }
    else if( message.member() == "TechnologyRemoved" ) {
        m_techs.remove( message[0].toObjectPath() );
    }
}

void MainWindowImpl::serviceStateChanged( const QDBusObjectPath &path, const QString &oldstate, const QString &newstate )
{
    ServiceListener *service = m_services[path];
    if( service ) {
        for( QListViewItem *item = serviceList->firstChild(); item; item = item->nextSibling() ) {
            if( item->text(3) == QString(path) ) {
                updateListItem( item, service );
                break;
            }
        }
    }
}

void MainWindowImpl::updateList()
{
    QListViewItem *item = serviceList->currentItem();
    QString lastItemId;
    if( item )
        lastItemId = item->text(3);

    serviceList->clear();

    QListViewItem *lastItem = NULL;
    item = NULL;
    for( QDictIterator<ServiceListener> it(m_services); it.current(); ++it ) {
        item = new QListViewItem(serviceList, item);

        updateListItem( item, it.current() );

        QObject::connect(it.current()->proxy(), SIGNAL(asyncReply(int, const QDBusMessage&)),
                        this, SLOT(slotServiceAsyncReply(int, const QDBusMessage&)));

        if( !lastItemId.isEmpty() && lastItem == NULL && lastItemId == it.currentKey() )
            lastItem = item;
    }

    if( lastItem ) {
        serviceList->ensureItemVisible(lastItem);
        serviceList->setCurrentItem(lastItem);
    }
}

void MainWindowImpl::updateListItem( QListViewItem *item, ServiceListener *service )
{
    item->setPixmap( 0, m_stateIcons[service->state()] );

    QString type = service->serviceType();
    QPixmap pix = m_typeIcons[type];

    item->setPixmap( 1, pix );
    item->setText(2, service->serviceName());;
    item->setText(3, service->proxy()->path());
}

ServiceListener *MainWindowImpl::selectedService()
{
    QListViewItem *item = serviceList->currentItem();
    if(item) {
        ServiceListener *service = m_services[item->text(3)];
        if( service )
            return service;
    }
    return NULL;
}

void MainWindowImpl::slotConnectService()
{
    ServiceListener *service = selectedService();
    if( service ) {
        int callId = service->proxy()->sendWithAsyncReply("Connect", QValueList<QDBusData>());
    }
}

void MainWindowImpl::slotDisconnectService()
{
    ServiceListener *service = selectedService();
    if( service ) {
        int callId = service->proxy()->sendWithAsyncReply("Disconnect", QValueList<QDBusData>());
    }
}

void MainWindowImpl::slotConfigureService()
{
    ServiceListener *service = selectedService();
    if( service ) {
        ConnManServiceEditor *pe = new ConnManServiceEditor( service->proxy(), this, "blar", WType_Modal | WStyle_NormalBorder );
        pe->showMaximized();
        pe->exec();
        if(pe->result()) {
            //lbServices->changeItem(pe->getServiceName(), lbServices->currentItem());
        }
        delete pe;
    }
}

void MainWindowImpl::slotRemoveService()
{
    ServiceListener *service = selectedService();
    if( service ) {
        if( QMessageBox::warning(this, tr("Remove"),
                "<p>" + tr("Are you sure you want to remove all settings for the service '%1'?").arg(service->serviceName()) + "</p>",
                tr("Remove"),tr("Cancel"),0,1,0) == 0 ) {
            int callId = service->proxy()->sendWithAsyncReply("Remove", QValueList<QDBusData>());
        }
    }
}

void MainWindowImpl::slotScan()
{
    for( QDictIterator<TechnologyListener> it(m_techs); it.current(); ++it ) {
        if( it.current()->isPowered() ) {
            it.current()->proxy()->sendWithAsyncReply("Scan", QValueList<QDBusData>());
        }
    }
}

void MainWindowImpl::setHostname()
{
    static QRegExp filter("[^A-Za-z0-9_\\-\\.]");
    if (filter.match(m_Nameinput->text())!=-1) {
        odebug << "Wrong hostname" << oendl;
        QMessageBox::critical(0, tr("Error"), tr("This is an invalid hostname.<br>Please use A-Z, a-z, _, - or a single dot."));
        return;
    }

    OProcess h;
    m_procTemp = "";
    h << "hostname" << m_Nameinput->text();
    connect(&h,SIGNAL(receivedStderr(Opie::Core::OProcess*,char*,int)),this,SLOT(slotHostname(Opie::Core::OProcess*,char*,int)));
    if( !h.start( OProcess::Block, OProcess::Stderr ))
        owarn << "Failed execution of 'hostname'. Are the paths correct?" << oendl;
    odebug << "Got " << m_procTemp << " - " << h.exitStatus() << oendl;
    if( h.exitStatus() != 0 ) {
        QMessageBox::critical(0, tr("Error"), "<p>" + QString(tr("Could not set hostname:\n\n%1")).arg(m_procTemp.stripWhiteSpace()));
        return;
    }
    m_procTemp = "";

    QFile f(_HOSTFILE);
    if (f.open(IO_Truncate|IO_WriteOnly)) {
        QTextStream s(&f);
        s << m_Nameinput->text();
    }
    else {
        QMessageBox::critical(0, tr("Error"), tr("Could not save name."));
        return;
    }

    f.close();
    f.setName(_IRDANAME);
    if (f.open(IO_WriteOnly)) {
        QTextStream s(&f);
        s << m_Nameinput->text();
    }
    else {
        owarn << "Could not set IrDA name." << oendl;
    }
}

void MainWindowImpl::initHostname()
{
    OProcess h;
    m_procTemp = "";

    h << "hostname";
    connect(&h,SIGNAL(receivedStdout(Opie::Core::OProcess*,char*,int)),this,SLOT(slotHostname(Opie::Core::OProcess*,char*,int)));
    if (!h.start(OProcess::Block,OProcess::AllOutput))
        owarn << "Could not execute 'hostname'. Are the paths correct?" oendl;
    odebug << "Got " << m_procTemp <<oendl;
    m_Nameinput->setText(m_procTemp.stripWhiteSpace());
    m_procTemp = "";
}

void MainWindowImpl::slotHostname(Opie::Core::OProcess * /*proc*/, char *buffer, int buflen)
{
    if( buflen < 1 || buffer == 0 ) 
        return;
    char*_t = new char[buflen+1];
    ::memset(_t,0,buflen+1);
    ::memcpy(_t,buffer,buflen);
    m_procTemp += _t;
    delete[]_t;
}
