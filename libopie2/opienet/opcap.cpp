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

#include <opie2/opcap.h>

/* QT */

#include <qapplication.h> // don't use oapplication here (will decrease reusability in other projects)
#include <qsocketnotifier.h>

/*======================================================================================
 * OPacket
 *======================================================================================*/

OPacket::OPacket( packetheaderstruct header, const unsigned char* data, QObject* parent )
        :QObject( parent, "Generic" ), _hdr( header ), _data( data )
{
    qDebug( "OPacket::OPacket(): (Len %d, CapLen %d)" /*, ctime((const time_t*) header.ts.tv_sec)*/, header.len, header.caplen );

    if ( packetCapturer()->dataLink() == DLT_EN10MB )
    {
        qDebug( "OPacket::OPacket(): Received Packet. Datalink = ETHERNET" );
        new OEthernetPacket( (const struct ether_header*) data, this );
    }
    else
    {
        qDebug( "OPacket::OPacket(): Received Packet. Datalink = IEEE802.11" );
        new OWaveLanPacket( (const struct ieee_802_11_header*) data, this );
    }
}


OPacket::~OPacket()
{
}


OPacketCapturer* OPacket::packetCapturer() const
{
    return parent()->inherits( "OPacketCapturer" ) ? static_cast<OPacketCapturer*>( parent() ) : 0;
}


timevalstruct OPacket::timeval() const
{
    return _hdr.ts;
}


int OPacket::caplen() const
{
    return _hdr.caplen;
}


void OPacket::dump() const
{
    printf( "OPacket::dump()\n" );
    printf( "----------------\n" );

    for ( int i = 0; i < _hdr.caplen; ++i )
    {
        printf( "%02x ", _data[i] );
        if ( !((i+1) % 32) ) printf( "\n" );
    }
    printf( "\n\n" );
}



int OPacket::len() const
{
    return _hdr.len;
}

/*======================================================================================
 * OEthernetPacket
 *======================================================================================*/

OEthernetPacket::OEthernetPacket( const struct ether_header* data, QObject* parent )
                :QObject( parent, "Ethernet" ), _ether( data )

{

    qDebug( "Source = %s", (const char*) sourceAddress().toString() );
    qDebug( "Destination = %s", (const char*) destinationAddress().toString() );

    if ( sourceAddress() == OMacAddress::broadcast )
        qDebug( "Source is broadcast address" );
    if ( destinationAddress() == OMacAddress::broadcast )
        qDebug( "Destination is broadcast address" );

    switch ( type() )
    {
        case ETHERTYPE_IP: new OIPPacket( (const struct iphdr*) (data+1), this ); break;
        case ETHERTYPE_ARP: { qDebug( "OPacket::OPacket(): Received Ethernet Packet : Type = ARP" ); break; }
        case ETHERTYPE_REVARP: { qDebug( "OPacket::OPacket(): Received Ethernet Packet : Type = RARP" ); break; }
        default: qDebug( "OPacket::OPacket(): Received Ethernet Packet : Type = UNKNOWN" );
    }

}


OEthernetPacket::~OEthernetPacket()
{
}


OMacAddress OEthernetPacket::sourceAddress() const
{
    return OMacAddress( _ether->ether_shost );
}


OMacAddress OEthernetPacket::destinationAddress() const
{
    return OMacAddress( _ether->ether_dhost );
}

int OEthernetPacket::type() const
{
    return ntohs( _ether->ether_type );
}


/*======================================================================================
 * OIPPacket
 *======================================================================================*/


OIPPacket::OIPPacket( const struct iphdr* data, QObject* parent )
          :QObject( parent, "IP" ), _iphdr( data )

{
    qDebug( "OIPPacket::OIPPacket(): decoding IP header..." );

    //qDebug( "FromAddress: %s", (const char*) inet_ntoa( *src ) );
    //qDebug( "  ToAddress: %s", (const char*) inet_ntoa( *dst ) );

    qDebug( "FromAddress: %s", (const char*) fromIPAddress().toString() );
    qDebug( "  toAddress: %s", (const char*) toIPAddress().toString() );

    switch ( protocol() )
    {
        case IPPROTO_UDP: new OUDPPacket( (const struct udphdr*) (data+1), this ); break;
        case IPPROTO_TCP: new OTCPPacket( (const struct tcphdr*) (data+1), this ); break;
        default: qDebug( "OIPPacket::OIPPacket(): unknown IP protocol type = %d", protocol() );
    }

}

OIPPacket::~OIPPacket()
{
}


QHostAddress OIPPacket::fromIPAddress() const
{
    return EXTRACT_32BITS( &_iphdr->saddr );
}


QHostAddress OIPPacket::toIPAddress() const
{
    return EXTRACT_32BITS( &_iphdr->saddr );
}


int OIPPacket::tos() const
{
    return _iphdr->tos;
}


int OIPPacket::len() const
{
    return EXTRACT_16BITS( &_iphdr->tot_len );
}


int OIPPacket::id() const
{
    return EXTRACT_16BITS( &_iphdr->id );
}


int OIPPacket::offset() const
{
    return EXTRACT_16BITS( &_iphdr->frag_off );
}


int OIPPacket::ttl() const
{
    return _iphdr->ttl;
}


int OIPPacket::protocol() const
{
    return _iphdr->protocol;
}


int OIPPacket::checksum() const
{
    return EXTRACT_16BITS( &_iphdr->check );
}

/*======================================================================================
 * OUDPPacket
 *======================================================================================*/


OUDPPacket::OUDPPacket( const struct udphdr* data, QObject* parent )
           :QObject( parent, "UDP" ), _udphdr( data )

{
    qDebug( "OUDPPacket::OUDPPacket(): decoding UDP header..." );
}

OUDPPacket::~OUDPPacket()
{
}


/*======================================================================================
 * OTCPPacket
 *======================================================================================*/


OTCPPacket::OTCPPacket( const struct tcphdr* data, QObject* parent )
           :QObject( parent, "TCP" ), _tcphdr( data )

{
    qDebug( "OTCPPacket::OTCPPacket(): decoding TCP header..." );
}

OTCPPacket::~OTCPPacket()
{
}


/*======================================================================================
 * OWaveLanPacket
 *======================================================================================*/


OWaveLanPacket::OWaveLanPacket( const struct ieee_802_11_header* data, QObject* parent )
                :QObject( parent, "802.11" ), _wlanhdr( data )

{
    qDebug( "OWaveLanPacket::OWaveLanPacket(): decoding IEEE 802.11 header..." );
    qDebug( "type: %0X", type() );
    qDebug( "subType: %0X", subType() );
    qDebug( "duration: %d", duration() );
    qDebug( "powermanagement: %d", usesPowerManagement() );
    qDebug( "wep: %d", usesWep() );
    qDebug( "MAC1: %s", (const char*) macAddress1().toString() );
    qDebug( "MAC2: %s", (const char*) macAddress2().toString() );
    qDebug( "MAC3: %s", (const char*) macAddress3().toString() );
    qDebug( "MAC4: %s", (const char*) macAddress4().toString() );

    switch ( type() )
    {
        case T_MGMT: new OWaveLanManagementPacket( (const struct ieee_802_11_mgmt_header*) data, this ); break;
        case T_DATA: new OWaveLanDataPacket( (const struct ieee_802_11_data_header*) data, this ); break;
        //case T_CTRL: new OWaveLanControlPacket( (const struct ieee_802_11_ctrl_header*) data, this ); break;
        default: qDebug( "OWaveLanPacket::OWaveLanPacket(): Warning: Unknown type!" );
    }
}

OWaveLanPacket::~OWaveLanPacket()
{
}


int OWaveLanPacket::duration() const
{
    return _wlanhdr->duration;
}


OMacAddress OWaveLanPacket::macAddress1() const
{
    return OMacAddress( _wlanhdr->mac1 );
}


OMacAddress OWaveLanPacket::macAddress2() const
{
    return OMacAddress( _wlanhdr->mac2 );
}


OMacAddress OWaveLanPacket::macAddress3() const
{
    return OMacAddress( _wlanhdr->mac3 );
}


OMacAddress OWaveLanPacket::macAddress4() const
{
    return OMacAddress( _wlanhdr->mac4 );
}


int OWaveLanPacket::subType() const
{
    return FC_SUBTYPE( EXTRACT_LE_16BITS( &_wlanhdr->frame_control ) );
}


int OWaveLanPacket::type() const
{
    return FC_TYPE( EXTRACT_LE_16BITS( &_wlanhdr->frame_control ) );
}


int OWaveLanPacket::version() const
{
    return FC_VERSION( EXTRACT_LE_16BITS( &_wlanhdr->frame_control ) );
}


bool OWaveLanPacket::fromDS() const
{
    return FC_FROM_DS( EXTRACT_LE_16BITS( &_wlanhdr->frame_control ) );
}


bool OWaveLanPacket::toDS() const
{
    return FC_TO_DS( EXTRACT_LE_16BITS( &_wlanhdr->frame_control ) );
}


bool OWaveLanPacket::usesPowerManagement() const
{
    return FC_POWER_MGMT( EXTRACT_LE_16BITS( &_wlanhdr->frame_control ) );
}


bool OWaveLanPacket::usesWep() const
{
    return FC_WEP( EXTRACT_LE_16BITS( &_wlanhdr->frame_control ) );
}


/*======================================================================================
 * OWaveLanManagementPacket
 *======================================================================================*/

OWaveLanManagementPacket::OWaveLanManagementPacket( const struct ieee_802_11_mgmt_header* data, OWaveLanPacket* parent )
                :QObject( parent, "802.11 Management" ), _header( data ),
                _body( (const struct ieee_802_11_mgmt_body*) (data+1) )
{
    qDebug( "OWaveLanManagementPacket::OWaveLanManagementPacket(): decoding frame..." );

    switch ( ((OWaveLanPacket*) this->parent() )->subType() )
    {
        case ST_BEACON:
        {
            qDebug( "TYPE: BEACON FRAME" );
            qDebug( "ESSID: %s", (const char*) SSID() );
            break;
        }
    }
}


OWaveLanManagementPacket::~OWaveLanManagementPacket()
{
}


QString OWaveLanManagementPacket::SSID() const
{
    int length = _body->ssid.length;
    if ( length > 32 ) length = 32;
    char essid[length+1];
    memcpy( &essid, _body->ssid.ssid, length );
    essid[length] = 0x0;
    return essid;
}


/*======================================================================================
 * OWaveLanDataPacket
 *======================================================================================*/

OWaveLanDataPacket::OWaveLanDataPacket( const struct ieee_802_11_data_header* data, OWaveLanPacket* parent )
                :QObject( parent, "802.11 Data" ), _header( data )
{
    //qDebug( "size of header = %d", sizeof( struct ieee_802_11_data_header ) );
    //qDebug( "header: %0x", data );
    const unsigned char* payload = (const unsigned char*) data + sizeof( struct ieee_802_11_data_header );
    //qDebug( "payload: %0x", payload );

    if (!( ( (OWaveLanPacket*) this->parent())->duration() )) payload -= 6; // compensation for missing last address

    new OLLCPacket( (const struct ieee_802_11_802_2_header*) payload, this );
}


OWaveLanDataPacket::~OWaveLanDataPacket()
{
}


/*======================================================================================
 * OLLCPacket
 *======================================================================================*/

OLLCPacket::OLLCPacket( const struct ieee_802_11_802_2_header* data, QObject* parent )
                :QObject( parent, "802.11 802_2" ), _header( data )
{
    qDebug( "OLLCPacket::OLLCPacket(): decoding frame..." );

    if ( !(_header->oui[0] || _header->oui[1] || _header->oui[2]) )
    {
        qDebug( "OLLCPacket::OLLCPacket(): contains an encapsulated Ethernet frame (type=%04X)", EXTRACT_16BITS( &_header->type ) );

        switch ( EXTRACT_16BITS( &_header->type ) ) // defined in linux/if_ether.h
        {
            case ETH_P_IP: new OIPPacket( (const struct iphdr*) (data+1), this ); break;
            default: qDebug( "OLLCPacket::OLLCPacket(): Unknown Encapsulation Type" );
        }

    }
}


OLLCPacket::~OLLCPacket()
{
}

/*======================================================================================
 * OPacketCapturer
 *======================================================================================*/

OPacketCapturer::OPacketCapturer( QObject* parent, const char* name )
                 :QObject( parent, name ), _name( QString::null ), _open( false ),
                 _pch( 0 )
{
}


OPacketCapturer::~OPacketCapturer()
{
    if ( _open )
    {
        qDebug( "OPacketCapturer::~OPacketCapturer(): pcap still open, autoclosing." );
        close();
    }
}


void OPacketCapturer::setBlocking( bool b )
{
    if ( pcap_setnonblock( _pch, 1-b, _errbuf ) != -1 )
    {
        qDebug( "OPacketCapturer::setBlocking(): blocking mode changed successfully." );
    }
    else
    {
        qDebug( "OPacketCapturer::setBlocking(): can't change blocking mode: %s", _errbuf );
    }
}


bool OPacketCapturer::blocking() const
{
    int b = pcap_getnonblock( _pch, _errbuf );
    if ( b == -1 )
    {
        qDebug( "OPacketCapturer::blocking(): can't get blocking mode: %s", _errbuf );
        return -1;
    }
    return !b;
}


void OPacketCapturer::close()
{
    if ( _open )
    {
        pcap_close( _pch );
        _open = false;
    }
}


int OPacketCapturer::dataLink() const
{
    return pcap_datalink( _pch );
}


int OPacketCapturer::fileno() const
{
    if ( _open )
    {
        return pcap_fileno( _pch );
    }
    else
    {
        return -1;
    }
}


OPacket* OPacketCapturer::next()
{
    packetheaderstruct header;
    const unsigned char* pdata = pcap_next( _pch, &header );
    if ( header.len )
        return new OPacket( header, pdata, this );
    else
        return 0;
}


bool OPacketCapturer::open( const QString& name )
{
    if ( _open )
    {
        if ( name == _name )    // ignore opening an already openend device
        {
            return true;
        }
        else                    // close the last opened device
        {
            close();
        }
    }

    _name = name;

    pcap_t* handle = pcap_open_live( const_cast<char*>( (const char*) name ), 1024, 0, 0, &_errbuf[0] );

    if ( handle )
    {
        qDebug( "OPacketCapturer::open(): libpcap opened successfully." );
        _pch = handle;
        _open = true;

        // in case we have a qapp, create a socket notifier
        if ( qApp )
        {
            QSocketNotifier* sn = new QSocketNotifier( fileno(), QSocketNotifier::Read, this );
            connect( sn, SIGNAL( activated(int) ), this, SLOT( readyToReceive() ) );
        }

        return true;
    }
    else
    {
        qDebug( "OPacketCapturer::open(): can't open libpcap: %s", _errbuf );
        return false;
    }

}


bool OPacketCapturer::isOpen() const
{
    return _open;
}


void OPacketCapturer::readyToReceive()
{
    qDebug( "OPacketCapturer::readyToReceive(): about to emit 'receivePacket(...)'" );
    emit receivedPacket( next() );
}

