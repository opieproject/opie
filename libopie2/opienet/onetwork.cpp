/*
                             This file is part of the Opie Project
                             Copyright (C) 2003 by Michael 'Mickey' Lauer <mickey@Vanille.de>
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
#include <opie2/odebug.h>

/* QT */

#include <qfile.h>
#include <qtextstream.h>

/* UNIX */

#include <assert.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/sockios.h>
#include <net/if_arp.h>
#include <stdarg.h>

#ifndef NODEBUG
#include <opie2/odebugmapper.h>
DebugMapper* debugmapper = new DebugMapper();
#endif

/*======================================================================================
 * ONetwork
 *======================================================================================*/

ONetwork* ONetwork::_instance = 0;

ONetwork::ONetwork()
{
    odebug << "ONetwork::ONetwork()" << oendl;
    odebug << "ONetwork: This code has been compiled against Wireless Extensions V" << WIRELESS_EXT << oendl;
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
        odebug << "ONetwork: /proc/net/dev not existing. No network devices available" << oendl;
        return;
    }
    QTextStream s( &f );
    s.readLine();
    s.readLine();
    while ( !s.atEnd() )
    {
        s >> str;
        str.truncate( str.find( ':' ) );
        odebug << "ONetwork: found interface '" << str << "'" << oendl;
        ONetworkInterface* iface;
        if ( isWirelessInterface( str ) )
        {
            iface = new OWirelessNetworkInterface( this, (const char*) str );
            odebug << "ONetwork: interface '" << str << "' has Wireless Extensions" << oendl;
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
    odebug << "ONetworkInterface::ONetworkInterface()" << oendl;
    init();
}


struct ifreq& ONetworkInterface::ifr() const
{
    return _ifr;
}


void ONetworkInterface::init()
{
    odebug << "ONetworkInterface::init()" << oendl;

    memset( &_ifr, 0, sizeof( struct ifreq ) );

    if ( _sfd == -1 )
    {
        odebug << "ONetworkInterface::init(): Warning - can't get socket for device '" << name() << "'" << oendl;
        return;
    }
}


bool ONetworkInterface::ioctl( int call, struct ifreq& ifreq ) const
{
    #ifndef NODEBUG
    int result = ::ioctl( _sfd, call, &ifreq );
    if ( result == -1 )
        odebug << "ONetworkInterface::ioctl (" << name() << ") call '" << debugmapper->map( call )
               << "' FAILED! " << result << " (" << strerror( errno ) << ")" << oendl;
    else
        odebug << "ONetworkInterface::ioctl (" << name() << ") call '" << debugmapper->map( call )
               << "' - Status: Ok." << oendl;
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
    odebug << "ONetwork::setMonitoring(): Installed monitoring driver '" << m->name() << "' on interface '" << name() << "'" << oendl;
}


OMonitoringInterface* ONetworkInterface::monitoring() const
{
    return _mon;
}


ONetworkInterface::~ONetworkInterface()
{
    odebug << "ONetworkInterface::~ONetworkInterface()" << oendl;
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
    odebug << "OChannelHopper::timerEvent(): set channel " << *_channel << " on interface '" << _iface->name() << "'" << oendl;
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
    odebug << "OWirelessNetworkInterface::OWirelessNetworkInterface()" << oendl;
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
    odebug << "OWirelessNetworkInterface::init()" << oendl;
    memset( &_iwr, 0, sizeof( struct iwreq ) );
    buildInformation();
    buildPrivateList();
    dumpInformation();
}


bool OWirelessNetworkInterface::isAssociated() const
{
    //FIXME: handle different modes
    return !(associatedAP() == OMacAddress::unknown);
}


OMacAddress OWirelessNetworkInterface::associatedAP() const
{
    if ( ioctl( SIOCGIWAP ) )
        return (const unsigned char*) &_ifr.ifr_hwaddr.sa_data[0];
    else
        return OMacAddress::unknown;
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
        owarn << "OWirelessNetworkInterface::buildInformation(): Can't get channel information - using default values." << oendl;
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
            owarn << "OWirelessNetworkInterface::buildInformation(): Driver for wireless interface '" << name()
                  << "' sucks! It overwrote the buffer end with at least " << max - sizeof( struct iw_range ) << " bytes!" << oendl;
        }
        // </check if the driver overwrites stuff>

        struct iw_range range;
        memcpy( &range, buffer, sizeof range );

        odebug << "OWirelessNetworkInterface::buildInformation(): Interface reported to have " << (int) range.num_frequency << " channels." << oendl;
        for ( int i = 0; i < range.num_frequency; ++i )
        {
            int freq = (int) ( double( range.freq[i].m ) * pow( 10.0, range.freq[i].e ) / 1000000.0 );
            _channels.insert( freq, i+1 );
        }
    }

    memcpy( &_range, buffer, sizeof( struct iw_range ) );
    odebug << "OWirelessNetworkInterface::buildInformation(): Information block constructed." << oendl;
    free(buffer);
}


void OWirelessNetworkInterface::buildPrivateList()
{
    odebug << "OWirelessNetworkInterface::buildPrivateList()" << oendl;

    struct iw_priv_args priv[IW_MAX_PRIV_DEF];

    _iwr.u.data.pointer = (char*) &priv;
    _iwr.u.data.length = IW_MAX_PRIV_DEF; // length in terms of number of (sizeof iw_priv_args), not (sizeof iw_priv_args) itself
    _iwr.u.data.flags = 0;

    if ( !wioctl( SIOCGIWPRIV ) )
    {
        owarn << "OWirelessNetworkInterface::buildPrivateList(): Can't get private ioctl information." << oendl;
        return;
    }

    for ( int i = 0; i < _iwr.u.data.length; ++i )
    {
        new OPrivateIOCTL( this, priv[i].name, priv[i].cmd, priv[i].get_args, priv[i].set_args );
    }
    odebug << "OWirelessNetworkInterface::buildPrivateList(): Private ioctl list constructed." << oendl;
}


void OWirelessNetworkInterface::dumpInformation() const
{
    odebug << "OWirelessNetworkInterface::() -------------- dumping information block ----------------" << oendl;

    qDebug( " - driver's idea of maximum throughput is %d bps = %d byte/s = %d Kb/s = %f.2 Mb/s", _range.throughput, _range.throughput / 8, _range.throughput / 8 / 1024, float( _range.throughput ) / 8.0 / 1024.0 / 1024.0 );
    qDebug( " - driver for '%s' has been compiled against WE V%d (source=V%d)", name(), _range.we_version_compiled, _range.we_version_source );

    odebug << "OWirelessNetworkInterface::() ---------------------------------------------------------" << oendl;
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
        oerr << "OWirelessNetworkInterface::setChannel( 0 ) called - fix your application!" << oendl;
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


void OWirelessNetworkInterface::commit() const
{
    wioctl( SIOCSIWCOMMIT );
}


void OWirelessNetworkInterface::setMode( const QString& newMode )
{
    #ifdef FINALIZE
    QString currentMode = mode();
    if ( currentMode == newMode ) return;
    #endif

    odebug << "OWirelessNetworkInterface::setMode(): trying to set mode " << newMode << oendl;

    _iwr.u.mode = stringToMode( newMode );

    if ( _iwr.u.mode != IW_MODE_MONITOR )
    {
        // IWR.U.MODE WIRD DURCH ABFRAGE DES MODE HIER PLATTGEMACHT!!!!!!!!!!!!!!!!!!!!! DEPP!
        _iwr.u.mode = stringToMode( newMode );
        wioctl( SIOCSIWMODE );

        // special iwpriv fallback for monitor mode (check if we're really out of monitor mode now)

        if ( mode() == "monitor" )
        {
            odebug << "OWirelessNetworkInterface::setMode(): SIOCSIWMODE not sufficient - trying fallback to iwpriv..." << oendl;
            if ( _mon )
                _mon->setEnabled( false );
            else
                odebug << "ONetwork(): can't switch monitor mode without installed monitoring interface" << oendl;
        }

    }
    else    // special iwpriv fallback for monitor mode
    {
        if ( wioctl( SIOCSIWMODE ) )
        {
            odebug << "OWirelessNetworkInterface::setMode(): IW_MODE_MONITOR ok" << oendl;
        }
        else
        {
            odebug << "OWirelessNetworkInterface::setMode(): SIOCSIWMODE not working - trying fallback to iwpriv..." << oendl;

            if ( _mon )
                _mon->setEnabled( true );
            else
                odebug << "ONetwork(): can't switch monitor mode without installed monitoring interface" << oendl;
        }
    }
}


QString OWirelessNetworkInterface::mode() const
{
    memset( &_iwr, 0, sizeof( struct iwreq ) );

    if ( !wioctl( SIOCGIWMODE ) )
    {
        return "<unknown>";
    }

    odebug << "OWirelessNetworkInterface::setMode(): WE's idea of current mode seems to be " << modeToString( _iwr.u.mode ) << oendl;

    // legacy compatible monitor mode check

    if ( dataLinkType() == ARPHRD_IEEE80211 || dataLinkType() == 802 )
    {
        return "monitor";
    }
    else
    {
        return modeToString( _iwr.u.mode );
    }
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
        owarn << "OWirelessNetworkInterface::setPrivate(): interface '" << name()
              << "' does not support private ioctl '" << call << "'" << oendl;
        return;
    }
    if ( priv->numberSetArgs() != numargs )
    {
        owarn << "OWirelessNetworkInterface::setPrivate(): parameter count not matching. '"
              << call << "' expects " << priv->numberSetArgs() << ", but got " << numargs << oendl;
        return;
    }

    odebug << "OWirelessNetworkInterface::setPrivate(): about to call '" << call << "' on interface '" << name() << "'" << oendl;
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
    oerr << "OWirelessNetworkInterface::getPrivate() is not implemented yet." << oendl;
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


OStationList* OWirelessNetworkInterface::scanNetwork()
{
    _iwr.u.param.flags = IW_SCAN_DEFAULT;
    _iwr.u.param.value = 0;
    if ( !wioctl( SIOCSIWSCAN ) )
    {
        return 0;
    }

    OStationList* stations = new OStationList();

    int timeout = 1000000;

    odebug << "ONetworkInterface::scanNetwork() - scan started." << oendl;

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
            odebug << "ONetworkInterface::scanNetwork() - scan in progress..." << oendl;
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

    odebug << "ONetworkInterface::scanNetwork() - scan finished." << oendl;

    if ( results )
    {
        odebug << " - result length = " << _iwr.u.data.length << oendl;
        if ( !_iwr.u.data.length )
        {
            odebug << " - no results (empty neighbourhood)" << oendl;
            return stations;
        }

        odebug << " - results are in!" << oendl;
        dumpBytes( (const unsigned char*) &buffer[0], _iwr.u.data.length );

        // parse results

        int offset = 0;
        struct iw_event* we = (struct iw_event*) &buffer[0];

        while ( offset < _iwr.u.data.length )
        {
            //const char* cmd = *(*_ioctlmap)[we->cmd];
            //if ( !cmd ) cmd = "<unknown>";
            odebug << " - reading next event... cmd=" << we->cmd << ", len=" << we->len << oendl;
            switch (we->cmd)
            {
                case SIOCGIWAP:
                {
                    odebug << "SIOCGIWAP" << oendl;
                    stations->append( new OStation() );
                    stations->last()->macAddress = (const unsigned char*) &we->u.ap_addr.sa_data[0];
                    break;
                }
                case SIOCGIWMODE:
                {
                    odebug << "SIOCGIWMODE" << oendl;
                    stations->last()->type = modeToString( we->u.mode );
                    break;
                }
                case SIOCGIWFREQ:
                {
                    odebug << "SIOCGIWFREQ" << oendl;
                    stations->last()->channel = _channels[ static_cast<int>(double( we->u.freq.m ) * pow( 10.0, we->u.freq.e ) / 1000000) ];
                    break;
                }
                case SIOCGIWESSID:
                {
                    odebug << "SIOCGIWESSID" << oendl;
                    stations->last()->ssid = we->u.essid.pointer;
                    break;
                }
                case SIOCGIWSENS: odebug << "SIOCGIWSENS" << oendl; break;
                case SIOCGIWENCODE: odebug << "SIOCGIWENCODE" << oendl; break;
                case IWEVTXDROP: odebug << "IWEVTXDROP" << oendl; break;         /* Packet dropped to excessive retry */
                case IWEVQUAL: odebug << "IWEVQUAL" << oendl; break;             /* Quality part of statistics (scan) */
                case IWEVCUSTOM: odebug << "IWEVCUSTOM" << oendl; break;         /* Driver specific ascii string */
                case IWEVREGISTERED: odebug << "IWEVREGISTERED" << oendl; break; /* Discovered a new node (AP mode) */
                case IWEVEXPIRED: odebug << "IWEVEXPIRED" << oendl; break;       /* Expired a node (AP mode) */
                default: odebug << "unhandled event" << oendl;
            }

            offset += we->len;
            we = (struct iw_event*) &buffer[offset];
        }
        return stations;

        return stations;

    }
    else
    {
        odebug << " - no results (timeout) :(" << oendl;
        return stations;
    }
}


bool OWirelessNetworkInterface::wioctl( int call, struct iwreq& iwreq ) const
{
    #ifndef NODEBUG
    int result = ::ioctl( _sfd, call, &iwreq );

    if ( result == -1 )
        odebug << "ONetworkInterface::wioctl (" << name() << ") call '"
               << debugmapper->map( call ) << "' FAILED! " << result << " (" << strerror( errno ) << ")" << oendl;
    else
        odebug << "ONetworkInterface::wioctl (" << name() << ") call '"
               << debugmapper->map( call ) << "' - Status: Ok." << oendl;

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
    int monitorCode = _prismHeader ? 1 : 2;
    if ( b )
    {
        _if->setPrivate( "monitor", 1, monitorCode );
    }
    else
    {
        _if->setPrivate( "monitor", 1, 0 );
    }
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

    if ( b )
    {
        setChannel( 1 );
    }
    else
    {
        _if->setPrivate( "monitor", 2, 0, 0 );
    }
}


QString OOrinocoMonitoringInterface::name() const
{
    return "orinoco";
}
