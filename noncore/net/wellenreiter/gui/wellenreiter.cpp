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

// Qt

#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qsocketnotifier.h>

// Qtopia

#ifdef QWS
#include <qpe/qpeapplication.h>
#include <qpe/global.h>
#endif

// Opie

#ifdef QWS
#include <opie/odevice.h>
using namespace Opie;
#endif

// Standard

#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <fcntl.h>

// Local

#include "wellenreiter.h"
#include "scanlist.h"
#include "logwindow.h"
#include "hexwindow.h"
#include "configwindow.h"

#include "manufacturers.h"

#include <daemon/source/config.hh>
#include <libwellenreiter/source/wl_types.hh>
#include <libwellenreiter/source/wl_sock.hh>
#include <libwellenreiter/source/wl_proto.hh>

Wellenreiter::Wellenreiter( QWidget* parent )
    : WellenreiterBase( parent, 0, 0 ),
      daemonRunning( false ), manufacturerdb( 0 ), configwindow( 0 )
{

    //
    // construct manufacturer database
    //

    QString manufile;
    #ifdef QWS
    manufile.sprintf( "%s/share/wellenreiter/manufacturers.dat", (const char*) QPEApplication::qpeDir() );
    #else
    manufile.sprintf( "/usr/local/share/wellenreiter/manufacturers.dat" );
    #endif
    manufacturerdb = new ManufacturerDB( manufile );

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

    //
    // setup socket for daemon communication, register socket notifier
    //

    // struct sockaddr_in sockaddr;
    daemon_fd = wl_setupsock( GUIADDR, GUIPORT, sockaddr );
    if ( daemon_fd == -1 )
    {
        logwindow->log( "(E) Couldn't get file descriptor for commsocket." );
    }
    else
    {
        int flags;
        flags = fcntl( daemon_fd, F_GETFL, 0 );
        fcntl( daemon_fd, F_SETFL, flags | O_NONBLOCK );
        QSocketNotifier *sn  = new QSocketNotifier( daemon_fd, QSocketNotifier::Read, parent );
        connect( sn, SIGNAL( activated( int ) ), this, SLOT( dataReceived() ) );
    }

    // setup GUI
    netview->setColumnWidthMode( 1, QListView::Manual );

    if ( manufacturerdb )
        netview->setManufacturerDB( manufacturerdb );

}

Wellenreiter::~Wellenreiter()
{
    // no need to delete child widgets, Qt does it all for us

    delete manufacturerdb;

    // X11-only - Hmm... Closing the socket here segfaults on exit,
    // Maybe because the notifier still has a handle to it!? Seems not to
    // occur on Qt/Embedded

    #ifdef QWS
    if ( daemon_fd != -1 )
    {
        qDebug( "closing comm socket" );
        close( daemon_fd );
    }
    #endif

}

void Wellenreiter::setConfigWindow( WellenreiterConfigWindow* cw )
{
    configwindow = cw;
}

void Wellenreiter::handleMessage()
{
    // FIXME: receive message and handle it

    qDebug( "received message from daemon." );

    /*char buffer[10000];
    memset( &buffer, 0, sizeof( buffer ) );*/
    
    char buffer[WL_SOCKBUF];

    // int result = #wl_recv( &daemon_fd, (char*) &buffer, sizeof(buffer) );
    
    /*
    
    struct sockaddr from;
    socklen_t len;
    
    int result = recvfrom( daemon_fd, &buffer, 8192, MSG_WAITALL, &from, &len );
    qDebug( "received %d from recv [%d bytes]", result, len );
    
    */
    
    int result = wl_recv( &daemon_fd, sockaddr, (char*) &buffer, WL_SOCKBUF );

    if ( result == -1 )
    {
        qDebug( "Warning: %s", strerror( errno ) );
        return;
    }

    int command = buffer[1] - 48;

/*
typedef struct {
  int net_type;     1 = Accesspoint ; 2 = Ad-Hoc
  int ssid_len;     Length of SSID
  int channel;      Channel
  int wep;          1 = WEP enabled ; 0 = disabled
  char mac[64];     MAC address of Accesspoint
  char bssid[128];  BSSID of Accesspoint
} wl_network_t;
*/

    qDebug( "Recv result: %d", ( result ) );
    qDebug( "Sniffer sent: '%s'", (const char*) buffer );
    hexwindow->log( (const char*) &buffer );

    if ( command == NETFOUND )  /* new network found */
    {
        qDebug( "Sniffer said: new network found." );
        wl_network_t n;
        get_network_found( &n, (char*) &buffer );
        
        qDebug( "Sniffer said: net_type is %d.", n.net_type );
        qDebug( "Sniffer said: MAC is %s", (const char*) &n.mac );

        //n.bssid[n.ssid_len] = "\0";

        QString type;
        
        if ( n.net_type == 1 )
            type = "managed";
        else
            type = "adhoc";

        netview->addNewItem( type, n.bssid, QString( (const char*) &n.mac ), n.wep, n.channel, 0 );

    }

    else

    {
        qDebug( "unknown sniffer command." );
    }

}

void Wellenreiter::dataReceived()
{
    logwindow->log( "(d) Received data from daemon" );
    handleMessage();
}

void Wellenreiter::startStopClicked()
{
    if ( daemonRunning )
    {
        daemonRunning = false;

        logwindow->log( "(i) Daemon has been stopped." );
        setCaption( tr( "Wellenreiter/Opie" ) );

        // Stop daemon - ugly for now... later better

        system( "killall wellenreiterd" );

        // get configuration from config window

        const QString& interface = configwindow->interfaceName->currentText();

        // reset the interface trying to get it into a usable state again

        QString cmdline;
        cmdline.sprintf( "iwpriv %s monitor 0; ifdown %s; ifup %s", (const char*) interface, (const char*) interface, (const char*) interface );
        system( cmdline );

        // message the user

        QMessageBox::information( this, "Wellenreiter/Opie", "Your wireless card\nshould now be usable again." );
    }

    else
    {

        // get configuration from config window

        const QString& interface = configwindow->interfaceName->currentText();
        const int cardtype = configwindow->daemonDeviceType();
        const int interval = configwindow->daemonHopInterval();

        if ( ( interface == "<select>" ) || ( cardtype == 0 ) )
        {
            QMessageBox::information( this, "Wellenreiter/Opie", "Your device is not\nptoperly configured. Please reconfigure!" );
            return;
        }

        // start wellenreiterd

        QString cmdline;
        cmdline.sprintf( "wellenreiterd %s %d &", (const char*) interface, cardtype );

        qDebug( "about to execute '%s' ...", (const char*) cmdline );
        system( cmdline );
        qDebug( "done!" );

        logwindow->log( "(i) Daemon has been started." );
        daemonRunning = true;
        setCaption( tr( "Scanning ..." ) );

    }
}
