/*
                             This file is part of the Opie Project
                             Copyright (C) 2003 by the Wellenreiter team:
                             Martin J. Muench <mjm@remote-exploit.org>
                             Max Moser <mmo@remote-exploit.org
                             Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

/* OPIE */

#include <opie2/onetwork.h>

/* QT */

#include <qfile.h>
#include <qtextstream.h>

/* UNIX */

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <math.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/wireless.h>

using namespace std;

/*======================================================================================
 * ONetwork
 *======================================================================================*/

ONetwork* ONetwork::_instance = 0;

ONetwork::ONetwork()
{
    qDebug( "ONetwork::ONetwork()" );
    synchronize();
}

void ONetwork::synchronize()
{
    // gather available interfaces by inspecting /proc/net/dev
    // we could use SIOCGIFCONF here, but we aren't interested in virtual (e.g. eth0:0) devices

    _interfaces.clear();
    QString str;
    QFile f( "/proc/net/dev" );
    bool hasFile = f.open( IO_ReadOnly );
    if ( !hasFile )
    {
        qDebug( "ONetwork: /proc/net/dev not existing. No network devices available" );
        return;
    }
    QTextStream s( &f );
    s.readLine();
    s.readLine();
    while ( !s.atEnd() )
    {
        s >> str;
        str.truncate( str.find( ':' ) );
        qDebug( "ONetwork: found interface '%s'", (const char*) str );
        ONetworkInterface* iface;
        if ( isWirelessInterface( str ) )
        {
            iface = new OWirelessNetworkInterface( str );
            qDebug( "ONetwork: interface '%s' has Wireless Extensions", (const char*) str );
        }
        else
        {
            iface = new ONetworkInterface( str );
        }
        _interfaces.insert( str, iface );
        s.readLine();
    }
}


ONetworkInterface* ONetwork::interface( QString iface ) const
{
    return _interfaces[iface];
}


ONetwork* ONetwork::instance()
{
    if ( !_instance ) _instance = new ONetwork();
    return _instance;
}


ONetwork::InterfaceIterator ONetwork::iterator() const
{
    return ONetwork::InterfaceIterator( _interfaces );
}


bool ONetwork::isWirelessInterface( const char* name ) const
{
    int sfd = socket( AF_INET, SOCK_DGRAM, 0 );
    iwreqstruct iwr;
    memset( &iwr, 0, sizeof( iwreqstruct ) );
    strcpy( (char*) &iwr.ifr_name, name );
    int result = ::ioctl( sfd, SIOCGIWNAME, &iwr );
    if ( result == -1 )
        qDebug( "ONetwork::ioctl(): SIOCGIWNAME failed: %d (%s)", result, strerror( errno ) );
    else
        qDebug( "ONetwork::ioctl(): SIOCGIWNAME ok." );
    return ( result != -1 );
}

/*======================================================================================
 * ONetworkInterface
 *======================================================================================*/

ONetworkInterface::ONetworkInterface( const QString& name )
                   :_name( name ), _sfd( socket( AF_INET, SOCK_DGRAM, 0 ) ), _mon( 0 )
{
    qDebug( "ONetworkInterface::ONetworkInterface()" );
    init();
}


ifreqstruct& ONetworkInterface::ifr() const
{
    return _ifr;
}


void ONetworkInterface::init()
{
    qDebug( "ONetworkInterface::init()" );

    memset( &_ifr, 0, sizeof( struct ifreq ) );

    if ( _sfd == -1 )
    {
        qDebug( "ONetworkInterface::init(): Warning - can't get socket for device '%s'", (const char*) _name );
        return;
    }
}


bool ONetworkInterface::ioctl( int call, ifreqstruct& ifreq ) const
{
    int result = ::ioctl( _sfd, call, &ifreq );
    if ( result == -1 )
        qDebug( "ONetworkInterface::ioctl(): Call %d - Status: Failed: %d (%s)", call, result, strerror( errno ) );
    else
        qDebug( "ONetworkInterface::ioctl(): Call %d - Status: Ok.", call );
    return ( result != -1 );
}


bool ONetworkInterface::ioctl( int call ) const
{
    strcpy( _ifr.ifr_name, (const char*) _name );
    return ioctl( call, _ifr );
}


bool ONetworkInterface::isLoopback() const
{
    ioctl( SIOCGIFFLAGS );
    return _ifr.ifr_flags & IFF_LOOPBACK;
}


bool ONetworkInterface::setUp( bool b )
{
    ioctl( SIOCGIFFLAGS );
    if ( b ) _ifr.ifr_flags |= IFF_UP;
    else   _ifr.ifr_flags &= (~IFF_UP);
    return ioctl( SIOCSIFFLAGS );
}


bool ONetworkInterface::isUp() const
{
    ioctl( SIOCGIFFLAGS );
    return _ifr.ifr_flags & IFF_UP;
}


QString ONetworkInterface::ipV4Address() const
{
    if ( ioctl( SIOCGIFADDR ) )
    {
        struct sockaddr_in *sa = (struct sockaddr_in *) &_ifr.ifr_addr;
        //FIXME: Use QHostAddress here
        return QString( inet_ntoa( sa->sin_addr ) );
    }
    else
        return "<unknown>";
}


OMacAddress ONetworkInterface::macAddress() const
{
    if ( ioctl( SIOCGIFHWADDR ) )
    {
        return OMacAddress( _ifr );
    }
    else
    {
        return OMacAddress::unknown;
    }
}


void ONetworkInterface::setMonitoring( OMonitoringInterface* m )
{
    _mon = m;
    qDebug( "ONetwork::setMonitoring(): Installed monitoring interface '%s'", (const char*) m->name() );
}


OMonitoringInterface* ONetworkInterface::monitoring() const
{
    return _mon;
}


const QString& ONetworkInterface::name() const
{
    return _name;
}


ONetworkInterface::~ONetworkInterface()
{
    qDebug( "ONetworkInterface::~ONetworkInterface()" );
    if ( _sfd != -1 ) ::close( _sfd );
}


bool ONetworkInterface::setPromiscuousMode( bool b )
{
    ioctl( SIOCGIFFLAGS );
    if ( b ) _ifr.ifr_flags |= IFF_PROMISC;
    else   _ifr.ifr_flags &= (~IFF_PROMISC);
    return ioctl( SIOCSIFFLAGS );
}


bool ONetworkInterface::promiscuousMode() const
{
    ioctl( SIOCGIFFLAGS );
    return _ifr.ifr_flags & IFF_PROMISC;
}


bool ONetworkInterface::isWireless() const
{
    return ioctl( SIOCGIWNAME );
}


/*======================================================================================
 * OChannelHopper
 *======================================================================================*/

OChannelHopper::OChannelHopper( OWirelessNetworkInterface* iface )
               :QObject( 0, "Mickey's funky hopper" ),
               _iface( iface ), _interval( 0 ), _channel( 1 ), _tid( 0 ),
               _maxChannel( iface->channels()+1 )
{
}


OChannelHopper::~OChannelHopper()
{
}


bool OChannelHopper::isActive() const
{
    return _tid;
}


int OChannelHopper::channel() const
{
    return _channel;
}


void OChannelHopper::timerEvent( QTimerEvent* )
{
    if ( !--_channel ) _channel = _maxChannel;
    _iface->setChannel( _channel );
    qDebug( "OChannelHopper::timerEvent(): set channel %d on interface '%s'",
            _channel, (const char*) _iface->name() );
}


void OChannelHopper::setInterval( int interval )
{
    if ( interval == _interval )
        return;

    if ( _interval )
        killTimer( _tid );

    _tid = 0;
    _interval = interval;

    if ( _interval )
    {
        _tid = startTimer( interval );
    }
}


int OChannelHopper::interval() const
{
    return _interval;
}


/*======================================================================================
 * OWirelessNetworkInterface
 *======================================================================================*/

OWirelessNetworkInterface::OWirelessNetworkInterface( const QString& name )
                           :ONetworkInterface( name ), _hopper( 0 )
{
    qDebug( "OWirelessNetworkInterface::OWirelessNetworkInterface()" );
    init();
}


OWirelessNetworkInterface::~OWirelessNetworkInterface()
{
}


iwreqstruct& OWirelessNetworkInterface::iwr() const
{
    return _iwr;
}


void OWirelessNetworkInterface::init()
{
    qDebug( "OWirelessNetworkInterface::init()" );

    memset( &_iwr, 0, sizeof( struct iwreq ) );

    // IEEE802.11(b) radio frequency channels
    //FIXME: get these directly from the interface
    //FIXME: check if these channels are off-by-one

    iwrangestruct range;
    _iwr.u.data.pointer = (char*) &range;
    _iwr.u.data.length = sizeof( iwrangestruct );
    if ( !wioctl( SIOCGIWRANGE ) )
    {
        qDebug( "OWirelessNetworkInterface::init(): SIOCGIWRANGE failed (%s)", strerror( errno ) );
        return;
    }

    for ( int i = 0; i < range.num_frequency; ++i )
    {
        int freq = (int) ( double( range.freq[i].m ) * pow( 10, range.freq[i].e ) / 1000000.0 );
        _channels.insert( freq, i+1 );
    }
}


QString OWirelessNetworkInterface::associatedAP() const
{
    //FIXME: use OMacAddress
    QString mac;

    if ( ioctl( SIOCGIWAP ) )
    {
        mac.sprintf( "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
        _ifr.ifr_hwaddr.sa_data[0]&0xff,
        _ifr.ifr_hwaddr.sa_data[1]&0xff,
        _ifr.ifr_hwaddr.sa_data[2]&0xff,
        _ifr.ifr_hwaddr.sa_data[3]&0xff,
        _ifr.ifr_hwaddr.sa_data[4]&0xff,
        _ifr.ifr_hwaddr.sa_data[5]&0xff );
    }
    else
    {
        mac = "<Unknown>";
    }
    return mac;
}


int OWirelessNetworkInterface::channel() const
{
    //FIXME: When monitoring enabled, then use it
    //FIXME: to gather the current RF channel
    //FIXME: Until then, get active channel from hopper.
    if ( _hopper && _hopper->isActive() )
        return _hopper->channel();

    if ( !wioctl( SIOCGIWFREQ ) )
    {
        return -1;
    }
    else
    {
        return _channels[ static_cast<int>(double( _iwr.u.freq.m ) * pow( 10, _iwr.u.freq.e ) / 1000000) ];
    }
}


void OWirelessNetworkInterface::setChannel( int c ) const
{
    if ( !_mon )
    {
        memset( &_iwr, 0, sizeof( iwreqstruct ) );
        _iwr.u.freq.m = c;
        _iwr.u.freq.e = 0;
        wioctl( SIOCSIWFREQ );
    }
    else
    {
        _mon->setChannel( c );
    }
}


double OWirelessNetworkInterface::frequency() const
{
    if ( !wioctl( SIOCGIWFREQ ) )
    {
        return -1.0;
    }
    else
    {
        return double( _iwr.u.freq.m ) * pow( 10, _iwr.u.freq.e ) / 1000000000.0;
    }
}


int OWirelessNetworkInterface::channels() const
{
    return _channels.count();
}


void OWirelessNetworkInterface::setChannelHopping( int interval )
{
    if ( !_hopper ) _hopper = new OChannelHopper( this );
    _hopper->setInterval( interval );
    //FIXME: When and by whom will the channel hopper be deleted?
}


int OWirelessNetworkInterface::channelHopping() const
{
    return _hopper->interval();
}


void OWirelessNetworkInterface::setMonitorMode( bool b )
{
    if ( _mon )
        _mon->setEnabled( b );
    else
        qDebug( "ONetwork(): can't switch monitor mode without installed monitoring interface" );
}


bool OWirelessNetworkInterface::monitorMode() const
{
    return _mon ? _mon->enabled() : false;
}


QString OWirelessNetworkInterface::nickName() const
{
    char str[IW_ESSID_MAX_SIZE];
    _iwr.u.data.pointer = &str[0];
    _iwr.u.data.length = IW_ESSID_MAX_SIZE;
    if ( !wioctl( SIOCGIWNICKN ) )
    {
        return "<unknown>";
    }
    else
    {
        str[_iwr.u.data.length] = 0x0; // some drivers (e.g. wlan-ng) don't zero-terminate the string
        return str;
    }
}


QString OWirelessNetworkInterface::SSID() const
{
    char str[IW_ESSID_MAX_SIZE];
    _iwr.u.essid.pointer = &str[0];
    _iwr.u.essid.length = IW_ESSID_MAX_SIZE;
    if ( !wioctl( SIOCGIWESSID ) )
    {
        return "<unknown>";
    }
    else
    {
        return str;
    }
}


void OWirelessNetworkInterface::setSSID( const QString& ssid )
{
    _iwr.u.essid.pointer = const_cast<char*>( (const char*) ssid );
    _iwr.u.essid.length = ssid.length();
    wioctl( SIOCSIWESSID );
}


bool OWirelessNetworkInterface::wioctl( int call, iwreqstruct& iwreq ) const
{
    int result = ::ioctl( _sfd, call, &iwreq );
    if ( result == -1 )
        qDebug( "ONetworkInterface::wioctl(): Call %d - Status: Failed: %d (%s)", call, result, strerror( errno ) );
    else
        qDebug( "ONetworkInterface::wioctl(): Call %d - Status: Ok.", call );
    return ( result != -1 );
}


bool OWirelessNetworkInterface::wioctl( int call ) const
{
    strcpy( _iwr.ifr_name, (const char*) _name );
    return wioctl( call, _iwr );
}


/*======================================================================================
 * OMonitoringInterface
 *======================================================================================*/

OMonitoringInterface::OMonitoringInterface( ONetworkInterface* iface )
                      :_enabled( false ), _if( static_cast<OWirelessNetworkInterface*>( iface ) )
{
}


OMonitoringInterface::~OMonitoringInterface()
{
}


void OMonitoringInterface::setChannel( int c )
{
    // use standard WE channel switching protocol
    memset( &_if->_iwr, 0, sizeof( iwreqstruct ) );
    _if->_iwr.u.freq.m = c;
    _if->_iwr.u.freq.e = 0;
    _if->wioctl( SIOCSIWFREQ );
}


bool OMonitoringInterface::enabled() const
{
    return _enabled;
}

void OMonitoringInterface::setEnabled( bool b )
{
    // open a packet capturer here or leave this to
    // the client code?

    /*

    if ( b )
    {
        OPacketCapturer* opcap = new OPacketCapturer();
        opcap->open( _if->name() );
    }
    */

    _enabled = b;

}

/*======================================================================================
 * OCiscoMonitoringInterface
 *======================================================================================*/

OCiscoMonitoringInterface::OCiscoMonitoringInterface( ONetworkInterface* iface )
                           :OMonitoringInterface( iface )
{
    iface->setMonitoring( this );
}


OCiscoMonitoringInterface::~OCiscoMonitoringInterface()
{
}


void OCiscoMonitoringInterface::setEnabled( bool b )
{
    QString fname;
    fname.sprintf( "/proc/driver/aironet/%s", (const char*) _if->name() );
    QFile f( fname );
    if ( !f.exists() ) return;

    if ( f.open( IO_WriteOnly ) )
    {
        QTextStream s( &f );
        s << "Mode: r";
        s << "Mode: y";
        s << "XmitPower: 1";

        OMonitoringInterface::setEnabled( b );

    }

    // flushing and closing will be done automatically when f goes out of scope
}


QString OCiscoMonitoringInterface::name() const
{
    return "cisco";
}


void OCiscoMonitoringInterface::setChannel( int )
{
    // cisco devices automatically switch channels when in monitor mode
}


/*======================================================================================
 * OWlanNGMonitoringInterface
 *======================================================================================*/


OWlanNGMonitoringInterface::OWlanNGMonitoringInterface( ONetworkInterface* iface )
                           :OMonitoringInterface( iface )
{
    iface->setMonitoring( this );
}


OWlanNGMonitoringInterface::~OWlanNGMonitoringInterface()
{
}


void OWlanNGMonitoringInterface::setEnabled( bool b )
{
    //FIXME: do nothing if its already in the same mode

    QString enable = b ? "true" : "false";
    QString cmd;
    cmd.sprintf( "$(which wlanctl-ng) %s lnxreq_wlansniff channel=%d enable=%s", (const char*) _if->name(), 1, (const char*) enable );
    system( cmd );

    OMonitoringInterface::setEnabled( b );
}


QString OWlanNGMonitoringInterface::name() const
{
    return "wlan-ng";
}


void OWlanNGMonitoringInterface::setChannel( int )
{
    // wlan-ng devices automatically switch channels when in monitor mode
}


/*======================================================================================
 * OHostAPMonitoringInterface
 *======================================================================================*/

OHostAPMonitoringInterface::OHostAPMonitoringInterface( ONetworkInterface* iface )
                           :OMonitoringInterface( iface )
{
    iface->setMonitoring( this );
}

OHostAPMonitoringInterface::~OHostAPMonitoringInterface()
{
}

void OHostAPMonitoringInterface::setEnabled( bool b )
{
    // IW_MODE_MONITOR was introduced in Wireless Extensions Version 15
    // Wireless Extensions < Version 15 need iwpriv commandos for monitoring

    #if WIRELESS_EXT > 14
    _if->_iwr.u.mode = IW_MODE_MONITOR;
    _if->wioctl( SIOCSIWMODE );
    #else
    int* args = (int*) &_if._iwr.u.name;
    args[0] = 2;
    args[1] = 0;
    _if->wioctl( SIOCDEVPRIVATE );
    #endif

    OMonitoringInterface::setEnabled( b );
}


QString OHostAPMonitoringInterface::name() const
{
    return "hostap";
}


/*======================================================================================
 * OOrinocoNetworkInterface
 *======================================================================================*/

OOrinocoMonitoringInterface::OOrinocoMonitoringInterface( ONetworkInterface* iface )
                           :OMonitoringInterface( iface )
{
    iface->setMonitoring( this );
}


OOrinocoMonitoringInterface::~OOrinocoMonitoringInterface()
{
}


void OOrinocoMonitoringInterface::setChannel( int c )
{
    // call iwpriv <device> monitor 2 <channel>
    int* args = (int*) &_if->_iwr.u.name;
    args[0] = 2;
    args[1] = c;
    _if->wioctl( SIOCIWFIRSTPRIV + 0x8 );
}


void OOrinocoMonitoringInterface::setEnabled( bool b )
{
    if ( b )
    {
        setChannel( 1 );
    }
    else
    {
        // call iwpriv <device> monitor 0 0
        int* args = (int*) &_if->_iwr.u.name;
        args[0] = 0;
        args[1] = 0;
        _if->wioctl( SIOCIWFIRSTPRIV + 0x8 );
    }

    OMonitoringInterface::setEnabled( b );
}


QString OOrinocoMonitoringInterface::name() const
{
    return "orinoco";
}
