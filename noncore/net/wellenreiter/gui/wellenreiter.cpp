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

Wellenreiter::Wellenreiter( QWidget* parent, const char* name, WFlags fl )
    : WellenreiterBase( parent, name, fl ), daemonRunning( false ), manufacturerdb( 0 )
{

    //
    // construct manufacturer database
    //
    
    QString manufile;
    #ifdef QWS
    manufile.sprintf( "%s/share/wellenreiter/manufacturers.dat", (const char*) QPEApplication::qpeDir() );
    #else
    manufile.sprintf( "/home/mickey/work/opie/share/wellenreiter/manufacturers.dat" );
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
        
    connect( button, SIGNAL( clicked() ), this, SLOT( buttonClicked() ) );
    // button->setEnabled( false );
    netview->setColumnWidthMode( 1, QListView::Manual );
    
    if ( manufacturerdb )
        netview->setManufacturerDB( manufacturerdb );
        
}

Wellenreiter::~Wellenreiter()
{
    // no need to delete child widgets, Qt does it all for us
    
    delete manufacturerdb;
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

void Wellenreiter::buttonClicked()
{
        /*
        // add some test stations, so that we can see if the GUI part works
        addNewItem( "managed", "Vanille", "04:00:20:EF:A6:43", true, 6, 80 );
        addNewItem( "managed", "Vanille", "04:00:20:EF:A6:23", true, 11, 10 );
        addNewItem( "adhoc", "ELAN", "40:03:43:E7:16:22", false, 3, 10 );
        addNewItem( "adhoc", "ELAN", "40:03:53:E7:56:62", false, 3, 15 );
        addNewItem( "adhoc", "ELAN", "40:03:63:E7:56:E2", false, 3, 20 );
        */


    if ( daemonRunning )
    {
        daemonRunning = false;
        
        logwindow->log( "(i) Daemon has been stopped." );
        button->setText( "Start Scanning" );
        
        // Stop daemon - ugly for now... later better
        
        system( "killall orinoco_hopper" );
        system( "killall wellenreiterd" );
        
        // get configuration from config window
        
        const QString& interface = configwindow->interfaceName->currentText();
        const QString& cardtype = configwindow->deviceType->currentText();
        const QString& interval = configwindow->hopInterval->cleanText();
        
        // reset the card trying to get into a usable state again
        
        QString cmdline;
        cmdline.sprintf( "iwpriv %s monitor 1", (const char*) interface );
        system( cmdline );
        cmdline.sprintf( "iwpriv %s monitor 1 6", (const char*) interface );
        system( cmdline );
        cmdline.sprintf( "ifconfig %s -promisc", (const char*) interface );
        system( cmdline );
        cmdline.sprintf( "killall -14 dhcpcd" );
        system( cmdline );
        cmdline.sprintf( "killall -10 udhcpc" );
        
        // message the user

        QMessageBox::information( this, "Wellenreiter/Opie", "Your wireless card\nshould now be usable again." );
    }
    
    else
    {    
    
        logwindow->log( "(i) Daemon has been started." );
        daemonRunning = true;
        button->setText( "Stop Scanning" );
        
        // get configuration from config window
        
        const QString& interface = configwindow->interfaceName->currentText();
        const QString& cardtype = configwindow->deviceType->currentText();
        const QString& interval = configwindow->hopInterval->cleanText();
        
        if ( ( interface == "<select>" ) || ( cardtype == "<select>" ) )
        {
            QMessageBox::information( this, "Wellenreiter/Opie", "You must configure your\ndevice before scanning." );
            return;
        }
        
        // set interface into monitor mode
        /* Global::Execute definitely does not work very well with non-gui stuff! :( */
        
        QString cmdline;
        cmdline.sprintf( "iwpriv %s monitor 2", (const char*) interface );
        system( cmdline );
        cmdline.sprintf( "iwpriv %s monitor 2 1", (const char*) interface );
        system( cmdline );
        
        // start channel hopper
        
        cmdline = "orinoco_hopper ";
        cmdline += interface;
        cmdline += " -i ";
        cmdline += interval;
        cmdline += " &";
        qDebug( "execute: %s", (const char*) cmdline );
        system( cmdline ); 
        qDebug( "done" );
        
        // start daemon
        
        cmdline = "wellenreiterd ";
        cmdline += interface;
        cmdline += " 3";
        cmdline += " &";

        qDebug( "execute: %s", (const char*) cmdline );       
        system( cmdline );
        qDebug( "done" );

    }
}
