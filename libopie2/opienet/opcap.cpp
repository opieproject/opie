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
#include <opie2/opcap.h>
#include <opie2/odebug.h>

/* QT */
#include <qapplication.h> // don't use oapplication here (will decrease reusability in other projects)
#include <qsocketnotifier.h>
#include <qobjectlist.h>

/* SYSTEM */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/* LOCAL */
#include "udp_ports.h"

/*======================================================================================
 * OPacket
 *======================================================================================*/

OPacket::OPacket( int datalink, packetheaderstruct header, const unsigned char* data, QObject* parent )
        :QObject( parent, "Generic" ), _hdr( header ), _data( data )
{
    //qDebug( "OPacket::OPacket(): (Len %d, CapLen %d)" /*, ctime((const time_t*) header.ts.tv_sec)*/, header.len, header.caplen );

    _end = (unsigned char*) data + header.len;
    //qDebug( "OPacket::data @ %0x, end @ %0x", data, _end );

    switch ( datalink )
    {
        case DLT_EN10MB:
            odebug << "OPacket::OPacket(): Received Packet. Datalink = ETHERNET" << oendl;
            new OEthernetPacket( _end, (const struct ether_header*) data, this );
            break;

        case DLT_IEEE802_11:
            odebug << "OPacket::OPacket(): Received Packet. Datalink = IEEE802.11" << oendl;
            new OWaveLanPacket( _end, (const struct ieee_802_11_header*) data, this );
            break;

        case DLT_PRISM_HEADER:
            odebug << "OPacket::OPacket(): Received Packet. Datalink = PRISM_HEADER" << oendl;
            new OPrismHeaderPacket( _end, (const struct prism_hdr*) (unsigned char*) data, this );
            break;

        default:
            owarn << "OPacket::OPacket(): Received Packet over unsupported datalink, type " << datalink << "!" << oendl;
    }
}


OPacket::~OPacket()
{
}


timevalstruct OPacket::timeval() const
{
    return _hdr.ts;
}


int OPacket::caplen() const
{
    return _hdr.caplen;
}


void OPacket::updateStats( QMap<QString,int>& stats, QObjectList* l )
{
    if (!l) return;
    QObject* o = l->first();
    while ( o )
    {
        stats[o->name()]++;
        updateStats( stats, const_cast<QObjectList*>( o->children() ) );
        o = l->next();
    }
}


QString OPacket::dumpStructure() const
{
    return "[ |" + _dumpStructure(  const_cast<QObjectList*>( this->children() ) ) + " ]";
}


QString OPacket::_dumpStructure( QObjectList* l ) const
{
    if (!l) return QString::null;
    QObject* o = l->first();
    QString str(" ");

    while ( o )
    {
        str.append( o->name() );
        str.append( " |" );
        str += _dumpStructure( const_cast<QObjectList*>( o->children() ) );
        o = l->next();
    }
    return str;
}

QString OPacket::dump( int bpl ) const
{
    static int index = 0;
    index++;
    int len = _hdr.caplen;
    QString str;
    str.sprintf( "\n<----- Packet #%04d Len = 0x%X (%d) ----->\n\n", index, len, len );
    str.append( "0000: " );
    QString tmp;
    QString bytes;
    QString chars;

    for ( int i = 0; i < len; ++i )
    {
        tmp.sprintf( "%02X ", _data[i] ); bytes.append( tmp );
        if ( (_data[i] > 31) && (_data[i]<128) ) chars.append( _data[i] );
        else chars.append( '.' );

        if ( !((i+1) % bpl) )
        {
            str.append( bytes );
            str.append( ' ' );
            str.append( chars );
            str.append( '\n' );
            tmp.sprintf( "%04X: ", i+1 ); str.append( tmp );
            bytes = "";
            chars = "";
        }

    }
    if ( (len % bpl) )
    {
        str.append( bytes.leftJustify( 1 + 3*bpl ) );
        str.append( chars );
    }
    str.append( '\n' );
    return str;
}


int OPacket::len() const
{
    return _hdr.len;
}


QTextStream& operator<<( QTextStream& s, const OPacket& p )
{
    s << p.dumpStructure();
}


/*======================================================================================
 * OEthernetPacket
 *======================================================================================*/

OEthernetPacket::OEthernetPacket( const unsigned char* end, const struct ether_header* data, QObject* parent )
                :QObject( parent, "Ethernet" ), _ether( data )
{

    odebug << "Source = " << sourceAddress().toString();
    odebug << "Destination = " << destinationAddress().toString();

    if ( sourceAddress() == OMacAddress::broadcast )
        odebug << "Source is broadcast address" << oendl;
    if ( destinationAddress() == OMacAddress::broadcast )
        odebug << "Destination is broadcast address" << oendl;

    switch ( type() )
    {
        case ETHERTYPE_IP: new OIPPacket( end, (const struct iphdr*) (data+1), this ); break;
        case ETHERTYPE_ARP: new OARPPacket( end, (const struct myarphdr*) (data+1), this ); break;
        case ETHERTYPE_REVARP: { odebug << "OPacket::OPacket(): Received Ethernet Packet : Type = RARP" << oendl; break; }
        default: odebug << "OPacket::OPacket(): Received Ethernet Packet : Type = UNKNOWN" << oendl;
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


OIPPacket::OIPPacket( const unsigned char* end, const struct iphdr* data, QObject* parent )
          :QObject( parent, "IP" ), _iphdr( data )
{
    odebug << "OIPPacket::OIPPacket(): decoding IP header..." << oendl;

    odebug << "FromAddress = " << fromIPAddress().toString() << oendl;
    odebug << "  toAddress = " << toIPAddress().toString() << oendl;

    switch ( protocol() )
    {
        case IPPROTO_UDP: new OUDPPacket( end, (const struct udphdr*) (data+1), this ); break;
        case IPPROTO_TCP: new OTCPPacket( end, (const struct tcphdr*) (data+1), this ); break;
        default: odebug << "OIPPacket::OIPPacket(): unknown IP protocol, type = " << protocol() << oendl;
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
 * OARPPacket
 *======================================================================================*/


OARPPacket::OARPPacket( const unsigned char* end, const struct myarphdr* data, QObject* parent )
           :QObject( parent, "ARP" ), _arphdr( data )
{
    odebug << "OARPPacket::OARPPacket(): decoding ARP header..." << oendl;
    odebug << "ARP type seems to be " << EXTRACT_16BITS( &_arphdr->ar_op ) << " = " << type() << oendl;
    odebug << "Sender: MAC " << senderMacAddress().toString() << " = IP " << senderIPV4Address().toString() << oendl;
    odebug << "Target: MAC " << targetMacAddress().toString() << " = IP " << targetIPV4Address().toString() << oendl;
}


OARPPacket::~OARPPacket()
{
}


QString OARPPacket::type() const
{
    switch ( EXTRACT_16BITS( &_arphdr->ar_op ) )
    {
        case 1: return "REQUEST";
        case 2: return "REPLY";
        case 3: return "RREQUEST";
        case 4: return "RREPLY";
        case 8: return "InREQUEST";
        case 9: return "InREPLY";
        case 10: return "NAK";
        default: owarn << "OARPPacket::type(): invalid ARP type!" << oendl; return "<unknown>";
    }
}


QHostAddress OARPPacket::senderIPV4Address() const
{
    return EXTRACT_32BITS( &_arphdr->ar_sip );
}


QHostAddress OARPPacket::targetIPV4Address() const
{
    return EXTRACT_32BITS( &_arphdr->ar_tip );
}


OMacAddress OARPPacket::senderMacAddress() const
{
    return OMacAddress( _arphdr->ar_sha );
}


OMacAddress OARPPacket::targetMacAddress() const
{
    return OMacAddress( _arphdr->ar_tha );
}


/*======================================================================================
 * OUDPPacket
 *======================================================================================*/


OUDPPacket::OUDPPacket( const unsigned char* end, const struct udphdr* data, QObject* parent )
           :QObject( parent, "UDP" ), _udphdr( data )

{
    odebug << "OUDPPacket::OUDPPacket(): decoding UDP header..." << oendl;
    odebug << "fromPort = " << fromPort() << oendl;
    odebug << "  toPort = " << toPort() << oendl;

    // TODO: Make this a case or a hash if we know more udp protocols

    if ( fromPort() == UDP_PORT_BOOTPS || fromPort() == UDP_PORT_BOOTPC ||
         toPort() == UDP_PORT_BOOTPS || toPort() == UDP_PORT_BOOTPC )
    {
        odebug << "seems to be part of a DHCP conversation => creating DHCP packet." << oendl;
        new ODHCPPacket( end, (const struct dhcp_packet*) (data+1), this );
    }
}


OUDPPacket::~OUDPPacket()
{
}


int OUDPPacket::fromPort() const
{
    return EXTRACT_16BITS( &_udphdr->source );
}


int OUDPPacket::toPort() const
{
    return EXTRACT_16BITS( &_udphdr->dest );
}


int OUDPPacket::length() const
{
    return EXTRACT_16BITS( &_udphdr->len );
}


int OUDPPacket::checksum() const
{
    return EXTRACT_16BITS( &_udphdr->check );
}


/*======================================================================================
 * ODHCPPacket
 *======================================================================================*/


ODHCPPacket::ODHCPPacket( const unsigned char* end, const struct dhcp_packet* data, QObject* parent )
           :QObject( parent, "DHCP" ), _dhcphdr( data )

{
    odebug << "ODHCPPacket::ODHCPPacket(): decoding DHCP information..." << oendl;
    odebug << "DHCP opcode seems to be " << _dhcphdr->op << ": " << ( isRequest() ? "REQUEST" : "REPLY" ) << oendl;
    odebug << "clientAddress = " << clientAddress().toString() << oendl;
    odebug << "  yourAddress = " << yourAddress().toString() << oendl;
    odebug << "serverAddress = " << serverAddress().toString() << oendl;
    odebug << " relayAddress = " << relayAddress().toString() << oendl;
    odebug << "parsing DHCP options..." << oendl;

    _type = 0;

    const unsigned char* option = &_dhcphdr->options[4];
    char tag = -1;
    char len = -1;

    while ( ( tag = *option++ ) != -1 /* end of option field */ )
    {
        len = *option++;
        odebug << "recognized DHCP option #" << tag << ", length " << len << oendl;

        if ( tag == DHO_DHCP_MESSAGE_TYPE )
            _type = *option;

        option += len;
        if ( option >= end )
        {
            owarn << "DHCP parsing ERROR: sanity check says the packet is at its end!" << oendl;
            break;
        }
    }

    odebug << "DHCP type seems to be << " << type() << oendl;
}


ODHCPPacket::~ODHCPPacket()
{
}


bool ODHCPPacket::isRequest() const
{
    return ( _dhcphdr->op == 01 );
}


bool ODHCPPacket::isReply() const
{
    return ( _dhcphdr->op == 02 );
}


QString ODHCPPacket::type() const
{
    switch ( _type )
    {
        case 1: return "DISCOVER";
        case 2: return "OFFER";
        case 3: return "REQUEST";
        case 4: return "DECLINE";
        case 5: return "ACK";
        case 6: return "NAK";
        case 7: return "RELEASE";
        case 8: return "INFORM";
        default: owarn << "ODHCPPacket::type(): invalid DHCP type " << _dhcphdr->op << oendl; return "<unknown>";
    }
}


QHostAddress ODHCPPacket::clientAddress() const
{
    return EXTRACT_32BITS( &_dhcphdr->ciaddr );
}


QHostAddress ODHCPPacket::yourAddress() const
{
    return EXTRACT_32BITS( &_dhcphdr->yiaddr );
}


QHostAddress ODHCPPacket::serverAddress() const
{
    return EXTRACT_32BITS( &_dhcphdr->siaddr );
}


QHostAddress ODHCPPacket::relayAddress() const
{
    return EXTRACT_32BITS( &_dhcphdr->giaddr );
}


OMacAddress ODHCPPacket::clientMacAddress() const
{
    return OMacAddress( _dhcphdr->chaddr );
}


/*======================================================================================
 * OTCPPacket
 *======================================================================================*/


OTCPPacket::OTCPPacket( const unsigned char* end, const struct tcphdr* data, QObject* parent )
           :QObject( parent, "TCP" ), _tcphdr( data )

{
    odebug << "OTCPPacket::OTCPPacket(): decoding TCP header..." << oendl;
}


OTCPPacket::~OTCPPacket()
{
}


int OTCPPacket::fromPort() const
{
    return EXTRACT_16BITS( &_tcphdr->source );
}


int OTCPPacket::toPort() const
{
    return EXTRACT_16BITS( &_tcphdr->dest );
}


int OTCPPacket::seq() const
{
    return EXTRACT_16BITS( &_tcphdr->seq );
}


int OTCPPacket::ack() const
{
    return EXTRACT_16BITS( &_tcphdr->ack_seq );
}


int OTCPPacket::window() const
{
    return EXTRACT_16BITS( &_tcphdr->window );
}


int OTCPPacket::checksum() const
{
    return EXTRACT_16BITS( &_tcphdr->check );
}

/*======================================================================================
 * OPrismHeaderPacket
 *======================================================================================*/


OPrismHeaderPacket::OPrismHeaderPacket( const unsigned char* end, const struct prism_hdr* data, QObject* parent )
                :QObject( parent, "Prism" ), _header( data )

{
    odebug << "OPrismHeaderPacket::OPrismHeaderPacket(): decoding PRISM header..." << oendl;

    odebug << "Signal Strength = " << data->signal.data << oendl;

    new OWaveLanPacket( end, (const struct ieee_802_11_header*) (data+1), this );
}

OPrismHeaderPacket::~OPrismHeaderPacket()
{
}


unsigned int OPrismHeaderPacket::signalStrength() const
{
    return _header->signal.data;
}

/*======================================================================================
 * OWaveLanPacket
 *======================================================================================*/


OWaveLanPacket::OWaveLanPacket( const unsigned char* end, const struct ieee_802_11_header* data, QObject* parent )
                :QObject( parent, "802.11" ), _wlanhdr( data )

{
    odebug << "OWaveLanPacket::OWaveLanPacket(): decoding IEEE 802.11 header..." << oendl;
    odebug << "type = " << type() << oendl;
    odebug << "subType = " << subType() << oendl;
    odebug << "duration = " << duration() << oendl;
    odebug << "powermanagement = " << usesPowerManagement() << oendl;
    odebug << "payload is encrypted = " << ( usesWep() ? "yes" : "no" ) << oendl;
    odebug << "MAC1 = " << macAddress1().toString() << oendl;
    odebug << "MAC2 = " << macAddress2().toString() << oendl;
    odebug << "MAC3 = " << macAddress3().toString() << oendl;
    odebug << "MAC4 = " << macAddress4().toString() << oendl;

    switch ( type() )
    {
        case T_MGMT: new OWaveLanManagementPacket( end, (const struct ieee_802_11_mgmt_header*) data, this ); break;
        case T_DATA: new OWaveLanDataPacket( end, (const struct ieee_802_11_data_header*) data, this ); break;
        case T_CTRL: new OWaveLanControlPacket( end, (const struct ieee_802_11_control_header*) data, this ); break;
        default: odebug << "OWaveLanPacket::OWaveLanPacket(): Warning: Unknown major type = " << type() << oendl;
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

OWaveLanManagementPacket::OWaveLanManagementPacket( const unsigned char* end, const struct ieee_802_11_mgmt_header* data, OWaveLanPacket* parent )
                :QObject( parent, "802.11 Management" ), _header( data ),
                _body( (const struct ieee_802_11_mgmt_body*) (data+1) )
{
    odebug << "OWaveLanManagementPacket::OWaveLanManagementPacket(): decoding frame..." << oendl;
    odebug << "Detected subtype is " << managementType() << oendl;

    // Grab tagged values.
    // Beacons contain a 12 byte long fixed parameters set before the tagged parameters come,
    // Other management frames don't - which is why we have to inspect the subtype here.

    const unsigned char* ptr = managementType() == "Beacon" ?  (const unsigned char*) (_body+1) : (const unsigned char*) (_header+1);

    while (ptr < end)
    {
        switch ( *ptr )
        {
            case E_SSID: new OWaveLanManagementSSID( end, (struct ssid_t*) ptr, this ); break;
            case E_FH: new OWaveLanManagementFH( end, (struct fh_t*) ptr, this ); break;
            case E_DS: new OWaveLanManagementDS( end, (struct ds_t*) ptr, this ); break;
            case E_RATES: new OWaveLanManagementRates( end, (struct rates_t*) ptr, this ); break;
            case E_CF: new OWaveLanManagementCF( end, (struct cf_t*) ptr, this ); break;
            case E_TIM: new OWaveLanManagementTim( end, (struct tim_t*) ptr, this ); break;
            case E_IBSS: new OWaveLanManagementIBSS( end, (struct ibss_t*) ptr, this ); break;
            case E_CHALLENGE: new OWaveLanManagementChallenge( end, (struct challenge_t*) ptr, this ); break;
        }
        ptr+= ( ( struct ssid_t* ) ptr )->length; // skip length of tagged value
        ptr+= 2; // skip tag ID and length
    }
}


OWaveLanManagementPacket::~OWaveLanManagementPacket()
{
}


QString OWaveLanManagementPacket::managementType() const
{
    switch ( FC_SUBTYPE( EXTRACT_LE_16BITS( &_header->fc ) ) )
    {
        case ST_ASSOC_REQUEST: return "AssociationRequest"; break;
        case ST_ASSOC_RESPONSE: return "AssociationResponse"; break;
        case ST_REASSOC_REQUEST: return "ReassociationRequest"; break;
        case ST_REASSOC_RESPONSE: return "ReassociationResponse"; break;
        case ST_PROBE_REQUEST: return "ProbeRequest"; break;
        case ST_PROBE_RESPONSE: return "ProbeResponse"; break;
        case ST_BEACON: return "Beacon"; break;
        case ST_ATIM: return "Atim"; break;
        case ST_DISASSOC: return "Disassociation"; break;
        case ST_AUTH: return "Authentication"; break;
        case ST_DEAUTH: return "Deathentication"; break;
        default: owarn << "OWaveLanManagementPacket::managementType(): unhandled subtype " << FC_SUBTYPE( EXTRACT_LE_16BITS( &_header->fc ) ) << oendl; return "Unknown";
    }
}


int OWaveLanManagementPacket::beaconInterval() const
{
    return EXTRACT_LE_16BITS( &_body->beacon_interval );
}


int OWaveLanManagementPacket::capabilities() const
{
    return EXTRACT_LE_16BITS( &_body->capability_info );
}


bool OWaveLanManagementPacket::canESS() const
{
    return CAPABILITY_ESS( EXTRACT_LE_16BITS( &_body->capability_info ) );
}


bool OWaveLanManagementPacket::canIBSS() const
{
    return CAPABILITY_IBSS( EXTRACT_LE_16BITS( &_body->capability_info ) );
}


bool OWaveLanManagementPacket::canCFP() const
{
    return CAPABILITY_CFP( EXTRACT_LE_16BITS( &_body->capability_info ) );
}


bool OWaveLanManagementPacket::canCFP_REQ() const
{
    return CAPABILITY_CFP_REQ( EXTRACT_LE_16BITS( &_body->capability_info ) );
}


bool OWaveLanManagementPacket::canPrivacy() const
{
    return CAPABILITY_PRIVACY( EXTRACT_LE_16BITS( &_body->capability_info ) );
}


/*======================================================================================
 * OWaveLanManagementSSID
 *======================================================================================*/

OWaveLanManagementSSID::OWaveLanManagementSSID( const unsigned char* end, const struct ssid_t* data, QObject* parent )
                :QObject( parent, "802.11 SSID" ), _data( data )
{
    odebug << "OWaveLanManagementSSID()" << oendl;
}


OWaveLanManagementSSID::~OWaveLanManagementSSID()
{
}


QString OWaveLanManagementSSID::ID( bool decloak ) const
{
    int length = _data->length;
    if ( length > 32 ) length = 32;
    char essid[length+1];
    memcpy( &essid, &_data->ssid, length );
    essid[length] = 0x0;
    if ( !decloak || length < 2 || essid[0] != '\0' ) return essid;
    odebug << "OWaveLanManagementSSID:ID(): SSID is cloaked - decloaking..." << oendl;

    QString decloakedID;
    for ( int i = 1; i < length; ++i )
    {
        if ( essid[i] >= 32 && essid[i] <= 126 ) decloakedID.append( essid[i] );
        else decloakedID.append( '.' );
    }
    return decloakedID;
}


/*======================================================================================
 * OWaveLanManagementRates
 *======================================================================================*/

OWaveLanManagementRates::OWaveLanManagementRates( const unsigned char* end, const struct rates_t* data, QObject* parent )
                :QObject( parent, "802.11 Rates" ), _data( data )
{
    odebug << "OWaveLanManagementRates()" << oendl;
}


OWaveLanManagementRates::~OWaveLanManagementRates()
{
}

/*======================================================================================
 * OWaveLanManagementCF
 *======================================================================================*/

OWaveLanManagementCF::OWaveLanManagementCF( const unsigned char* end, const struct cf_t* data, QObject* parent )
                :QObject( parent, "802.11 CF" ), _data( data )
{
    odebug << "OWaveLanManagementCF()" << oendl;
}


OWaveLanManagementCF::~OWaveLanManagementCF()
{
}

/*======================================================================================
 * OWaveLanManagementFH
 *======================================================================================*/

OWaveLanManagementFH::OWaveLanManagementFH( const unsigned char* end, const struct fh_t* data, QObject* parent )
                :QObject( parent, "802.11 FH" ), _data( data )
{
    odebug << "OWaveLanManagementFH()" << oendl;
}


OWaveLanManagementFH::~OWaveLanManagementFH()
{
}

/*======================================================================================
 * OWaveLanManagementDS
 *======================================================================================*/

OWaveLanManagementDS::OWaveLanManagementDS( const unsigned char* end, const struct ds_t* data, QObject* parent )
                :QObject( parent, "802.11 DS" ), _data( data )
{
    odebug << "OWaveLanManagementDS()" << oendl;
}


OWaveLanManagementDS::~OWaveLanManagementDS()
{
}


int OWaveLanManagementDS::channel() const
{
    return _data->channel;
}

/*======================================================================================
 * OWaveLanManagementTim
 *======================================================================================*/

OWaveLanManagementTim::OWaveLanManagementTim( const unsigned char* end, const struct tim_t* data, QObject* parent )
                :QObject( parent, "802.11 Tim" ), _data( data )
{
    odebug << "OWaveLanManagementTim()" << oendl;
}


OWaveLanManagementTim::~OWaveLanManagementTim()
{
}

/*======================================================================================
 * OWaveLanManagementIBSS
 *======================================================================================*/

OWaveLanManagementIBSS::OWaveLanManagementIBSS( const unsigned char* end, const struct ibss_t* data, QObject* parent )
                :QObject( parent, "802.11 IBSS" ), _data( data )
{
    odebug << "OWaveLanManagementIBSS()" << oendl;
}


OWaveLanManagementIBSS::~OWaveLanManagementIBSS()
{
}

/*======================================================================================
 * OWaveLanManagementChallenge
 *======================================================================================*/

OWaveLanManagementChallenge::OWaveLanManagementChallenge( const unsigned char* end, const struct challenge_t* data, QObject* parent )
                :QObject( parent, "802.11 Challenge" ), _data( data )
{
    odebug << "OWaveLanManagementChallenge()" << oendl;
}


OWaveLanManagementChallenge::~OWaveLanManagementChallenge()
{
}

/*======================================================================================
 * OWaveLanDataPacket
 *======================================================================================*/

OWaveLanDataPacket::OWaveLanDataPacket( const unsigned char* end, const struct ieee_802_11_data_header* data, OWaveLanPacket* parent )
                :QObject( parent, "802.11 Data" ), _header( data )
{
    odebug << "OWaveLanDataPacket::OWaveLanDataPacket(): decoding frame..." << oendl;

    const unsigned char* payload = (const unsigned char*) data + sizeof( struct ieee_802_11_data_header );

    #warning The next line works for most cases, but can not be correct generally!
    if (!( ( (OWaveLanPacket*) this->parent())->duration() )) payload -= 6; // compensation for missing last address

    new OLLCPacket( end, (const struct ieee_802_11_802_2_header*) payload, this );
}


OWaveLanDataPacket::~OWaveLanDataPacket()
{
}


/*======================================================================================
 * OLLCPacket
 *======================================================================================*/

OLLCPacket::OLLCPacket( const unsigned char* end, const struct ieee_802_11_802_2_header* data, QObject* parent )
           :QObject( parent, "802.11 LLC" ), _header( data )
{
    odebug << "OLLCPacket::OLLCPacket(): decoding frame..." << oendl;

    if ( !(_header->oui[0] || _header->oui[1] || _header->oui[2]) )
    {
        owarn << "OLLCPacket::OLLCPacket(): contains an encapsulated Ethernet frame (type = " << EXTRACT_16BITS( &_header->type ) << ")" << oendl;

        switch ( EXTRACT_16BITS( &_header->type ) ) // defined in linux/if_ether.h
        {
            case ETH_P_IP: new OIPPacket( end, (const struct iphdr*) (data+1), this ); break;
            case ETH_P_ARP: new OARPPacket( end, (const struct myarphdr*) (data+1), this ); break;
            default: owarn << "OLLCPacket::OLLCPacket(): Unknown Encapsulation type = " << EXTRACT_16BITS( &_header->type ) << oendl;
        }
    }
}


OLLCPacket::~OLLCPacket()
{
}


/*======================================================================================
 * OWaveLanControlPacket
 *======================================================================================*/

OWaveLanControlPacket::OWaveLanControlPacket( const unsigned char* end, const struct ieee_802_11_control_header* data, OWaveLanPacket* parent )
                      :QObject( parent, "802.11 Control" ), _header( data )
{
    odebug << "OWaveLanControlPacket::OWaveLanDataControl(): decoding frame..." << oendl;
    odebug << "Detected subtype is " << controlType() << oendl;
}


OWaveLanControlPacket::~OWaveLanControlPacket()
{
}


QString OWaveLanControlPacket::controlType() const
{
    switch ( FC_SUBTYPE( EXTRACT_LE_16BITS( &_header->fc ) ) )
    {
        case CTRL_PS_POLL: return "PowerSavePoll"; break;
        case CTRL_RTS: return "RequestToSend"; break;
        case CTRL_CTS: return "ClearToSend"; break;
        case CTRL_ACK: return "Acknowledge"; break;
        case CTRL_CF_END: return "ContentionFreeEnd"; break;
        case CTRL_END_ACK: return "AcknowledgeEnd"; break;
        default:
            owarn << "OWaveLanControlPacket::managementType(): unhandled subtype " << FC_SUBTYPE( EXTRACT_LE_16BITS( &_header->fc ) ) << oendl;
            return "Unknown";
    }
}


/*======================================================================================
 * OPacketCapturer
 *======================================================================================*/

OPacketCapturer::OPacketCapturer( QObject* parent, const char* name )
                :QObject( parent, name ), _name( QString::null ), _open( false ), _pch( 0 ), _pcd( 0 ), _sn( 0 )
{
}


OPacketCapturer::~OPacketCapturer()
{
    if ( _open )
    {
        odebug << "OPacketCapturer::~OPacketCapturer(): pcap still open, autoclosing." << oendl;
        close();
    }
}


void OPacketCapturer::setBlocking( bool b )
{
    if ( pcap_setnonblock( _pch, 1-b, _errbuf ) != -1 )
    {
        odebug << "OPacketCapturer::setBlocking(): blocking mode changed successfully." << oendl;
    }
    else
    {
        odebug << "OPacketCapturer::setBlocking(): can't change blocking mode: " << _errbuf << oendl;
    }
}


bool OPacketCapturer::blocking() const
{
    int b = pcap_getnonblock( _pch, _errbuf );
    if ( b == -1 )
    {
        odebug << "OPacketCapturer::blocking(): can't get blocking mode: " << _errbuf << oendl;
        return -1;
    }
    return !b;
}


void OPacketCapturer::closeDumpFile()
{
    if ( _pcd )
    {
        pcap_dump_close( _pcd );
        _pcd = 0;
    }
    pcap_close( _pch );
}


void OPacketCapturer::close()
{
    if ( _open )
    {
        if ( _sn )
        {
            _sn->disconnect( SIGNAL( activated(int) ), this, SLOT( readyToReceive() ) );
            delete _sn;
        }
        closeDumpFile();
    _open = false;
    }

    odebug << "OPacketCapturer::close() --- dumping capturing statistics..." << oendl;
    odebug << "--------------------------------------------------" << oendl;
    for( QMap<QString,int>::Iterator it = _stats.begin(); it != _stats.end(); ++it )
        odebug << it.key() << " = " << it.data() << oendl;
    odebug << "--------------------------------------------------" << oendl;

}


int OPacketCapturer::dataLink() const
{
    return pcap_datalink( _pch );
}


void OPacketCapturer::dump( OPacket* p )
{
    if ( !_pcd )
    {
        owarn << "OPacketCapturer::dump() - cannot dump without open capture file!" << oendl;
        return;
    }
    pcap_dump( (u_char*) _pcd, &p->_hdr, p->_data );
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


OPacket* OPacketCapturer::next( int time )
{
    fd_set fds;
    struct timeval tv;
    FD_ZERO( &fds );
    FD_SET( pcap_fileno( _pch ), &fds );
    tv.tv_sec = time / 1000;
    tv.tv_usec = time % 1000;
    int retval = select( pcap_fileno( _pch )+1, &fds, NULL, NULL, &tv);
    if ( retval > 0 ) // clear to read!
        return next();
    else
        return 0;
}


OPacket* OPacketCapturer::next()
{
    packetheaderstruct header;
    odebug << "==> OPacketCapturer::next()" << oendl;
    const unsigned char* pdata = pcap_next( _pch, &header );
    odebug << "<== OPacketCapturer::next()" << oendl;

    if ( pdata && header.len )
    {
        OPacket* p = new OPacket( dataLink(), header, pdata, 0 );
        // packets shouldn't be inserted in the QObject child-parent hierarchy,
        // because due to memory constraints they will be deleted as soon
        // as possible - that is right after they have been processed
        // by emit() [ see below ]
        //TODO: make gathering statistics optional, because it takes time
        p->updateStats( _stats, const_cast<QObjectList*>( p->children() ) );
        odebug << "OPacket::dumpStructure: " << p->dumpStructure() << oendl;
        return p;
    }
    else
    {
        owarn << "OPacketCapturer::next() - no packet received!" << oendl;
        return 0;
    }
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

    // open libpcap
    pcap_t* handle = pcap_open_live( const_cast<char*>( (const char*) name ), 1024, 0, 0, &_errbuf[0] );

    if ( !handle )
    {
        owarn << "OPacketCapturer::open(): can't open libpcap with '" << name << "': " << _errbuf << oendl;
        return false;
    }

    odebug << "OPacketCapturer::open(): libpcap [" << name << "] opened successfully." << oendl;
    _pch = handle;
    _open = true;
    _stats.clear();

    // in case we have an application object, create a socket notifier
    if ( qApp ) //TODO: I don't like this here...
    {
        _sn = new QSocketNotifier( fileno(), QSocketNotifier::Read );
        connect( _sn, SIGNAL( activated(int) ), this, SLOT( readyToReceive() ) );
    }

    return true;
}


bool OPacketCapturer::openDumpFile( const QString& filename )
{
    pcap_dumper_t* dump = pcap_dump_open( _pch, const_cast<char*>( (const char*) filename ) );
    if ( !dump )
    {
        owarn << "OPacketCapturer::open(): can't open dump with '" << filename << "': " << _errbuf << oendl;
        return false;
    }
    odebug << "OPacketCapturer::open(): dump [" << filename << "] opened successfully." << oendl;
    _pcd = dump;

    return true;
}


bool OPacketCapturer::open( const QFile& file )
{
    QString name = file.name();

    if ( _open )
    {
        close();
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

    pcap_t* handle = pcap_open_offline( const_cast<char*>( (const char*) name ), &_errbuf[0] );

    if ( handle )
    {
        odebug << "OPacketCapturer::open(): libpcap opened successfully." << oendl;
        _pch = handle;
        _open = true;

        // in case we have an application object, create a socket notifier
        if ( qApp )
        {
            _sn = new QSocketNotifier( fileno(), QSocketNotifier::Read );
            connect( _sn, SIGNAL( activated(int) ), this, SLOT( readyToReceive() ) );
        }

        return true;
    }
    else
    {
        odebug << "OPacketCapturer::open(): can't open libpcap with '" << name << "': " << _errbuf << oendl;
        return false;
    }

}


bool OPacketCapturer::isOpen() const
{
    return _open;
}


void OPacketCapturer::readyToReceive()
{
    odebug << "OPacketCapturer::readyToReceive(): about to emit 'receivePacket(p)'" << oendl;
    OPacket* p = next();
    emit receivedPacket( p );
    // emit is synchronous - packet has been dealt with, now it's safe to delete
    delete p;
}


const QMap<QString,int>& OPacketCapturer::statistics() const
{
    return _stats;
}


int OPacketCapturer::snapShot() const
{
    return pcap_snapshot( _pch );
}


bool OPacketCapturer::swapped() const
{
    return pcap_is_swapped( _pch );
}


QString OPacketCapturer::version() const
{
    return QString().sprintf( "%d.%d", pcap_major_version( _pch ), pcap_minor_version( _pch ) );
}


