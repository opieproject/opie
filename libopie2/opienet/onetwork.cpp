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
#include <opie2/ostation.h>

/* QT */

#include <qfile.h>
#include <qtextstream.h>

/* UNIX */

#include <assert.h>
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <math.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/sockios.h>
#include <net/if_arp.h>
#include <stdarg.h>

using namespace std;

#ifndef NODEBUG
#include <opie2/oioctlmap.h>
IntStringMap* _ioctlmap = constructIoctlMap();
#endif

/*======================================================================================
 * ONetwork
 *======================================================================================*/

ONetwork* ONetwork::_instance = 0;

ONetwork::ONetwork()
{
    qDebug( "ONetwork::ONetwork()" );
    qDebug( "ONetwork: This code has been compiled against Wireless Extensions V%d", WIRELESS_EXT );
    synchronize();
}

void ONetwork::synchronize()
{
    // gather available interfaces by inspecting /proc/net/dev
    //FIXME: we could use SIOCGIFCONF here, but we aren't interested in virtual (e.g. eth0:0) devices
    //FIXME: Use SIOCGIFCONF anway, because we can disable listing of aliased devices
    //FIXME: Best is use SIOCGIFCONF and if this doesn't work (result=-1), then fallback to parsing /proc/net/dev

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
            iface = new OWirelessNetworkInterface( this, (const char*) str );
            qDebug( "ONetwork: interface '%s' has Wireless Extensions", (const char*) str );
        }
        else
        {
            iface = new ONetworkInterface( this, (const char*) str );
        }
        _interfaces.insert( str, iface );
        s.readLine();
    }
}


short ONetwork::wirelessExtensionVersion()
{
    return WIRELESS_EXT;
}


int ONetwork::count() const
{
    return _interfaces.count();
}


ONetworkInterface* ONetwork::interface( const QString& iface ) const
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
    int sfd = socket( AF_INET, SOCK_STREAM, 0 );
    struct iwreq iwr;
    memset( &iwr, 0, sizeof( struct iwreq ) );
    strcpy( (char*) &iwr.ifr_name, name );
    int result = ::ioctl( sfd, SIOCGIWNAME, &iwr );
    return result != -1;
}

/*======================================================================================
 * ONetworkInterface
 *======================================================================================*/

ONetworkInterface::ONetworkInterface( QObject* parent, const char* name )
                 :QObject( parent, name ),
                 _sfd( socket( AF_INET, SOCK_DGRAM, 0 ) ), _mon( 0 )
{
    qDebug( "ONetworkInterface::ONetworkInterface()" );
    init();
}


struct ifreq& ONetworkInterface::ifr() const
{
    return _ifr;
}


void ONetworkInterface::init()
{
    qDebug( "ONetworkInterface::init()" );

    memset( &_ifr, 0, sizeof( struct ifreq ) );

    if ( _sfd == -1 )
    {
        qDebug( "ONetworkInterface::init(): Warning - can't get socket for device '%s'", name() );
        return;
    }
}


bool ONetworkInterface::ioctl( int call, struct ifreq& ifreq ) const
{
    #ifndef NODEBUG
    int result = ::ioctl( _sfd, call, &ifreq );
    if ( result == -1 )
        qDebug( "ONetworkInterface::ioctl (%s) call %s (0x%04X) - Status: Failed: %d (%s)", name(), (const char*) *(*_ioctlmap)[call], call, result, strerror( errno ) );
    else
        qDebug( "ONetworkInterface::ioctl (%s) call %s (0x%04X) - Status: Ok.", name(), (const char*) *(*_ioctlmap)[call], call );
    return ( result != -1 );
    #else
    return ::ioctl( _sfd, call, &ifreq ) != -1;
    #endif
}


bool ONetworkInterface::ioctl( int call ) const
{
    strcpy( _ifr.ifr_name, name() );
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


void ONetworkInterface::setIPV4Address( const QHostAddress& addr )
{
    struct sockaddr_in *sa = (struct sockaddr_in *) &_ifr.ifr_addr;
    sa->sin_family = AF_INET;
    sa->sin_port = 0;
    sa->sin_addr.s_addr = htonl( addr.ip4Addr() );
    ioctl( SIOCSIFADDR );
}


QString ONetworkInterface::ipV4Address() const
{
    if ( ioctl( SIOCGIFADDR ) )
    {
        struct sockaddr_in* sa = (struct sockaddr_in *) &_ifr.ifr_addr;
        //FIXME: Use QHostAddress here
        return QString( inet_ntoa( sa->sin_addr ) );
    }
    else
        return "<unknown>";
}


void ONetworkInterface::setMacAddress( const OMacAddress& addr )
{
    _ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    memcpy( &_ifr.ifr_hwaddr.sa_data, addr.native(), 6 );
    ioctl( SIOCSIFHWADDR );
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


void ONetworkInterface::setIPV4Netmask( const QHostAddress& addr )
{
    struct sockaddr_in *sa = (struct sockaddr_in *) &_ifr.ifr_addr;
    sa->sin_family = AF_INET;
    sa->sin_port = 0;
    sa->sin_addr.s_addr = htonl( addr.ip4Addr() );
    ioctl( SIOCSIFNETMASK );
}


QString ONetworkInterface::ipV4Netmask() const
{
    if ( ioctl( SIOCGIFNETMASK ) )
    {
        struct sockaddr_in* sa = (struct sockaddr_in *) &_ifr.ifr_addr;
        //FIXME: Use QHostAddress here
        return QString( inet_ntoa( sa->sin_addr ) );
    }
    else
        return "<unknown>";
}


int ONetworkInterface::dataLinkType() const
{
    if ( ioctl( SIOCGIFHWADDR ) )
    {
        return _ifr.ifr_hwaddr.sa_family;
    }
    else
    {
        return -1;
    }
}


void ONetworkInterface::setMonitoring( OMonitoringInterface* m )
{
    _mon = m;
    qDebug( "ONetwork::setMonitoring(): Installed monitoring driver '%s' on interface '%s'", (const char*) m->name(), name() );
}


OMonitoringInterface* ONetworkInterface::monitoring() const
{
    return _mon;
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
               _iface( iface ), _interval( 0 ), _tid( 0 )
{
    int _maxChannel = iface->channels()+1;
    // generate fancy hopping sequence honoring the device capabilities
    if ( _maxChannel >=  1 ) _channels.append(  1 );
    if ( _maxChannel >=  7 ) _channels.append(  7 );
    if ( _maxChannel >= 13 ) _channels.append( 13 );
    if ( _maxChannel >=  2 ) _channels.append(  2 );
    if ( _maxChannel >=  8 ) _channels.append(  8 );
    if ( _maxChannel >=  3 ) _channels.append(  3 );
    if ( _maxChannel >= 14 ) _channels.append( 14 );
    if ( _maxChannel >=  9 ) _channels.append(  9 );
    if ( _maxChannel >=  4 ) _channels.append(  4 );
    if ( _maxChannel >= 10 ) _channels.append( 10 );
    if ( _maxChannel >=  5 ) _channels.append(  5 );
    if ( _maxChannel >= 11 ) _channels.append( 11 );
    if ( _maxChannel >=  6 ) _channels.append(  6 );
    if ( _maxChannel >= 12 ) _channels.append( 12 );
    _channel = _channels.begin();

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
    return *_channel;
}


void OChannelHopper::timerEvent( QTimerEvent* )
{
    _iface->setChannel( *_channel );
    emit( hopped( *_channel ) );
    qDebug( "OChannelHopper::timerEvent(): set channel %d on interface '%s'",
            *_channel, (const char*) _iface->name() );
    if ( ++_channel == _channels.end() ) _channel = _channels.begin();
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

OWirelessNetworkInterface::OWirelessNetworkInterface( QObject* parent, const char* name )
                         :ONetworkInterface( parent, name ), _hopper( 0 )
{
    qDebug( "OWirelessNetworkInterface::OWirelessNetworkInterface()" );
    init();
}


OWirelessNetworkInterface::~OWirelessNetworkInterface()
{
}


struct iwreq& OWirelessNetworkInterface::iwr() const
{
    return _iwr;
}


void OWirelessNetworkInterface::init()
{
    qDebug( "OWirelessNetworkInterface::init()" );
    memset( &_iwr, 0, sizeof( struct iwreq ) );
    buildInformation();
    buildPrivateList();
    dumpInformation();
}


bool OWirelessNetworkInterface::isAssociated() const
{
    //FIXME: handle different modes
    return associatedAP() != "44:44:44:44:44:44";
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


void OWirelessNetworkInterface::buildInformation()
{
    //ML: If you listen carefully enough, you can hear lots of WLAN drivers suck
    //ML: The HostAP drivers need more than sizeof struct_iw range to complete
    //ML: SIOCGIWRANGE otherwise they fail with "Invalid Argument Length".
    //ML: The Wlan-NG drivers on the otherside fail (segfault!) if you allocate
    //ML: _too much_ space. This is damn shitty crap *sigh*
    //ML: We allocate a large memory region in RAM and check whether the
    //ML: driver pollutes this extra space. The complaint will be made on stdout,
    //ML: so please forward this...

    struct iwreq wrq;
    int len = sizeof( struct iw_range )*2;
    char *buffer = (char*) malloc( len );
    //FIXME: Validate if we actually got the memory block
    memset( buffer, 0, len );
    memcpy( wrq.ifr_name, name(), IFNAMSIZ);
    wrq.u.data.pointer = (caddr_t) buffer;
    wrq.u.data.length = sizeof( struct iw_range );
    wrq.u.data.flags = 0;

    if ( ::ioctl( _sfd, SIOCGIWRANGE, &wrq ) == -1 )
    {
        qDebug( "OWirelessNetworkInterface::buildInformation(): SIOCGIWRANGE failed (%s) - using default values.", strerror( errno ) );
        _channels.insert( 2412,  1 ); // 2.412 GHz
        _channels.insert( 2417,  2 ); // 2.417 GHz
        _channels.insert( 2422,  3 ); // 2.422 GHz
        _channels.insert( 2427,  4 ); // 2.427 GHz
        _channels.insert( 2432,  5 ); // 2.432 GHz
        _channels.insert( 2437,  6 ); // 2.437 GHz
        _channels.insert( 2442,  7 ); // 2.442 GHz
        _channels.insert( 2447,  8 ); // 2.447 GHz
        _channels.insert( 2452,  9 ); // 2.452 GHz
        _channels.insert( 2457, 10 ); // 2.457 GHz
        _channels.insert( 2462, 11 ); // 2.462 GHz

        memset( &_range, 0, sizeof( struct iw_range ) );
    }
    else
    {
        // <check if the driver overwrites stuff>
        int max = 0;
        for ( int r = sizeof( struct iw_range ); r < len; r++ )
            if (buffer[r] != 0)
                max = r;
        if (max > 0)
        {
            qWarning( "OWirelessNetworkInterface::buildInformation(): Driver for wireless interface '%s' sucks!\n"
                    "It overwrote the buffer end with at least %i bytes!\n", name(), max - sizeof( struct iw_range ) );
        }
        // </check if the driver overwrites stuff>

        struct iw_range range;
        memcpy( &range, buffer, sizeof range );

        qDebug( "OWirelessNetworkInterface::buildInformation(): Interface %s reported to have %d channels.", name(), range.num_frequency );
        for ( int i = 0; i < range.num_frequency; ++i )
        {
            int freq = (int) ( double( range.freq[i].m ) * pow( 10.0, range.freq[i].e ) / 1000000.0 );
            _channels.insert( freq, i+1 );
        }
    }

    memcpy( &_range, buffer, sizeof( struct iw_range ) );
    qDebug( "OWirelessNetworkInterface::buildInformation(): Information block constructed." );
    free(buffer);
}


void OWirelessNetworkInterface::buildPrivateList()
{
    qDebug( "OWirelessNetworkInterface::buildPrivateList()" );

    struct iw_priv_args priv[IW_MAX_PRIV_DEF];

    _iwr.u.data.pointer = (char*) &priv;
    _iwr.u.data.length = IW_MAX_PRIV_DEF; // length in terms of number of (sizeof iw_priv_args), not (sizeof iw_priv_args) itself
    _iwr.u.data.flags = 0;

    if ( !wioctl( SIOCGIWPRIV ) )
    {
        qDebug( "OWirelessNetworkInterface::buildPrivateList(): SIOCGIWPRIV failed (%s) - can't get private ioctl information.", strerror( errno ) );
        return;
    }

    for ( int i = 0; i < _iwr.u.data.length; ++i )
    {
        new OPrivateIOCTL( this, priv[i].name, priv[i].cmd, priv[i].get_args, priv[i].set_args );
    }
    qDebug( "OWirelessNetworkInterface::buildPrivateList(): Private IOCTL list constructed." );
}


void OWirelessNetworkInterface::dumpInformation() const
{
    qDebug( "OWirelessNetworkInterface::() -------------- dumping information block ----------------" );

    qDebug( " - driver's idea of maximum throughput is %d bps = %d byte/s = %d Kb/s = %f.2 Mb/s", _range.throughput, _range.throughput / 8, _range.throughput / 8 / 1024, float( _range.throughput ) / 8.0 / 1024.0 / 1024.0 );
    qDebug( " - driver for '%s' has been compiled against WE V%d (source=V%d)", name(), _range.we_version_compiled, _range.we_version_source );

    qDebug( "OWirelessNetworkInterface::() ---------------------------------------------------------" );
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
        return _channels[ static_cast<int>(double( _iwr.u.freq.m ) * pow( 10.0, _iwr.u.freq.e ) / 1000000) ];
    }
}


void OWirelessNetworkInterface::setChannel( int c ) const
{
    if ( !c )
    {
        qWarning( "OWirelessNetworkInterface::setChannel( 0 ) called - fix your application!" );
        return;
    }

    if ( !_mon )
    {
        memset( &_iwr, 0, sizeof( struct iwreq ) );
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
        return double( _iwr.u.freq.m ) * pow( 10.0, _iwr.u.freq.e ) / 1000000000.0;
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
    //TODO: rely on QObject hierarchy
}


int OWirelessNetworkInterface::channelHopping() const
{
    return _hopper->interval();
}


OChannelHopper* OWirelessNetworkInterface::channelHopper() const
{
    return _hopper;
}


void OWirelessNetworkInterface::setMode( const QString& mode )
{
    if ( mode == "auto" )            _iwr.u.mode = IW_MODE_AUTO;
    else if ( mode == "adhoc" )      _iwr.u.mode = IW_MODE_ADHOC;
    else if ( mode == "managed" )    _iwr.u.mode = IW_MODE_INFRA;
    else if ( mode == "master" )     _iwr.u.mode = IW_MODE_MASTER;
    else if ( mode == "repeater" )   _iwr.u.mode = IW_MODE_REPEAT;
    else if ( mode == "secondary" )  _iwr.u.mode = IW_MODE_SECOND;
    else if ( mode == "monitor" )    _iwr.u.mode = IW_MODE_MONITOR;
    else
    {
        qDebug( "ONetwork: Warning! Invalid IEEE 802.11 mode '%s' specified.", (const char*) mode );
        return;
    }
    wioctl( SIOCSIWMODE );
}


QString OWirelessNetworkInterface::mode() const
{
    if ( !wioctl( SIOCGIWMODE ) )
    {
        return "<unknown>";
    }
    switch ( _iwr.u.mode )
    {
        case IW_MODE_AUTO: return "auto";
        case IW_MODE_ADHOC: return "adhoc";
        case IW_MODE_INFRA: return "managed";
        case IW_MODE_MASTER: return "master";
        case IW_MODE_REPEAT: return "repeater";
        case IW_MODE_SECOND: return "secondary";
        case IW_MODE_MONITOR: return "monitor";
        default: assert( 0 ); // shouldn't happen
    }
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
    qDebug( "dataLinkType = %d", dataLinkType() );
    return ( dataLinkType() == ARPHRD_IEEE80211 || dataLinkType() == 802 );
    //FIXME: 802 is the header type for PRISM - Linux support for this is pending...
    //FIXME: What is 119, by the way?
}


void OWirelessNetworkInterface::setNickName( const QString& nickname )
{
    _iwr.u.essid.pointer = const_cast<char*>( (const char*) nickname );
    _iwr.u.essid.length = nickname.length();
    wioctl( SIOCSIWNICKN );
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


void OWirelessNetworkInterface::setPrivate( const QString& call, int numargs, ... )
{
    OPrivateIOCTL* priv = static_cast<OPrivateIOCTL*>( child( (const char*) call ) );
    if ( !priv )
    {
        qDebug( "OWirelessNetworkInterface::setPrivate(): interface '%s' does not support private ioctl '%s'", name(), (const char*) call );
        return;
    }
    if ( priv->numberSetArgs() != numargs )
    {
        qDebug( "OWirelessNetworkInterface::setPrivate(): parameter count not matching. '%s' expects %d arguments, but got %d", (const char*) call, priv->numberSetArgs(), numargs );
        return;
    }

    qDebug( "OWirelessNetworkInterface::setPrivate(): about to call '%s' on interface '%s'", (const char*) call, name() );
    memset( &_iwr, 0, sizeof _iwr );
    va_list argp;
    va_start( argp, numargs );
    for ( int i = 0; i < numargs; ++i )
    {
        priv->setParameter( i, va_arg( argp, int ) );
    }
    va_end( argp );
    priv->invoke();
}


void OWirelessNetworkInterface::getPrivate( const QString& call )
{
    qWarning( "OWirelessNetworkInterface::getPrivate() is not implemented yet." );
}


bool OWirelessNetworkInterface::hasPrivate( const QString& call )
{
    return child( (const char*) call );
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


int OWirelessNetworkInterface::scanNetwork()
{
    _iwr.u.param.flags = IW_SCAN_DEFAULT;
    _iwr.u.param.value = 0;
    if ( !wioctl( SIOCSIWSCAN ) )
    {
        return -1;
    }

    int timeout = 1000000;

    qDebug( "ONetworkInterface::scanNetwork() - scan started." );

    bool results = false;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 250000; // initial timeout ~ 250ms
    char buffer[IW_SCAN_MAX_DATA];

    while ( !results && timeout > 0 )
    {
        timeout -= tv.tv_usec;
        select( 0, 0, 0, 0, &tv );

        _iwr.u.data.pointer = &buffer[0];
        _iwr.u.data.flags = 0;
        _iwr.u.data.length = sizeof buffer;
        if ( wioctl( SIOCGIWSCAN ) )
        {
            results = true;
            continue;
        }
        else if ( errno == EAGAIN)
        {
            qDebug( "ONetworkInterface::scanNetwork() - scan in progress..." );
            #if 0
            if ( qApp )
            {
                qApp->processEvents( 100 );
                continue;
            }
            #endif
            tv.tv_sec = 0;
            tv.tv_usec = 100000;
            continue;
        }
    }

    qDebug( "ONetworkInterface::scanNetwork() - scan finished." );

    if ( results )
    {
        qDebug( " - result length = %d", _iwr.u.data.length );
        if ( !_iwr.u.data.length )
        {
            qDebug( " - no results (empty neighbourhood)" );
            return 0;
        }

        qDebug( " - results are in!" );
        dumpBytes( (const unsigned char*) &buffer[0], _iwr.u.data.length );

        int stations = 0;

        // parse results

        int offset = 0;
        struct iw_event* we = (struct iw_event*) &buffer[0];

        while ( offset < _iwr.u.data.length )
        {
            //const char* cmd = *(*_ioctlmap)[we->cmd];
            //if ( !cmd ) cmd = "<unknown>";
            qDebug( "reading next event... cmd=%d, len=%d", we->cmd, we->len );
            switch (we->cmd)
            {
                case SIOCGIWAP: qDebug( "SIOCGIWAP" ); stations++; break;
                case SIOCGIWMODE: qDebug( "SIOCGIWMODE" ); break;
                case SIOCGIWFREQ: qDebug( "SIOCGIWFREQ" ); break;
                case SIOCGIWESSID: qDebug( "SIOCGIWESSID" ); break;
                case SIOCGIWSENS: qDebug( "SIOCGIWSENS" ); break;
                case SIOCGIWENCODE: qDebug( "SIOCGIWENCODE" ); break;
                case IWEVTXDROP: qDebug( "IWEVTXDROP" ); break;         /* Packet dropped to excessive retry */
                case IWEVQUAL: qDebug( "IWEVQUAL" ); break;             /* Quality part of statistics (scan) */
                case IWEVCUSTOM: qDebug( "IWEVCUSTOM" ); break;         /* Driver specific ascii string */
                case IWEVREGISTERED: qDebug( "IWEVREGISTERED" ); break; /* Discovered a new node (AP mode) */
                case IWEVEXPIRED: qDebug( "IWEVEXPIRED" ); break;       /* Expired a node (AP mode) */
                default: qDebug( "unhandled event" );
            }

            offset += we->len;
            we = (struct iw_event*) &buffer[offset];
        }

    }
    else
    {
        qDebug( " - no results (timeout) :(" );
        return 0;
    }
}


bool OWirelessNetworkInterface::wioctl( int call, struct iwreq& iwreq ) const
{
    #ifndef NODEBUG
    int result = ::ioctl( _sfd, call, &iwreq );
    if ( result == -1 )
        qDebug( "ONetworkInterface::wioctl (%s) call %s (0x%04X) - Status: Failed: %d (%s)", name(), (const char*) *(*_ioctlmap)[call], call, result, strerror( errno ) );
    else
        qDebug( "ONetworkInterface::wioctl (%s) call %s (0x%04X) - Status: Ok.", name(), (const char*) *(*_ioctlmap)[call], call );
    return ( result != -1 );
    #else
    return ::ioctl( _sfd, call, &iwreq ) != -1;
    #endif
}


bool OWirelessNetworkInterface::wioctl( int call ) const
{
    strcpy( _iwr.ifr_name, name() );
    return wioctl( call, _iwr );
}


/*======================================================================================
 * OMonitoringInterface
 *======================================================================================*/

OMonitoringInterface::OMonitoringInterface( ONetworkInterface* iface, bool prismHeader )
                      :_if( static_cast<OWirelessNetworkInterface*>( iface ) ), _prismHeader( prismHeader )
{
}


OMonitoringInterface::~OMonitoringInterface()
{
}


void OMonitoringInterface::setChannel( int c )
{
    // use standard WE channel switching protocol
    memset( &_if->_iwr, 0, sizeof( struct iwreq ) );
    _if->_iwr.u.freq.m = c;
    _if->_iwr.u.freq.e = 0;
    _if->wioctl( SIOCSIWFREQ );
}


bool OMonitoringInterface::enabled() const
{
    return _if->monitorMode();
}


void OMonitoringInterface::setEnabled( bool b )
{
}


/*======================================================================================
 * OCiscoMonitoringInterface
 *======================================================================================*/

OCiscoMonitoringInterface::OCiscoMonitoringInterface( ONetworkInterface* iface, bool prismHeader )
                           :OMonitoringInterface( iface, prismHeader )
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


OWlanNGMonitoringInterface::OWlanNGMonitoringInterface( ONetworkInterface* iface, bool prismHeader )
                           :OMonitoringInterface( iface, prismHeader )
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
    QString prism = _prismHeader ? "true" : "false";
    QString cmd;
    cmd.sprintf( "$(which wlanctl-ng) %s lnxreq_wlansniff channel=%d enable=%s prismheader=%s",
                 (const char*) _if->name(), 1, (const char*) enable, (const char*) prism );
    system( cmd );
}


QString OWlanNGMonitoringInterface::name() const
{
    return "wlan-ng";
}


void OWlanNGMonitoringInterface::setChannel( int c )
{
    //NOTE: Older wlan-ng drivers automatically hopped channels while lnxreq_wlansniff=true. Newer ones don't.

    QString enable = "true"; //_if->monitorMode() ? "true" : "false";
    QString prism = _prismHeader ? "true" : "false";
    QString cmd;
    cmd.sprintf( "$(which wlanctl-ng) %s lnxreq_wlansniff channel=%d enable=%s prismheader=%s",
                 (const char*) _if->name(), c, (const char*) enable, (const char*) prism );
    system( cmd );
}


/*======================================================================================
 * OHostAPMonitoringInterface
 *======================================================================================*/

OHostAPMonitoringInterface::OHostAPMonitoringInterface( ONetworkInterface* iface, bool prismHeader )
                           :OMonitoringInterface( iface, prismHeader )
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
    if ( b )
        _if->setMode( "monitor" );  // IW_MODE_MONITOR doesn't support prism header
    else
        _if->setMode( "managed" );
    #else
    int monitorCode = _prismHeader ? 1 : 2;
    if ( b )
    {
        _if->setPrivate( "monitor", 1, monitorCode );
    }
    else
    {
        _if->setPrivate( "monitor", 1, 0 );
    }
    #endif
}


QString OHostAPMonitoringInterface::name() const
{
    return "hostap";
}


/*======================================================================================
 * OOrinocoNetworkInterface
 *======================================================================================*/

OOrinocoMonitoringInterface::OOrinocoMonitoringInterface( ONetworkInterface* iface, bool prismHeader )
                           :OMonitoringInterface( iface, prismHeader )
{
    iface->setMonitoring( this );
}


OOrinocoMonitoringInterface::~OOrinocoMonitoringInterface()
{
}


void OOrinocoMonitoringInterface::setChannel( int c )
{
    int monitorCode = _prismHeader ? 1 : 2;
    _if->setPrivate( "monitor", 2, monitorCode, c );
}


void OOrinocoMonitoringInterface::setEnabled( bool b )
{
    // IW_MODE_MONITOR was introduced in Wireless Extensions Version 15
    // Wireless Extensions < Version 15 need iwpriv commandos for monitoring
    // However, as of recent orinoco drivers, IW_MODE_MONITOR is still not supported

    #if 0
    //#if WIRELESS_EXT > 14
    if ( b )
        _if->setMode( "monitor" );  // IW_MODE_MONITOR doesn't support prism header
    else
        _if->setMode( "managed" );
    #else
    if ( b )
    {
        setChannel( 1 );
    }
    else
    {
        _if->setPrivate( "monitor", 2, 0, 0 );
    }
    #endif
}


QString OOrinocoMonitoringInterface::name() const
{
    return "orinoco";
}
