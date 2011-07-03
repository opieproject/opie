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

    int callId = m_managerProxy->sendWithAsyncReply("GetProperties", QValueList<QDBusData>());
    m_calls[callId] = "GetProperties";

    serviceList->header()->hide();

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
    if( method == "GetProperties" ) {
        if (reply.type() == QDBusMessage::ReplyMessage && reply.count() == 1) {
            const QDBusDataMap<QString> map = reply[0].toStringKeyMap();
            QDBusDataMap<QString>::ConstIterator it = map.begin();
            for (; it != map.end(); ++it) {
                if( it.key() == "Services" ) {
                    m_servicePaths = it.data().toVariant().value.toList().toObjectPathList();
                    QTimer::singleShot( 0, this, SLOT( updateList()) );
                }
            }
        }
    }
    else {
        if (reply.type() == QDBusMessage::ReplyMessage) {
            odebug << method << " success?" << oendl;
        }
        else {
            odebug << method << " failed: " << reply.error().name() << ": " << reply.error().message() << oendl;
        }
    }
}

void MainWindowImpl::slotServiceAsyncReply( int callId, const QDBusMessage& reply )
{
}

void MainWindowImpl::slotDBusSignal(const QDBusMessage& message)
{
    if( message.member() == "PropertyChanged" ) {
        QString prop = message[0].toString();
        if( prop == "Services" ) {
            m_servicePaths = message[1].toVariant().value.toList().toObjectPathList();
            QTimer::singleShot( 0, this, SLOT( updateList()) );
        }
    }
}

void MainWindowImpl::updateList()
{
    QListViewItem *item = serviceList->currentItem();
    QString lastItemId;
    if( item )
        lastItemId = item->text(3);
    
    QPixmap wiredPix = OResource::loadPixmap( "connmansettings/wired", OResource::SmallIcon );
    QPixmap wirelessPix = OResource::loadPixmap( "connmansettings/wireless", OResource::SmallIcon );
    QMap<QString,QPixmap> stateIcons;
    stateIcons["online"] = OResource::loadPixmap( "up", OResource::SmallIcon );
    stateIcons["ready"] = stateIcons["online"];
    stateIcons["idle"] = OResource::loadPixmap( "down", OResource::SmallIcon );
    stateIcons["association"] = OResource::loadPixmap( "forward", OResource::SmallIcon );
    stateIcons["configuration"] = OResource::loadPixmap( "fastforward", OResource::SmallIcon );
    stateIcons["failure"] = OResource::loadPixmap( "reset", OResource::SmallIcon );

    m_services.clear();
    serviceList->clear();

    QListViewItem *lastItem = NULL;
    item = NULL;
    QValueList<QDBusObjectPath>::ConstIterator it2 = m_servicePaths.begin();
    for (; it2 != m_servicePaths.end(); ++it2) {
        QDBusProxy *proxy = new QDBusProxy(this);
        proxy->setService("net.connman");
        proxy->setPath((*it2));
        proxy->setInterface("net.connman.Service");
        proxy->setConnection(m_connection);
        
        QString name, type, state;
        QDBusMessage reply = proxy->sendWithReply("GetProperties", QValueList<QDBusData>());
        if (reply.type() == QDBusMessage::ReplyMessage && reply.count() == 1) {
            const QDBusDataMap<QString> map = reply[0].toStringKeyMap();
            QDBusDataMap<QString>::ConstIterator it = map.begin();
            for (; it != map.end(); ++it) {
                if( it.key() == "Name" ) {
                    name = it.data().toVariant().value.toString();
                }
                else if( it.key() == "Type" ) {
                    type = it.data().toVariant().value.toString();
                }
                else if( it.key() == "State" ) {
                    state = it.data().toVariant().value.toString();
                }
            }
        }

        if( !name.isEmpty() ) {
            item = new QListViewItem(serviceList, item);
            item->setPixmap( 0, stateIcons[state] );

            QPixmap pix;
            if( type == "ethernet" )
                pix = wiredPix;
            else if( type == "wifi" )
                pix = wirelessPix;

            item->setPixmap( 1, pix );
            item->setText(2, name);
            QString id = (*it2);
            item->setText(3, id);

            m_services.insert( id, proxy );
            QObject::connect(proxy, SIGNAL(asyncReply(int, const QDBusMessage&)),
                            this, SLOT(slotServiceAsyncReply(int, const QDBusMessage&)));

            if( !lastItemId.isEmpty() && lastItem == NULL && lastItemId == id )
                lastItem = item;
        }
    }

    if( lastItem )
        serviceList->ensureItemVisible(lastItem);
        serviceList->setCurrentItem(lastItem);
}

void MainWindowImpl::slotConnectService()
{
    QListViewItem *item = serviceList->currentItem();
    if(item) {
        QDBusProxy *proxy = m_services[item->text(3)];
        if( proxy ) {
            int callId = proxy->sendWithAsyncReply("Connect", QValueList<QDBusData>());
        }
    }
}

void MainWindowImpl::slotDisconnectService()
{
    QListViewItem *item = serviceList->currentItem();
    if(item) {
        QDBusProxy *proxy = m_services[item->text(3)];
        if( proxy ) {
            int callId = proxy->sendWithAsyncReply("Disconnect", QValueList<QDBusData>());
        }
    }
}

void MainWindowImpl::slotConfigureService()
{
    QListViewItem *item = serviceList->currentItem();
    if(item) {
        QDBusProxy *proxy = m_services[item->text(3)];
        if( proxy ) {
            ConnManServiceEditor *pe = new ConnManServiceEditor( proxy, this, "blar", WType_Modal | WStyle_NormalBorder );
            pe->showMaximized();
            pe->exec();
            if(pe->result()) {
                //lbServices->changeItem(pe->getServiceName(), lbServices->currentItem());
            }
            delete pe;
        }
    }
}

void MainWindowImpl::slotRemoveService()
{
    QListViewItem *item = serviceList->currentItem();
    if(item) {
        QDBusProxy *proxy = m_services[item->text(3)];
        if( proxy ) {
            int callId = proxy->sendWithAsyncReply("Remove", QValueList<QDBusData>());
        }
    }
}

void MainWindowImpl::slotScan()
{
    QValueList<QDBusData> parameters;
    parameters << QDBusData::fromString("");
    int callId = m_managerProxy->sendWithAsyncReply("RequestScan", parameters);
    m_calls[callId] = "RequestScan";
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
