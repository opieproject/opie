/**********************************************************************
** Copyright (C) 2002-2004 Michael 'Mickey' Lauer.  All rights reserved.
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

#include "gps.h"
#include "wellenreiter.h"
#include "scanlist.h"
#include "logwindow.h"
#include "packetview.h"
#include "configwindow.h"
#include "statwindow.h"
#include "graphwindow.h"
#include "protolistview.h"

/* OPIE */
#ifdef QWS
#include <opie2/oapplication.h>
#include <opie2/odebug.h>
#include <opie2/odevice.h>
#else
#include <qapplication.h>
#endif
#include <opie2/omanufacturerdb.h>
#include <opie2/onetwork.h>
#include <opie2/opcap.h>
#include <qpe/qcopenvelope_qws.h>
using namespace Opie::Core;
using namespace Opie::Net;
using namespace Opie::Ui;

/* QT */
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdatetime.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qobjectlist.h>
#include <qregexp.h>
#include <qspinbox.h>
#include <qtimer.h>
#include <qtoolbutton.h>
#include <qmainwindow.h>

/* STD */
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
    pcap->setAutoDelete( false );

    gps = new GPS( this );

    QTimer::singleShot( 1000, this, SLOT( initialTimer() ) );

}


Wellenreiter::~Wellenreiter()
{
    delete pcap;
}


void Wellenreiter::initialTimer()
{
    odebug << "Wellenreiter::preloading manufacturer database..." << oendl; 
    OManufacturerDB::instance();
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
            odebug << "parsePacket-action for '" << (const char*) name << "' seems to be '" << (const char*) action << "'" << oendl; 
            doAction( action, name, p );
        }
        else
        {
            odebug << "protocol '" << (const char*) name << "' not checked in parsePackets." << oendl; 
        }
    ++it;
    }
}


void Wellenreiter::handleManagementFrame( OPacket* p, OWaveLanManagementPacket* manage )
{
    if ( manage->managementType() == "Beacon" ) handleManagementFrameBeacon( p, manage );
    else if ( manage->managementType() == "ProbeRequest" ) handleManagementFrameProbeRequest( p, manage );
    else if ( manage->managementType() == "ProbeResponse" ) handleManagementFrameProbeResponse( p, manage );
    else owarn << "Wellenreiter::handleManagementFrame(): '" << (const char*) manage->managementType() << "' - please handle me!" << oendl; 
}


void Wellenreiter::handleManagementFrameProbeRequest( OPacket* p, OWaveLanManagementPacket* request )
{
    OWaveLanManagementSSID* ssid = static_cast<OWaveLanManagementSSID*>( p->child( "802.11 SSID" ) );
    QString essid = ssid ? ssid->ID( true /* decloak */ ) : QString("<unknown>");
    OWaveLanManagementDS* ds = static_cast<OWaveLanManagementDS*>( p->child( "802.11 DS" ) );
    int channel = ds ? ds->channel() : -1;
    OWaveLanPacket* header = static_cast<OWaveLanPacket*>( p->child( "802.11" ) );

    GpsLocation loc( -111, -111 );
    if ( configwindow->enableGPS->isChecked() )
    {
        // TODO: add check if GPS is working!?
        odebug << "Wellenreiter::gathering GPS data..." << oendl; 
        loc = gps->position();
        odebug << "Wellenreiter::GPS data received is ( " << loc.latitude() << " , " << loc.longitude() << " ) - dms string = '" << loc.dmsPosition().latin1() << "'" << oendl; 
    }

    if ( essid.length() )
        netView()->addNewItem( "adhoc", essid, header->macAddress2(), false /* should check FrameControl field */, -1, 0, loc, true /* only probed */ );
    odebug << "Wellenreiter::invalid frame [possibly noise] detected!" << oendl; 
}


void Wellenreiter::handleManagementFrameProbeResponse( OPacket* p, OWaveLanManagementPacket* response )
{
}


void Wellenreiter::handleManagementFrameBeacon( OPacket* p, OWaveLanManagementPacket* beacon )
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
        owarn << "Wellenreiter::invalid frame [possibly noise] detected!" << oendl; 
        return;
    }

    OWaveLanManagementSSID* ssid = static_cast<OWaveLanManagementSSID*>( p->child( "802.11 SSID" ) );
    QString essid = ssid ? ssid->ID( true /* decloak */ ) : QString("<unknown>");
    OWaveLanManagementDS* ds = static_cast<OWaveLanManagementDS*>( p->child( "802.11 DS" ) );
    int channel = ds ? ds->channel() : -1;

    OWaveLanPacket* header = static_cast<OWaveLanPacket*>( p->child( "802.11" ) );

    GpsLocation loc( -111, -111 );
    if ( configwindow->enableGPS->isChecked() )
    {
        // TODO: add check if GPS is working!?
        odebug << "Wellenreiter::gathering GPS data..." << oendl; 
        loc = gps->position();
        odebug << "Wellenreiter::GPS data received is ( " << loc.latitude() << " , " << loc.longitude() << " ) - dms string = '" << loc.dmsPosition().latin1() << "'" << oendl; 
    }

    netView()->addNewItem( type, essid, header->macAddress2(), beacon->canPrivacy(), channel, 0, loc );

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


void Wellenreiter::handleControlFrame( OPacket* p, OWaveLanControlPacket* control )
{
    OWaveLanPacket* header = static_cast<OWaveLanPacket*>( p->child( "802.11" ) );

    if ( control->controlType() == "Acknowledge" )
    {
        netView()->addNewItem( "adhoc", "<unknown>", header->macAddress1(), false, -1, 0, GpsLocation( -111, -111 ) );
    }
    else
    {
        odebug << "Wellenreiter::handleControlFrame - please handle " << (const char*) control->controlType() << " in a future version! :D" << oendl; 
    }
}


void Wellenreiter::handleWlanData( OPacket* p, OWaveLanDataPacket* data, OMacAddress& from, OMacAddress& to )
{
    OWaveLanPacket* wlan = (OWaveLanPacket*) p->child( "802.11" );
    if ( wlan->fromDS() && !wlan->toDS() )
    {
        netView()->fromDStraffic( wlan->macAddress3(), wlan->macAddress1(), wlan->macAddress2() );
        from = wlan->macAddress3();
        to = wlan->macAddress2();
    }
    else if ( !wlan->fromDS() && wlan->toDS() )
    {
        netView()->toDStraffic( wlan->macAddress2(), wlan->macAddress3(), wlan->macAddress1() );
        from = wlan->macAddress2();
        to = wlan->macAddress3();
    }
    else if ( wlan->fromDS() && wlan->toDS() )
    {
        netView()->WDStraffic( wlan->macAddress4(), wlan->macAddress3(), wlan->macAddress1(), wlan->macAddress2() );
        from = wlan->macAddress4();
        to = wlan->macAddress3();
    }
    else
    {
        netView()->IBSStraffic( wlan->macAddress2(), wlan->macAddress1(), wlan->macAddress3() );
        from = wlan->macAddress2();
        to = wlan->macAddress1();
    }
}


void Wellenreiter::handleEthernetData( OPacket* p, OEthernetPacket* data, OMacAddress& from, OMacAddress& to )
{
    from = data->sourceAddress();
    to = data->destinationAddress();

    netView()->addNewItem( "station", "<wired>", from, false, -1, 0, GpsLocation( -111, -111 ) );
}


void Wellenreiter::handleARPData( OPacket* p, OARPPacket*, OMacAddress& source, OMacAddress& dest )
{
    OARPPacket* arp = (OARPPacket*) p->child( "ARP" );
    if ( arp )
    {
        odebug << "Received ARP traffic (type '" << (const char*) arp->type() << "'): " << oendl; 
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
}


void Wellenreiter::handleIPData( OPacket* p, OIPPacket* ip, OMacAddress& source, OMacAddress& dest )
{
    //TODO: Implement more IP based protocols

    ODHCPPacket* dhcp = (ODHCPPacket*) p->child( "DHCP" );
    if ( dhcp )
    {
        odebug << "Received DHCP '" << (const char*) dhcp->type() << "' packet" << oendl; 
        if ( dhcp->type() == "OFFER" )
        {
            odebug << "DHCP: '" << (const char*) source.toString() << "' ('" << (const char*) dhcp->serverAddress().toString() << "') seems to be a DHCP server." << oendl; 
            netView()->identify( source, dhcp->serverAddress().toString() );
            netView()->addService( "DHCP", source, dhcp->serverAddress().toString() );
        }
        else if ( dhcp->type() == "ACK" )
        {
            odebug << "DHCP: '" << (const char*) dhcp->clientMacAddress().toString() << "' ('" << (const char*) dhcp->yourAddress().toString() << "') accepted the offered DHCP address." << oendl; 
            netView()->identify( dhcp->clientMacAddress(), dhcp->yourAddress().toString() );
        }
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
    if ( !configwindow->writeCaptureFile->isChecked() )
        return true; // semantic change - we're logging anyway now to /tmp/wellenreiter

    QObjectList* l = p->queryList();
    QObjectListIt it( *l );
    QObject* o;

    while ( (o = it.current()) != 0 )
    {
        QString name = it.current()->name();
        if ( configwindow->capturePackets->isProtocolChecked( name ) )
        {
            QString action = configwindow->capturePackets->protocolAction( name );
            odebug << "capturePackets-action for '" << (const char*) name << "' seems to be '" << (const char*) action << "'" << oendl; 
            if ( action == "Discard" )
            {
                logwindow->log( QString().sprintf( "(i) dump-discarding of '%s' packet requested.", (const char*) name ) );
                return false;
            }
        }
        else
        {
            odebug << "protocol '" << (const char*) name << "' not checked in capturePackets." << oendl; 
        }
    ++it;
    }
    return true;
}


void Wellenreiter::receivePacket( OPacket* p )
{
    hexWindow()->add( p );

    if ( checkDumpPacket( p ) )
    {
        pcap->dump( p );
    }

    // check for a management frame
    OWaveLanManagementPacket* manage = static_cast<OWaveLanManagementPacket*>( childIfToParse( p, "802.11 Management" ) );
    if ( manage )
    {
        handleManagementFrame( p, manage );
        return;
    }

    // check for a control frame
    OWaveLanControlPacket* control = static_cast<OWaveLanControlPacket*>( childIfToParse( p, "802.11 Control" ) );
    if ( control )
    {
        handleControlFrame( p, control );
        return;
    }

    OMacAddress source;
    OMacAddress dest;

    //TODO: WEP check here

    // check for a wireless data frame
    OWaveLanDataPacket* wlan = static_cast<OWaveLanDataPacket*>( childIfToParse( p, "802.11 Data" ) );
    if ( wlan )
    {
        handleWlanData( p, wlan, source, dest );
    }

    // check for a wired data frame
    OEthernetPacket* eth = static_cast<OEthernetPacket*>( childIfToParse( p, "Ethernet" ) );
    if ( eth )
    {
        handleEthernetData( p, eth, source, dest );
    }

    // check for an arp frame since arp frames come in two flavours:
    // 802.11 encapsulates ARP data within IP packets while wired ethernet doesn't.
    OARPPacket* arp = static_cast<OARPPacket*>( childIfToParse( p, "ARP" ) );
    if ( arp )
    {
        handleARPData( p, arp, source, dest );
    }

    // check for a ip frame
    OIPPacket* ip = static_cast<OIPPacket*>( childIfToParse( p, "IP" ) );
    if ( ip )
    {
        handleIPData( p, ip, source, dest );
    }

    //handleNotification( p );

}


void Wellenreiter::stopClicked()
{
    if ( iface )
    {
        disconnect( SIGNAL( receivedPacket(Opie::Net::OPacket*) ), this, SLOT( receivePacket(Opie::Net::OPacket*) ) );
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
        iface->setMode( "managed" );
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

    #ifdef QWS
    if ( WellenreiterConfigWindow::instance()->disablePM->isChecked() )
    {
        QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Enable;
    }
    #else
        #warning FIXME: setScreenSaverMode is not operational on the X11 build
    #endif

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

    // TODO: check if interface is wireless and support sniffing for non-wireless interfaces

    if ( cardtype != DEVTYPE_FILE )
    {
    
        if ( !net->isPresent( interface ) )
        {
            QMessageBox::information( this, "Wellenreiter II",
                                    tr( "The configured device (%1)\nis not available on this system\n. Please reconfigure!" ).arg( interface ) );
            return;
        }
    
        iface = static_cast<OWirelessNetworkInterface*>(net->interface( interface )); // fails if network is not wireless!
        assert( iface );

        // bring device UP
        iface->setUp( true );
        if ( !iface->isUp() )
        {
            QMessageBox::warning( this, "Wellenreiter II",
                                tr( "Can't bring interface '%1' up:\n" ).arg( iface->name() ) + strerror( errno ) );
            return;
        }
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
        case DEVTYPE_FILE: odebug << "Wellenreiter: Capturing from file '" << (const char*) interface << "'" << oendl;  break;
        default: assert( 0 ); // shouldn't reach this
    }

    // switch device into monitor mode
    if ( cardtype < DEVTYPE_FILE )
    {
        if ( cardtype != DEVTYPE_MANUAL )
            iface->setMode( "monitor" );
        if ( iface->mode() != "monitor" )
        {
            if ( QMessageBox::warning( this, "Wellenreiter II",
                                  tr( "Can't set interface '%1'\ninto monitor mode:\n" ).arg( iface->name() ) + strerror( errno ) +
                                  tr( "\nContinue with limited functionality?" ), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::No )
                return;
        }
    }

    // open GPS device
    if ( configwindow->enableGPS->isChecked() )
    {
        odebug << "Wellenreiter:GPS enabled @ " << (const char*) configwindow->gpsdHost->currentText() << ":" << configwindow->gpsdPort->value() << "" << oendl; 
        gps->open( configwindow->gpsdHost->currentText(), configwindow->gpsdPort->value() );
    }

    // open pcap and start sniffing

    if ( configwindow->writeCaptureFile->isChecked() ) // write to a user specified capture file?
    {
        dumpname = configwindow->captureFileName->text();
        if ( dumpname.isEmpty() ) dumpname = "captureFile";
        dumpname.append( '-' );
        dumpname.append( QTime::currentTime().toString().replace( QRegExp( ":" ), "-" ) );
        dumpname.append( ".wellenreiter" );
    }
    else // write it anyway ;)
    {
        dumpname = "/var/log/dump.wellenreiter";
    }

    if ( cardtype != DEVTYPE_FILE )
        pcap->open( interface );
    else
        pcap->open( QFile( interface ) );

    odebug << "Wellenreiter:: dumping to " << (const char*) dumpname << "" << oendl; 
    pcap->openDumpFile( dumpname );

    if ( !pcap->isOpen() )
    {
        QMessageBox::warning( this, "Wellenreiter II", tr( "Can't open packet capturer for\n'%1':\n" ).arg(
                              cardtype == DEVTYPE_FILE ? (const char*) interface : iface->name() ) + QString(strerror( errno ) ));
        return;
    }

    // set capturer to non-blocking mode
    pcap->setBlocking( false );

    // start channel hopper
    if ( cardtype != DEVTYPE_FILE )
    {
        logwindow->log( QString().sprintf( "(i) Starting channel hopper (d=%d ms)", configwindow->hopInterval->value() ) );
        iface->setChannelHopping( configwindow->hopInterval->value() ); //use interval from config window
    }

    if ( cardtype != DEVTYPE_FILE )
    {
        // connect socket notifier and start channel hopper
        connect( pcap, SIGNAL( receivedPacket(Opie::Net::OPacket*) ), this, SLOT( receivePacket(Opie::Net::OPacket*) ) );
        connect( iface->channelHopper(), SIGNAL( hopped(int) ), this, SLOT( channelHopped(int) ) );
    }
    else
    {
        // start timer for reading packets
        startTimer( 100 );
    }

    logwindow->log( "(i) Started Scanning." );
    sniffing = true;

    #ifdef QWS
    if ( WellenreiterConfigWindow::instance()->disablePM->isChecked() )
    {
        QCopEnvelope( "QPE/System", "setScreenSaverMode(int)" ) << QPEApplication::Disable;
    }
    #else
        #warning FIXME: setScreenSaverMode is not operational on the X11 build
    #endif

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
    odebug << "Wellenreiter::timerEvent()" << oendl; 
    OPacket* p = pcap->next();
    if ( !p ) // no more packets available
    {
        stopClicked();
    }
    else
    {
        receivePacket( p );
        // We no longer delete packets here. Ownership of the packets is
        // transferred to the PacketView.
        //delete p;
    }
}


void Wellenreiter::doAction( const QString& action, const QString& protocol, OPacket* p )
{
    #ifdef QWS
    if ( action == "TouchSound" )
        ODevice::inst()->playTouchSound();
    else if ( action == "AlarmSound" )
        ODevice::inst()->playAlarmSound();
    else if ( action == "KeySound" )
        ODevice::inst()->playKeySound();
    else if ( action == "LedOn" )
        ODevice::inst()->setLedState( Led_Mail, Led_On );
    else if ( action == "LedOff" )
        ODevice::inst()->setLedState( Led_Mail, Led_Off );
    else if ( action == "LogMessage" )
        logwindow->log( QString().sprintf( "Got packet with protocol '%s'", (const char*) protocol ) );
    else if ( action == "MessageBox" )
        QMessageBox::information( this, "Notification!",
        QString().sprintf( "Got packet with protocol '%s'", (const char*) protocol ) );
    #else
    #warning Actions do not work with Qt/X11 yet
    #endif
}

void Wellenreiter::joinNetwork(const QString& type, const QString& essid, int channel, const QString& macaddr)
{
    #ifdef QWS
    if ( !iface )
    {
        QMessageBox::warning( this, tr( "Can't do that!" ), tr( "No wireless\ninterface available." ) );
        return;
    }

    if ( sniffing )
    {
        QMessageBox::warning( this, tr( "Can't do that!" ), tr( "Stop sniffing before\njoining a net." ) );
        return;
    }

    qDebug( "joinNetwork() with Interface %s: %s, %s, %d, %s",
        (const char*) iface->name(),
        (const char*) type,
        (const char*) essid,
        channel,
        (const char*) macaddr );

    QCopEnvelope msg( "QPE/Application/networksettings", "wlan(QString,QString,QString)" );
    int count = 3;
    odebug << "sending " << count << " messages" << oendl; 
    msg << QString("count") << QString::number(count);
    odebug << "msg >" << iface->name() << "< Mode >" << type.latin1() << "<" << oendl; 
    msg << QString(iface->name()) << QString("Mode") << type;
    odebug << "msg >" << iface->name() << "< essid >" << essid.latin1() << "<" << oendl; 
    msg << QString(iface->name()) << QString("ESSID") << essid;
    odebug << "msg >" << iface->name() << "< channel >" << channel << "<" << oendl; 
    msg << QString(iface->name()) << QString("Channel") << channel;
//    odebug << "msg >" << iface->name() << "< mac >" << macaddr << "<" << oendl; 
//    msg << QString(iface->name()) << QString("MacAddr") << macaddr;
    #else
    QMessageBox::warning( this, tr( "Can't do that!" ), tr( "Function only available on Embedded build" ) );
    #endif

}

