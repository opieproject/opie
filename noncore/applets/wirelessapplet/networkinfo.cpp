/**********************************************************************
** MNetwork* classes
**
** Encapsulate network information
**
** Copyright (C) 2002, Michael Lauer
**                    mickey@tm.informatik.uni-frankfurt.de
**                    http://www.Vanille.de
**
** Based on portions of the Wireless Extensions
** Copyright (c) 1997-2002 Jean Tourrilhes <jt@hpl.hp.com>
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "networkinfo.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <netinet/ip.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>

#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <string.h>

#include <stdlib.h>

#include <qstring.h>
#include <qfile.h>
#include <qtextstream.h>

/* estimated wireless signal strength and noise level values
   based on experimentation with Orinoco and Prism2 cards.
   Seem to be correct, but please inform me, if you got values
   outside these boundaries. :Mickey: */

#define IW_UPPER 220
#define IW_LOWER 140

#define PROCNETDEV "/proc/net/dev"
#define PROCNETWIRELESS "/proc/net/wireless"

#define MDEBUG 0

//---------------------------------------------------------------------------
// class MNetworkInterface
//

MNetworkInterface::MNetworkInterface( const char* name )
                  :name( name )
{
    struct ifreq ifr;
    struct sockaddr_in *sin = (struct sockaddr_in *) &ifr.ifr_addr;
    fd = socket( AF_INET, SOCK_DGRAM, 0 );
}

MNetworkInterface::~MNetworkInterface()
{
    if ( fd != -1 )
        close( fd );
}

bool MNetworkInterface::updateStatistics()
{
    return true;
}

//---------------------------------------------------------------------------
// class MWirelessNetworkInterface
//

MWirelessNetworkInterface::MWirelessNetworkInterface( const char* n )
                         :MNetworkInterface( n )
{
    signal = 0;
    noise = 0;
    quality = 0;  
}

MWirelessNetworkInterface::~MWirelessNetworkInterface()
{
}

int MWirelessNetworkInterface::qualityPercent()
{
    return ( quality*100 ) / 92;
}

int MWirelessNetworkInterface::signalPercent()
{
    return ( ( signal-IW_LOWER ) * 100 ) / IW_UPPER;
}

int MWirelessNetworkInterface::noisePercent()
{
    return ( ( noise-IW_LOWER ) * 100 ) / IW_UPPER;
}

bool MWirelessNetworkInterface::updateStatistics()
{
        bool base = MNetworkInterface::updateStatistics();
        if ( !base )
            return false;

        const char* buffer[200];

        struct iwreq iwr;
        memset( &iwr, 0, sizeof( iwr ) );
        iwr.u.essid.pointer = (caddr_t) buffer;
        iwr.u.essid.length = IW_ESSID_MAX_SIZE;
        iwr.u.essid.flags = 0;

        // check if it is an IEEE 802.11 standard conform
        // wireless device by sending SIOCGIWESSID
        // which also gives back the Extended Service Set ID
        // (see IEEE 802.11 for more information)

        strcpy( iwr.ifr_ifrn.ifrn_name, (const char*) name );
        int result = ioctl( fd, SIOCGIWESSID, &iwr );
        if ( result == 0 )
        {
            hasWirelessExtensions = true;
            iwr.u.essid.pointer[(unsigned int) iwr.u.essid.length-1] = '\0';
            essid = iwr.u.essid.pointer;
        }
        else essid = "*** Unknown ***";

        // Address of associated access-point

        result = ioctl( fd, SIOCGIWAP, &iwr );
        if ( result == 0 )
        {
            APAddr.sprintf( "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
            iwr.u.ap_addr.sa_data[0]&0xff,
            iwr.u.ap_addr.sa_data[1]&0xff,
            iwr.u.ap_addr.sa_data[2]&0xff,
            iwr.u.ap_addr.sa_data[3]&0xff,
            iwr.u.ap_addr.sa_data[4]&0xff,
            iwr.u.ap_addr.sa_data[5]&0xff );
        } else APAddr = "*** Unknown ***";

        iwr.u.data.pointer = (caddr_t) buffer;
        iwr.u.data.length = IW_ESSID_MAX_SIZE;
        iwr.u.data.flags = 0;

        result = ioctl( fd, SIOCGIWNICKN, &iwr );
        if ( result == 0 )
        {
            iwr.u.data.pointer[(unsigned int) iwr.u.data.length-1] = '\0';
            nick = iwr.u.data.pointer;
        } else nick = "*** Unknown ***";

        result = ioctl( fd, SIOCGIWMODE, &iwr );
        if ( result == 0 )
            mode = ( iwr.u.mode == IW_MODE_ADHOC ) ? "Ad-Hoc" : "Managed";
        else mode = "*** Unknown ***";

        result = ioctl( fd, SIOCGIWFREQ, &iwr );
        if ( result == 0 )
            freq = double( iwr.u.freq.m ) * pow( 10, iwr.u.freq.e ) / 1000000000;
        else freq = 0;

    // gather link quality from /proc/net/wireless

    char c;
    QString status;
    QString name;
    QFile wfile( PROCNETWIRELESS );
    bool hasFile = wfile.open( IO_ReadOnly );
    QTextStream wstream( &wfile );
    if ( hasFile )
    {
        wstream.readLine();  // skip the first two lines
        wstream.readLine();  // because they only contain headers
    }
    if ( ( !hasFile ) || ( wstream.atEnd() ) )
    {
#ifdef MDEBUG
        qDebug( "WIFIAPPLET: D'oh! Someone removed the card..." );
#endif
        quality = -1;
        signal = IW_LOWER;
        noise = IW_LOWER;
        return false;
    }

    wstream >> name >> status >> quality >> c >> signal >> c >> noise;

    if ( quality > 92 )
#ifdef MDEBUG
        qDebug( "WIFIAPPLET: D'oh! Quality %d > estimated max!\n", quality );
#endif
    if ( ( signal > IW_UPPER ) || ( signal < IW_LOWER ) )
#ifdef MDEBUG
        qDebug( "WIFIAPPLET: Doh! Strength %d > estimated max!\n", signal );
#endif
    if ( ( noise > IW_UPPER ) || ( noise < IW_LOWER ) )
#ifdef MDEBUG
        qDebug( "WIFIAPPLET: Doh! Noise %d > estimated max!\n", noise );
#endif

    return true;
        
}

//---------------------------------------------------------------------------
// class Network
//

MNetwork::MNetwork()
{
    procfile = PROCNETDEV;
}

MNetwork::~MNetwork()
{
}

//---------------------------------------------------------------------------
// class WirelessNetwork
//

MWirelessNetwork::MWirelessNetwork()
{
    procfile = PROCNETWIRELESS;
}

MWirelessNetwork::~MWirelessNetwork()
{
}

MNetworkInterface* MWirelessNetwork::createInterface( const char* n ) const
{
    return new MWirelessNetworkInterface( n );
}

//---------------------------------------------------------------------------
// class NetworkInterface
//

MNetworkInterface* MNetwork::getFirstInterface()
{
    enumerateInterfaces();
    InterfaceMapIterator it( interfaces );
    return ( it.count() > 0 ) ? it.toFirst() : 0;
}

void MNetwork::enumerateInterfaces()
{   
    interfaces.clear();
    QString str;
    QFile f( procfile );
    bool hasFile = f.open( IO_ReadOnly );
    if ( !hasFile )
        return;
    QTextStream s( &f );
    s.readLine();
    s.readLine();
    while ( !s.atEnd() )
    {
        s >> str;
        str.truncate( str.find( ':' ) );
#ifdef MDEBUG
        qDebug( "WIFIAPPLET: found interface '%s'", (const char*) str );
#endif
        interfaces.insert( str, createInterface( str ) );
        s.readLine();
    }
}

MNetworkInterface* MNetwork::createInterface( const char* n ) const
{
    return new MNetworkInterface( n );
}
