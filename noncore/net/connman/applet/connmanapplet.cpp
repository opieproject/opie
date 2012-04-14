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
#include <qlabel.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>


#define AGENT_OBJECT_PATH        "/org/opie/connman/agent"


// QDialog subclass that allows us to handle authentication asynchronously
class AuthDialog: public QDialog {
public:
    AuthDialog( const QDBusMessage &authMsg, const QDBusDataMap<QString> &fields, 
                QWidget *parent=0, const char *name=0, bool modal=FALSE, WFlags f=0 )
        : QDialog( parent, name, modal, f ),
        m_authMsg( authMsg )
    {
        setCaption( tr( "Network Authentication" ) );

        QVBoxLayout *mainLayout = new QVBoxLayout( this ); 
        mainLayout->setSpacing( 0 );
        mainLayout->setMargin( 0 );

        QGridLayout *layout = new QGridLayout();
        layout->setSpacing( 6 );
        layout->setMargin( 4 );

        int i=0;
        QDict<QButtonGroup> groups;
        QDBusDataMap<QString>::ConstIterator it = fields.begin();
        for (; it != fields.end(); ++it) {
            QMap<QString,QDBusData> params = it.data().toVariant().value.toStringKeyMap().toQMap();
            if( params.contains("Alternates") ) {
                QButtonGroup *grp = new QButtonGroup(this);
                grp->hide();
                groups.insert( it.key(), grp );
                QStringList alternates = params["Alternates"].toVariant().value.toList().toStringList();
                for ( QStringList::Iterator it = alternates.begin(); it != alternates.end(); ++it )
                    groups.insert( (*it), grp );
            }

            QLineEdit *edit = new QLineEdit(this);
            m_controls.insert( it.key(), edit );
            QButtonGroup *grp = groups[it.key()];
            if( grp ) {
                QRadioButton *radio = new QRadioButton(this);
                radio->setText( it.key() );
                int id = grp->insert(radio);
                if( id == 0 )
                    grp->setButton(0);
                else
                    edit->setEnabled(false);
                layout->addWidget( radio, i, 0 );
                QObject::connect(radio, SIGNAL(toggled(bool)), edit, SLOT(setEnabled(bool)));
                QObject::connect(radio, SIGNAL(clicked()), edit, SLOT(setFocus()));
            }
            else {
                QLabel *label = new QLabel(this);
                label->setText( it.key() );
                layout->addWidget( label, i, 0 );
            }
            QObject::connect(edit, SIGNAL(returnPressed()), this, SLOT(accept()));
            layout->addWidget( edit, i, 1 );
            i++;
        }

        mainLayout->addLayout(layout);

        adjustSize();
        QWidget *desk = QApplication::desktop();
        move( desk->width()/2 - width()/2 ,
              desk->width()/2 - height()/2 );
    };

    void done( int ret ) {
        QDialog::done(ret);
        QDBusConnection connection = QDBusConnection::systemBus();
        if( ret == QDialog::Accepted ) {
            QDBusMessage reply = QDBusMessage::methodReply(m_authMsg);
            QMap<QString,QDBusData> map;
            for( QDictIterator<QLineEdit> it(m_controls); it.current(); ++it ) {
                if( it.current()->isEnabled() ) {
                    map.insert(it.currentKey(), QDBusData::fromVariant(QDBusVariant(QDBusData::fromString(it.current()->text()))));
                }
            }
            reply << QDBusData::fromStringKeyMap(map);
            connection.send(reply);
        }
        else {
            QDBusError error("net.connman.Agent.Error.Canceled", tr("User canceled authentication"));
            QDBusMessage reply = QDBusMessage::methodError(m_authMsg, error);
            connection.send(reply);
        }
    };
private:
    QDBusMessage m_authMsg;
    QDict<QLineEdit> m_controls;
};



ConnManApplet::ConnManApplet( QWidget *parent, const char *name ) : QWidget( parent, name )
{
    m_agentDlg = NULL;
    m_flight = false;
    m_strength = -1;

    setFixedHeight( AppLnk::smallIconSize() );
    setFixedWidth( AppLnk::smallIconSize() );

    m_brokenPix = OResource::loadImage( "connmanapplet/off", OResource::SmallIcon );
    m_flightPix = OResource::loadImage( "connmanapplet/flightmode", OResource::SmallIcon );
    m_offlinePix = OResource::loadImage( "connmanapplet/disconnected", OResource::SmallIcon );
    m_onlinePix = OResource::loadImage( "connmanapplet/connected", OResource::SmallIcon );
    m_strengthPix[0] = OResource::loadImage( "connmanapplet/signal_00", OResource::SmallIcon );
    m_strengthPix[25] = OResource::loadImage( "connmanapplet/signal_25", OResource::SmallIcon );
    m_strengthPix[50] = OResource::loadImage( "connmanapplet/signal_50", OResource::SmallIcon );
    m_strengthPix[75] = OResource::loadImage( "connmanapplet/signal_75", OResource::SmallIcon );
    m_strengthPix[100] = OResource::loadImage( "connmanapplet/signal_100", OResource::SmallIcon );

    m_services.setAutoDelete(TRUE);
    m_techs.setAutoDelete(TRUE);

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
    callId = m_managerProxy->sendWithAsyncReply("GetTechnologies", QValueList<QDBusData>());
    m_calls[callId] = "GetTechnologies";
    callId = m_managerProxy->sendWithAsyncReply("GetServices", QValueList<QDBusData>());
    m_calls[callId] = "GetServices";


    // Register agent
    m_connection.registerObject(AGENT_OBJECT_PATH, this);
    odebug << "Object registered for path " << AGENT_OBJECT_PATH << " on unique name " <<
           m_connection.uniqueName().local8Bit().data() << oendl;
    QValueList<QDBusData> parameters;
    parameters << QDBusData::fromObjectPath(QDBusObjectPath(AGENT_OBJECT_PATH));
    callId = m_managerProxy->sendWithAsyncReply("RegisterAgent", parameters);
    m_calls[callId] = "RegisterAgent";
}

ConnManApplet::~ConnManApplet()
{
    m_connection.unregisterObject(AGENT_OBJECT_PATH);
    delete m_managerProxy;
    delete m_agentDlg;
}

int ConnManApplet::position()
{
    return 6;
}

void ConnManApplet::mousePressEvent( QMouseEvent *)
{
    if( m_state == "" ) {
        QCopEnvelope e("QPE/TaskBar", "message(QString,QString)");
        e << tr("Connection manager unavailable");
        e << QString("connmanapplet");
        return;
    }

    QPopupMenu *menu = new QPopupMenu();

    int i=2;
    QStringList techs;
    for( QDictIterator<TechnologyListener> it(m_techs); it.current(); ++it ) {
        QString tech = it.current()->techName();
        // Bluetooth seems not to be enable-able and you shouldn't need to
        // control ethernet in this way (just unplug it!)
        if( it.current()->techType() == "bluetooth" || it.current()->techType() == "ethernet" )
            continue;

        if( it.current()->isPowered() )
            menu->insertItem( tr("Disable %1").arg( tech ), i );
        else
            menu->insertItem( tr("Enable %1").arg( tech ), i );
        i++;
        techs.append( it.currentKey() );
    }

    menu->insertSeparator();
    if( m_flight )
        menu->insertItem( tr("Normal mode"), 1 );
    else
        menu->insertItem( tr("Flight mode"), 1 );
    menu->insertSeparator();
    menu->insertItem( tr("Settings..."), 0 );

    QPoint p = mapToGlobal( QPoint(1, -menu->sizeHint().height()-1) );
    int ret = menu->exec(p, 0);
    menu->hide();

    if( ret == 0 ) {
        launchSettings();
    }
    else if( ret == 1 ) {
        toggleFlightMode();
    }
    else if( ret>1 ) {
        TechnologyListener *tech = m_techs[techs[ret-2]];
        if( tech )
            tech->setPowered(!tech->isPowered());
    }

    delete menu;
}

/**
 * Toggles offline (flight) mode
 */
void ConnManApplet::toggleFlightMode()
{
    QValueList<QDBusData> params;
    params << QDBusData::fromString("OfflineMode");
    params << QDBusData::fromVariant(QDBusVariant(QDBusData::fromBool(!m_flight)));
    m_managerProxy->sendWithAsyncReply("SetProperty", params);
}

/**
 * Launches the ConnMan manager
 */
void ConnManApplet::launchSettings()
{
    QCopEnvelope e("QPE/System", "execute(QString)");
    e << QString("connmansettings");
}


/**
 * Implementation of the paint event
 * @param the QPaintEvent
 */
void ConnManApplet::paintEvent( QPaintEvent* )
{
    QPainter p(this);
    if( m_flight )
        p.drawPixmap( 0, 0, m_flightPix );
    else if( m_state == "offline" )
        p.drawPixmap( 0, 0, m_offlinePix );
    else if( m_state == "online" || m_state == "connected" ) {
        if( m_strength > -1 )
            p.drawPixmap( 0, 0, m_strengthPix[m_strength] );
        else
            p.drawPixmap( 0, 0, m_onlinePix );
    }
    else
        p.drawPixmap( 0, 0, m_brokenPix );
}

void ConnManApplet::slotAsyncReply( int callId, const QDBusMessage& reply )
{
    QString method = m_calls[callId];
    if (reply.type() == QDBusMessage::ReplyMessage && reply.count() == 1) {
        if (method == "GetProperties") {
            const QDBusDataMap<QString> map = reply[0].toStringKeyMap();
            QDBusDataMap<QString>::ConstIterator it = map.begin();
            for (; it != map.end(); ++it) {
                managerPropertySet( it.key(), it.data().toVariant() );
            }
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
        else if (method == "GetServices") {
            m_services.clear();
            QValueList<QDBusData> services = reply[0].toList().toQValueList();
            addServices( services );
        }
    }
}

void ConnManApplet::slotDBusSignal(const QDBusMessage& message)
{
    if( message.member() == "PropertyChanged" ) {
        managerPropertySet( message[0].toString(), message[1].toVariant() );
    }
    else if( message.member() == "ServicesAdded" ) {
        addServices( message[0].toList().toQValueList() );
    }
    else if( message.member() == "ServicesRemoved" ) {
        removeServices( message[0].toList().toObjectPathList() );
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

void ConnManApplet::managerPropertySet( const QString &prop, const QDBusVariant &propval )
{
    if( prop == "State" ) {
        QString state = propval.value.toString();
        if( m_state != state ) {
            m_state = state;
            update();
        }
    }
    else if( prop == "OfflineMode" ) {
        bool flight = propval.value.toBool();
        if( m_flight != flight ) {
            m_flight = flight;
            update();
        }
    }
}

void ConnManApplet::serviceStateChanged( const QDBusObjectPath &path, const QString &oldstate, const QString &newstate )
{
    QString msg = "";
    QString name = m_services[path]->serviceName();
    if( newstate == "ready" || ( newstate == "online" && oldstate != "ready" ) ) {
        msg = tr("Connected to %1").arg(name);
    }

    if( !msg.isEmpty() ) {
        QCopEnvelope e("QPE/TaskBar", "message(QString,QString)");
        e << msg;
        e << QString("connmanapplet");
    }
}

void ConnManApplet::addServices( const QValueList<QDBusData> &services )
{
    for( QValueList<QDBusData>::ConstIterator it = services.begin(); it != services.end(); ++it ) {
        QValueList<QDBusData> memberList;
        memberList = (*it).toStruct();
        ServiceListener *listener = new ServiceListener( memberList[0].toObjectPath(), 
                                                            memberList[1].toStringKeyMap() );
        QObject::connect(listener, SIGNAL(serviceStateChanged(const QDBusObjectPath&, const QString&, const QString&)),
                        this, SLOT(serviceStateChanged(const QDBusObjectPath&, const QString&, const QString&)));
        QObject::connect(listener, SIGNAL(signalStrength(int)),
                        this, SLOT(signalStrength(int)));
        m_services.insert(memberList[0].toObjectPath(), listener);
    }
}

void ConnManApplet::removeServices( const QValueList<QDBusObjectPath> &services )
{
    for( QValueList<QDBusObjectPath>::ConstIterator it = services.begin(); it != services.end(); ++it ) {
        m_services.remove( QString(*it) );
    }
}

void ConnManApplet::signalStrength( int strength )
{
    odebug << "signalStrength( " << strength << " )" << oendl;
    int normStrength = 0;
    // These values shamelessly borrowed from nm-applet code
    if( strength > 80 )
        normStrength = 100;
    else if( strength > 55 )
        normStrength = 75;
    else if( strength > 30 )
        normStrength = 50;
    else if( strength > 5 )
        normStrength = 25;
    else if( strength == -1 )
        normStrength = -1;

    if( normStrength != m_strength ) {
        m_strength = normStrength;
        update();
    }
}

bool ConnManApplet::handleMethodCall(const QDBusMessage& message)
{
    odebug << "ConnManApplet::handleMethodCall: interface='" << message.interface().latin1() << "', member='" << message.member().latin1() << "'" << oendl;

    if (message.interface() != "net.connman.Agent")
        return false;

    if (message.member() == "ReportError") {
        destroyDialog();
        QMessageBox::warning(this, tr("Network Error"), message[1].toString());
        return true;
    }
    else if (message.member() == "RequestInput") {
        showDialog( message, message[1].toStringKeyMap() );
        return true;
    }
    else if (message.member() == "Cancel") {
        destroyDialog();
        QMessageBox::message(tr("Network"),tr("Authentication was cancelled"));
        return true;
    }

    return false;
}

void ConnManApplet::showDialog( const QDBusMessage& message, const QDBusDataMap<QString> &fields )
{
    destroyDialog();
    m_agentDlg = new AuthDialog(message, fields, this);
    m_agentDlg->show();
}

void ConnManApplet::destroyDialog()
{
    if( m_agentDlg ) {
        delete m_agentDlg;
        m_agentDlg = NULL;
    }
}


EXPORT_OPIE_APPLET_v1( ConnManApplet )

