/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
***********************************************************************/

// Local

#include "wellenreiter.h"
#include "scanlist.h"
#include "logwindow.h"
#include "hexwindow.h"
#include "configwindow.h"
#include "statwindow.h"
#include "graphwindow.h"
#include "protolistview.h"

// Opie

#ifdef QWS
#include <opie/odevice.h>
#include <qpe/qcopenvelope_qws.h>
using namespace Opie;
#endif

#ifdef QWS
#include <opie2/oapplication.h>
#else
#include <qapplication.h>
#endif
#include <opie2/onetwork.h>
#include <opie2/opcap.h>

// Qt

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdatetime.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qobjectlist.h>
#include <qregexp.h>
#include <qspinbox.h>
#include <qtoolbutton.h>
#include <qmainwindow.h>

// Standard

#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>

Wellenreiter::Wellenreiter( QWidget* parent )
    : WellenreiterBase( parent, 0, 0 ),
      sniffing( false ), iface( 0 ), configwindow( 0 )
{

    logwindow->log( "(i) Wellenreiter has been started." );

    //
    // detect operating system
    //

    #ifdef QWS
    QString sys;
    sys.sprintf( "(i) Running on '%s'.", (const char*) ODevice::inst()->systemString() );
    _system = ODevice::inst()->system();
    logwindow->log( sys );
    #endif

    netview->setColumnWidthMode( 1, QListView::Manual );
    connect( netview, SIGNAL( joinNetwork(const QString&,const QString&,int,const QString&) ),
             this, SLOT( joinNetwork(const QString&,const QString&,int,const QString&) ) );
    pcap = new OPacketCapturer();
}


Wellenreiter::~Wellenreiter()
{
    delete pcap;
}


void Wellenreiter::setConfigWindow( WellenreiterConfigWindow* cw )
{
    configwindow = cw;
}


void Wellenreiter::channelHopped(int c)
{
    QString title = "Wellenreiter II -scan- [";
    QString left;
    if ( c > 1 ) left.fill( '.', c-1 );
    title.append( left );
    title.append( '|' );
    if ( c < iface->channels() )
    {
        QString right;
        right.fill( '.', iface->channels()-c );
        title.append( right );
    }
    title.append( "]" );
    //title.append( QString().sprintf( " %02d", c ) );
    assert( parent() );
    ( (QMainWindow*) parent() )->setCaption( title );
}


void Wellenreiter::handleNotification( OPacket* p )
{
    QObjectList* l = p->queryList();
    QObjectListIt it( *l );
    QObject* o;

    while ( (o = it.current()) != 0 )
    {
        QString name = it.current()->name();
        if ( configwindow->parsePackets->isProtocolChecked( name ) )
        {
            QString action = configwindow->parsePackets->protocolAction( name );
            qDebug( "parsePacket-action for '%s' seems to be '%s'", (const char*) name, (const char*) action );
            doAction( action, name, p );
        }
        else
        {
            qDebug( "protocol '%s' not checked in parsePackets.", (const char*) name );
        }
    ++it;
    }
}


void Wellenreiter::handleBeacon( OPacket* p, OWaveLanManagementPacket* beacon )
{
    QString type;
    if ( beacon->canIBSS() )
    {
        type = "adhoc";
    }
    else if ( beacon->canESS() )
    {
        type = "managed";
    }
    else
    {
        qWarning( "Wellenreiter::invalid frame [possibly noise] detected!" );
        return;
    }

    OWaveLanManagementSSID* ssid = static_cast<OWaveLanManagementSSID*>( p->child( "802.11 SSID" ) );
    QString essid = ssid ? ssid->ID() : QString("<unknown>");
    OWaveLanManagementDS* ds = static_cast<OWaveLanManagementDS*>( p->child( "802.11 DS" ) );
    int channel = ds ? ds->channel() : -1;

    OWaveLanPacket* header = static_cast<OWaveLanPacket*>( p->child( "802.11" ) );
    netView()->addNewItem( type, essid, header->macAddress2(), beacon->canPrivacy(), channel, 0 );

    // update graph window
    if ( ds )
    {
        OPrismHeaderPacket* prism = static_cast<OPrismHeaderPacket*>( p->child( "Prism" ) );
        if ( prism )
            graphwindow->traffic( ds->channel(), prism->signalStrength() );
        else
            graphwindow->traffic( ds->channel(), 95 );
    }
}


void Wellenreiter::handleData( OPacket* p, OWaveLanDataPacket* data )
{
    OWaveLanPacket* wlan = (OWaveLanPacket*) p->child( "802.11" );
    if ( wlan->fromDS() && !wlan->toDS() )
    {
        netView()->fromDStraffic( wlan->macAddress3(), wlan->macAddress1(), wlan->macAddress2() );
    }
    else if ( !wlan->fromDS() && wlan->toDS() )
    {
        netView()->toDStraffic( wlan->macAddress2(), wlan->macAddress3(), wlan->macAddress1() );
    }
    else if ( wlan->fromDS() && wlan->toDS() )
    {
        netView()->WDStraffic( wlan->macAddress4(), wlan->macAddress3(), wlan->macAddress1(), wlan->macAddress2() );
    }
    else
    {
        netView()->IBSStraffic( wlan->macAddress2(), wlan->macAddress1(), wlan->macAddress3() );
    }

    OARPPacket* arp = (OARPPacket*) p->child( "ARP" );
    if ( arp )
    {
        qDebug( "Received ARP traffic (type '%s'): ", (const char*) arp->type() );
        if ( arp->type() == "REQUEST" )
        {
            netView()->identify( arp->senderMacAddress(), arp->senderIPV4Address().toString() );
        }
        else if ( arp->type() == "REPLY" )
        {
            netView()->identify( arp->senderMacAddress(), arp->senderIPV4Address().toString() );
            netView()->identify( arp->targetMacAddress(), arp->targetIPV4Address().toString() );
        }
    }

    OIPPacket* ip = (OIPPacket*) p->child( "IP" );
    if ( ip )
    {
        qDebug( "Received IP packet." );
    }
}


QObject* Wellenreiter::childIfToParse( OPacket* p, const QString& protocol )
{
    if ( configwindow->parsePackets->isProtocolChecked( protocol ) )
        if ( configwindow->parsePackets->protocolAction( protocol ) == "Discard!" )
            return 0;

    return p->child( protocol );
}


bool Wellenreiter::checkDumpPacket( OPacket* p )
{
    // go through all child packets and see if one is inside the child hierarchy for p
    // if so, do what the user requested (protocolAction), e.g. pass or discard
    if ( !configwindow->writeCaptureFile->isOn() )
        return false;

    QObjectList* l = p->queryList();
    QObjectListIt it( *l );
    QObject* o;

    while ( (o = it.current()) != 0 )
    {
        QString name = it.current()->name();
        if ( configwindow->capturePackets->isProtocolChecked( name ) )
        {
            QString action = configwindow->capturePackets->protocolAction( name );
            qDebug( "capturePackets-action for '%s' seems to be '%s'", (const char*) name, (const char*) action );
            if ( action == "Discard" )
            {
                logwindow->log( QString().sprintf( "(i) dump-discarding of '%s' packet requested.", (const char*) name ) );
                return false;
            }
        }
        else
        {
            qDebug( "protocol '%s' not checked in capturePackets.", (const char*) name );
        }
    ++it;
    }
    return true;
}


void Wellenreiter::receivePacket( OPacket* p )
{
    hexWindow()->log( p->dump( 8 ) );

    if ( checkDumpPacket( p ) )
    {
        pcap->dump( p );
    }

    // check if we received a beacon frame
    OWaveLanManagementPacket* beacon = static_cast<OWaveLanManagementPacket*>( childIfToParse( p, "802.11 Management" ) );
    if ( beacon && beacon->managementType() == "Beacon" )
    {
        handleBeacon( p, beacon );
        return;
    }

    //TODO: WEP check here

    // check for a data frame
    OWaveLanDataPacket* data = static_cast<OWaveLanDataPacket*>( childIfToParse( p, "802.11 Data" ) );
    if ( data )
    {
        handleData( p, data );
    }

    handleNotification( p );

}


void Wellenreiter::stopClicked()
{
    if ( iface )
    {
        disconnect( SIGNAL( receivedPacket(OPacket*) ), this, SLOT( receivePacket(OPacket*) ) );
        disconnect( SIGNAL( hopped(int) ), this, SLOT( channelHopped(int) ) );
        iface->setChannelHopping(); // stop hopping channels
    }
    else
        killTimers();

    pcap->close();
    sniffing = false;

    if ( iface )
    {
        // switch off monitor mode
        iface->setMonitorMode( false );
        // switch off promisc flag
        iface->setPromiscuousMode( false );

        system( "cardctl reset; sleep 1" ); //FIXME: Use OProcess
    }

    logwindow->log( "(i) Stopped Scanning." );
    assert( parent() );
    ( (QMainWindow*) parent() )->setCaption( "Wellenreiter II" );

    // message the user
    QMessageBox::information( this, "Wellenreiter II",
                              tr( "Your wireless card\nshould now be usable again." ) );

    sniffing = false;
    emit( stoppedSniffing() );

    // print out statistics
    for( QMap<QString,int>::ConstIterator it = pcap->statistics().begin(); it != pcap->statistics().end(); ++it )
       statwindow->updateCounter( it.key(), it.data() );
}


void Wellenreiter::startClicked()
{
    // get configuration from config window

    const QString& interface = configwindow->interfaceName->currentText();
    const int cardtype = configwindow->driverType();
    const int interval = configwindow->hoppingInterval();

    if ( ( interface == "" ) || ( cardtype == 0 ) )
    {
        QMessageBox::information( this, "Wellenreiter II",
                                  tr( "Your device is not\nproperly configured. Please reconfigure!" ) );
        return;
    }

    // configure device
    ONetwork* net = ONetwork::instance();
    iface = static_cast<OWirelessNetworkInterface*>(net->interface( interface ));

    // bring device UP
    iface->setUp( true );
    if ( !iface->isUp() )
    {
        QMessageBox::warning( this, "Wellenreiter II",
                            tr( "Can't bring interface '%1' up:\n" ).arg( iface->name() ) + strerror( errno ) );
        return;
    }

    // set monitor mode
    bool usePrism = configwindow->usePrismHeader();

    switch ( cardtype )
    {
        case DEVTYPE_CISCO: iface->setMonitoring( new OCiscoMonitoringInterface( iface, usePrism ) ); break;
        case DEVTYPE_WLAN_NG: iface->setMonitoring( new OWlanNGMonitoringInterface( iface, usePrism ) ); break;
        case DEVTYPE_HOSTAP: iface->setMonitoring( new OHostAPMonitoringInterface( iface, usePrism ) ); break;
        case DEVTYPE_ORINOCO: iface->setMonitoring( new OOrinocoMonitoringInterface( iface, usePrism ) ); break;
        case DEVTYPE_MANUAL: QMessageBox::information( this, "Wellenreiter II", tr( "Bring your device into\nmonitor mode now." ) ); break;
        case DEVTYPE_FILE: qDebug( "Wellenreiter: Capturing from file '%s'", (const char*) interface ); break;
        default: assert( 0 ); // shouldn't reach this
    }

    // switch device into monitor mode
    if ( cardtype < DEVTYPE_FILE )
    {
        if ( cardtype != DEVTYPE_MANUAL )
            iface->setMonitorMode( true );
        if ( !iface->monitorMode() )
        {
            QMessageBox::warning( this, "Wellenreiter II",
                                  tr( "Can't set interface '%1'\ninto monitor mode:\n" ).arg( iface->name() ) + strerror( errno ) );
            return;
        }
    }

    // open pcap and start sniffing
    if ( cardtype != DEVTYPE_FILE )
    {
        pcap->open( interface );

        if ( configwindow->writeCaptureFile->isOn() )
        {
            QString dumpname( configwindow->captureFileName->text() );
            dumpname.append( '-' );
            dumpname.append( QTime::currentTime().toString().replace( QRegExp( ":" ), "-" ) );
            dumpname.append( ".wellenreiter" );
            pcap->openDumpFile( dumpname );
        }
        else
        {
            pcap->open( interface );
        }
    }
    else
    {
        pcap->open( QFile( interface ) );
    }

    if ( !pcap->isOpen() )
    {
        QMessageBox::warning( this, "Wellenreiter II",
                              tr( "Can't open packet capturer for '%1':\n" ).arg( iface->name() ) + QString(strerror( errno ) ));
        return;
    }

    // set capturer to non-blocking mode
    pcap->setBlocking( false );

    // start channel hopper
    if ( cardtype != DEVTYPE_FILE )
        iface->setChannelHopping( 1000 ); //use interval from config window

    if ( cardtype != DEVTYPE_FILE )
    {
        // connect socket notifier and start channel hopper
        connect( pcap, SIGNAL( receivedPacket(OPacket*) ), this, SLOT( receivePacket(OPacket*) ) );
        connect( iface->channelHopper(), SIGNAL( hopped(int) ), this, SLOT( channelHopped(int) ) );
    }
    else
    {
        // start timer for reading packets
        startTimer( 100 );
    }

    logwindow->log( "(i) Started Scanning." );
    sniffing = true;
    emit( startedSniffing() );
    if ( cardtype != DEVTYPE_FILE ) channelHopped( 6 ); // set title
    else
    {
        assert( parent() );
        ( (QMainWindow*) parent() )->setCaption( tr( "Wellenreiter II - replaying capture file..." ) );
    }
}


void Wellenreiter::timerEvent( QTimerEvent* )
{
    qDebug( "Wellenreiter::timerEvent()" );
    OPacket* p = pcap->next();
    if ( !p ) // no more packets available
    {
        stopClicked();
    }
    else
    {
        receivePacket( p );
        delete p;
    }
}


void Wellenreiter::doAction( const QString& action, const QString& protocol, OPacket* p )
{
    if ( action == "TouchSound" )
        ODevice::inst()->touchSound();
    else if ( action == "AlarmSound" )
        ODevice::inst()->alarmSound();
    else if ( action == "KeySound" )
        ODevice::inst()->keySound();
    else if ( action == "LedOn" )
        ODevice::inst()->setLedState( Led_Mail, Led_On );
    else if ( action == "LedOff" )
        ODevice::inst()->setLedState( Led_Mail, Led_Off );
    else if ( action == "LogMessage" )
        logwindow->log( QString().sprintf( "Got packet with protocol '%s'", (const char*) protocol ) );
    else if ( action == "MessageBox" )
        QMessageBox::information ( this, "Notification!",
        QString().sprintf( "Got packet with protocol '%s'", (const char*) protocol ) );
}

void Wellenreiter::joinNetwork(const QString& type, const QString& essid, int channel, const QString& macaddr)
{
    qDebug( "joinNetwork() - %s, %s, %d, %s",
        (const char*) type,
        (const char*) essid,
        channel,
        (const char*) macaddr );

    // TODO: Stop scanning here

    QCopEnvelope msg( "QPE/Application/networksettings", "wlan(QString,QString,QString)" );
    msg << "test" << "test" << "test";

}

