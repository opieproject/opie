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
#include <qobjectlist.h>

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
            qDebug( "OPacket::OPacket(): Received Packet. Datalink = ETHERNET" );
            new OEthernetPacket( _end, (const struct ether_header*) data, this );
            break;

        case DLT_IEEE802_11:
            qDebug( "OPacket::OPacket(): Received Packet. Datalink = IEEE802.11" );
            new OWaveLanPacket( _end, (const struct ieee_802_11_header*) data, this );
            break;

        case DLT_PRISM_HEADER:
            qDebug( "OPacket::OPacket(): Received Packet. Datalink = PRISM_HEADER" );
            new OPrismHeaderPacket( _end, (const struct prism_hdr*) (unsigned char*) data, this );
            break;

        default:
            qWarning( "OPacket::OPacket(): Received Packet over unsupported datalink (type %d)!", datalink );
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


/*======================================================================================
 * OEthernetPacket
 *======================================================================================*/

OEthernetPacket::OEthernetPacket( const unsigned char* end, const struct ether_header* data, QObject* parent )
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
        case ETHERTYPE_IP: new OIPPacket( end, (const struct iphdr*) (data+1), this ); break;
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


OIPPacket::OIPPacket( const unsigned char* end, const struct iphdr* data, QObject* parent )
          :QObject( parent, "IP" ), _iphdr( data )
{
    qDebug( "OIPPacket::OIPPacket(): decoding IP header..." );

    //qDebug( "FromAddress: %s", (const char*) inet_ntoa( *src ) );
    //qDebug( "  ToAddress: %s", (const char*) inet_ntoa( *dst ) );

    qDebug( "FromAddress: %s", (const char*) fromIPAddress().toString() );
    qDebug( "  toAddress: %s", (const char*) toIPAddress().toString() );

    switch ( protocol() )
    {
        case IPPROTO_UDP: new OUDPPacket( end, (const struct udphdr*) (data+1), this ); break;
        case IPPROTO_TCP: new OTCPPacket( end, (const struct tcphdr*) (data+1), this ); break;
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
 * OARPPacket
 *======================================================================================*/


OARPPacket::OARPPacket( const unsigned char* end, const struct myarphdr* data, QObject* parent )
           :QObject( parent, "ARP" ), _arphdr( data )
{
    qDebug( "OARPPacket::OARPPacket(): decoding ARP header..." );
    qDebug( "ARP type seems to be %02d - '%s'", EXTRACT_16BITS( &_arphdr->ar_op ), (const char*) type() );
    qDebug( "Sender: MAC %s = IP %s", (const char*) senderMacAddress().toString(), (const char*) senderIPV4Address().toString() );
    qDebug( "Target: MAC %s = IP %s", (const char*) targetMacAddress().toString(), (const char*) targetIPV4Address().toString() );
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
        default: qWarning( "OARPPacket::type(): invalid ARP type!" ); return "<unknown>";
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
    qDebug( "OUDPPacket::OUDPPacket(): decoding UDP header..." );
}

OUDPPacket::~OUDPPacket()
{
}


/*======================================================================================
 * OTCPPacket
 *======================================================================================*/


OTCPPacket::OTCPPacket( const unsigned char* end, const struct tcphdr* data, QObject* parent )
           :QObject( parent, "TCP" ), _tcphdr( data )

{
    qDebug( "OTCPPacket::OTCPPacket(): decoding TCP header..." );
}

OTCPPacket::~OTCPPacket()
{
}


/*======================================================================================
 * OPrismHeaderPacket
 *======================================================================================*/


OPrismHeaderPacket::OPrismHeaderPacket( const unsigned char* end, const struct prism_hdr* data, QObject* parent )
                :QObject( parent, "Prism" ), _header( data )

{
    qDebug( "OPrismHeaderPacket::OPrismHeaderPacket(): decoding PRISM header..." );

    qDebug( "Signal Strength = %d", data->signal.data );

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
    qDebug( "OWaveLanPacket::OWaveLanPacket(): decoding IEEE 802.11 header..." );
    qDebug( "type: %0X", type() );
    qDebug( "subType: %0X", subType() );
    qDebug( "duration: %d", duration() );
    qDebug( "powermanagement: %d", usesPowerManagement() );
    qDebug( "payload is encrypted: %s", usesWep() ? "yes" : "no" );
    qDebug( "MAC1: %s", (const char*) macAddress1().toString() );
    qDebug( "MAC2: %s", (const char*) macAddress2().toString() );
    qDebug( "MAC3: %s", (const char*) macAddress3().toString() );
    qDebug( "MAC4: %s", (const char*) macAddress4().toString() );

    switch ( type() )
    {
        case T_MGMT: new OWaveLanManagementPacket( end, (const struct ieee_802_11_mgmt_header*) data, this ); break;
        case T_DATA: new OWaveLanDataPacket( end, (const struct ieee_802_11_data_header*) data, this ); break;
        case T_CTRL: new OWaveLanControlPacket( end, (const struct ieee_802_11_control_header*) data, this ); break;
        default: qDebug( "OWaveLanPacket::OWaveLanPacket(): Warning: Unknown major type '%d'!", type() );
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
    qDebug( "OWaveLanManagementPacket::OWaveLanManagementPacket(): decoding frame..." );
    qDebug( "Detected subtype is '%s'", (const char*) managementType() );

    // grab tagged values
    const unsigned char* ptr = (const unsigned char*) (_body+1);
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
        default:
            qWarning( "OWaveLanManagementPacket::managementType(): unhandled subtype %d", FC_SUBTYPE( EXTRACT_LE_16BITS( &_header->fc ) ) );
            return "Unknown";
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
    qDebug( "OWaveLanManagementSSID()" );
}


OWaveLanManagementSSID::~OWaveLanManagementSSID()
{
}


QString OWaveLanManagementSSID::ID() const
{
    int length = _data->length;
    if ( length > 32 ) length = 32;
    char essid[length+1];
    memcpy( &essid, &_data->ssid, length );
    essid[length] = 0x0;
    return essid;
}


/*======================================================================================
 * OWaveLanManagementRates
 *======================================================================================*/

OWaveLanManagementRates::OWaveLanManagementRates( const unsigned char* end, const struct rates_t* data, QObject* parent )
                :QObject( parent, "802.11 Rates" ), _data( data )
{
    qDebug( "OWaveLanManagementRates()" );
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
    qDebug( "OWaveLanManagementCF()" );
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
    qDebug( "OWaveLanManagementFH()" );
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
    qDebug( "OWaveLanManagementDS()" );
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
    qDebug( "OWaveLanManagementTim()" );
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
    qDebug( "OWaveLanManagementIBSS()" );
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
    qDebug( "OWaveLanManagementChallenge()" );
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
    qDebug( "OWaveLanDataPacket::OWaveLanDataPacket(): decoding frame..." );

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
    qDebug( "OLLCPacket::OLLCPacket(): decoding frame..." );

    if ( !(_header->oui[0] || _header->oui[1] || _header->oui[2]) )
    {
        qDebug( "OLLCPacket::OLLCPacket(): contains an encapsulated Ethernet frame (type=%04X)", EXTRACT_16BITS( &_header->type ) );

        switch ( EXTRACT_16BITS( &_header->type ) ) // defined in linux/if_ether.h
        {
            case ETH_P_IP: new OIPPacket( end, (const struct iphdr*) (data+1), this ); break;
            case ETH_P_ARP: new OARPPacket( end, (const struct myarphdr*) (data+1), this ); break;
            default: qWarning( "OLLCPacket::OLLCPacket(): Unknown Encapsulation (type=%04X)", EXTRACT_16BITS( &_header->type ) );
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
                :QObject( parent, "802.11 Data" ), _header( data )
{
    qDebug( "OWaveLanControlPacket::OWaveLanDataControl(): decoding frame..." );
    //TODO: Implement this
}


OWaveLanControlPacket::~OWaveLanControlPacket()
{
}


/*======================================================================================
 * OPacketCapturer
 *======================================================================================*/

OPacketCapturer::OPacketCapturer( QObject* parent, const char* name )
                 :QObject( parent, name ), _name( QString::null ), _open( false ),
                 _pch( 0 ), _pcd( 0 ), _sn( 0 )
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
        if ( _sn )
        {
            _sn->disconnect( SIGNAL( activated(int) ), this, SLOT( readyToReceive() ) );
            delete _sn;
        }
        if ( _pcd )
        {
            pcap_dump_close( _pcd );
            _pcd = 0;
        }
        pcap_close( _pch );
        _open = false;
    }

    qDebug( "OPacketCapturer::close() --- dumping capturing statistics..." );
    qDebug( "--------------------------------------------------" );
    for( QMap<QString,int>::Iterator it = _stats.begin(); it != _stats.end(); ++it )
        qDebug( "%s : %d", (const char*) it.key(), it.data() );
    qDebug( "--------------------------------------------------" );

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
    qDebug( "==> OPacketCapturer::next()" );
    const unsigned char* pdata = pcap_next( _pch, &header );
    qDebug( "<== OPacketCapturer::next()" );
    if ( _pcd )
        pcap_dump( (u_char*) _pcd, &header, pdata );

    if ( pdata && header.len )
    {
        OPacket* p = new OPacket( dataLink(), header, pdata, 0 );
        // packets shouldn't be inserted in the QObject child-parent hierarchy,
        // because due to memory constraints they will be deleted as soon
        // as possible - that is right after they have been processed
        // by emit() [ see below ]
        //TODO: make gathering statistics optional, because it takes time
        p->updateStats( _stats, const_cast<QObjectList*>( p->children() ) );

        return p;
    }
    else
    {
        qWarning( "OPacketCapturer::next() - no packet received!" );
        return 0;
    }
}


bool OPacketCapturer::open( const QString& name, const QString& filename )
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
        qWarning( "OPacketCapturer::open(): can't open libpcap with '%s': %s", (const char*) name, _errbuf );
        return false;
    }

    qDebug( "OPacketCapturer::open(): libpcap [%s] opened successfully.", (const char*) name );
    _pch = handle;
    _open = true;
    _stats.clear();

    // in case we have an application object, create a socket notifier
    if ( qApp ) //TODO: I don't like this here...
    {
        _sn = new QSocketNotifier( fileno(), QSocketNotifier::Read );
        connect( _sn, SIGNAL( activated(int) ), this, SLOT( readyToReceive() ) );
    }

    // if requested, open a dump
    pcap_dumper_t* dump = pcap_dump_open( _pch, const_cast<char*>( (const char*) filename ) );
    if ( !dump )
    {
        qWarning( "OPacketCapturer::open(): can't open dump with '%s': %s", (const char*) filename, _errbuf );
        return false;
    }
    qDebug( "OPacketCapturer::open(): dump [%s] opened successfully.", (const char*) filename );
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
        qDebug( "OPacketCapturer::open(): libpcap opened successfully." );
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
        qDebug( "OPacketCapturer::open(): can't open libpcap with '%s': %s", (const char*) name, _errbuf );
        return false;
    }

}


bool OPacketCapturer::isOpen() const
{
    return _open;
}


void OPacketCapturer::readyToReceive()
{
    qDebug( "OPacketCapturer::readyToReceive(): about to emit 'receivePacket(p)'" );
    OPacket* p = next();
    emit receivedPacket( p );
    // emit is synchronous - packet has been dealt with, now it's safe to delete
    delete p;
}


const QMap<QString,int>& OPacketCapturer::statistics() const
{
    return _stats;
}

