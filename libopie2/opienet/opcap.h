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

#ifndef OPCAP_H
#define OPCAP_H

/* LINUX */
extern "C"  // work around a bpf/pcap conflict in recent headers
{
    #include <pcap.h>
}
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <time.h>

/* QT */
#include <qevent.h>
#include <qfile.h>
#include <qhostaddress.h>
#include <qobject.h>
#include <qstring.h>
#include <qmap.h>

/* OPIE */
#include <opie2/onetutils.h>

/* Custom Network Includes */
#include "802_11_user.h"
#include "dhcp.h"

/* TYPEDEFS */
typedef struct timeval timevalstruct;
typedef struct pcap_pkthdr packetheaderstruct;

/* FORWARDS */
class OPacketCapturer;
class QSocketNotifier;

/*======================================================================================
 * OPacket - A frame on the wire
 *======================================================================================*/

/** @brief A class representing a data frame on the wire.
 *
 * The whole family of the packet classes are used when capturing frames from a network.
 * Most standard network protocols in use share a common architecture, which mostly is
 * a packet header and then the packet payload. In layered architectures, each lower layer
 * encapsulates data from its upper layer - that is it
 * treats the data from its upper layer as payload and prepends an own header to the packet,
 * which - again - is treated as the payload for the layer below. The figure below is an
 * example for how such a data frame is composed out of packets, e.g. when sending a mail.
 *
 * <pre>
 *                                                       | User Data |     == Mail Data
 *                                         | SMTP Header | User Data |     == SMTP
 *                           | TCP Header  | SMTP Header | User Data |     == TCP
 *               | IP Header | TCP Header  | SMTP Header | User Data |     == IP
 *  | MAC Header | IP Header | TCP Header  | SMTP Header | User Data |     == MAC
 *
 * </pre>
 *
 * The example is trimmed for simplicity, because the MAC (Medium Access Control) layer
 * also contains a few more levels of encapsulation.
 * Since the type of the payload is more or less independent from the encapsulating protocol,
 * the header must be inspected before attempting to decode the payload. Hence, the
 * encapsulation level varies and can't be deduced without actually looking into the packets.
 *
 * For actually working with captured frames, it's useful to identify the packets via names and
 * insert them into a parent/child - relationship based on the encapsulation. This is why
 * all packet classes derive from QObject. The amount of overhead caused by the QObject is
 * not a problem in this case, because we're talking about a theoratical maximum of about
 * 10 packets per captured frame. We need to stuff them into a searchable list anyway and the
 * QObject also cares about destroying the sub-, (child-) packets.
 *
 * This enables us to perform a simple look for packets of a certain type:
 * @code
 * OPacketCapturer* pcap = new OPacketCapturer();
 * pcap->open( "eth0" );
 * OPacket* p = pcap->next();
 * OIPPacket* ip = (OIPPacket*) p->child( "IP" ); // returns 0, if no such child exists
 * odebug << "got ip packet from " << ip->fromIPAddress().toString() << " to " << ip->toIPAddress().toString() << oendl;
 *
 */

class OPacket : public QObject
{
  Q_OBJECT

  friend class OPacketCapturer;

  public:
    OPacket( int datalink, packetheaderstruct, const unsigned char*, QObject* parent );
    virtual ~OPacket();

    timevalstruct timeval() const;

    int caplen() const;
    int len() const;
    QString dump( int = 32 ) const;

    void updateStats( QMap<QString,int>&, QObjectList* );

  private:

    void dumpStructure( QObjectList* );
    QString _dumpStructure( QObjectList* );

  private:
    const packetheaderstruct _hdr;  // pcap packet header
    const unsigned char* _data;     // pcap packet data
    const unsigned char* _end;      // end of pcap packet data
};

/*======================================================================================
 * OEthernetPacket - DLT_EN10MB frame
 *======================================================================================*/

class OEthernetPacket : public QObject
{
  Q_OBJECT

  public:
    OEthernetPacket( const unsigned char*, const struct ether_header*, QObject* parent = 0 );
    virtual ~OEthernetPacket();

    OMacAddress sourceAddress() const;
    OMacAddress destinationAddress() const;
    int type() const;

  private:
    const struct ether_header* _ether;
};

/*======================================================================================
 * OPrismHeaderPacket - DLT_PRISM_HEADER frame
 *======================================================================================*/

class OPrismHeaderPacket : public QObject
{
  Q_OBJECT

  public:
    OPrismHeaderPacket( const unsigned char*, const struct prism_hdr*, QObject* parent = 0 );
    virtual ~OPrismHeaderPacket();

    unsigned int signalStrength() const;

  private:
    const struct prism_hdr* _header;
};

/*======================================================================================
 * OWaveLanPacket - DLT_IEEE802_11 frame
 *======================================================================================*/

class OWaveLanPacket : public QObject
{
  Q_OBJECT

  public:
    OWaveLanPacket( const unsigned char*, const struct ieee_802_11_header*, QObject* parent = 0 );
    virtual ~OWaveLanPacket();

    int duration() const;
    bool fromDS() const;
    bool toDS() const;
    virtual OMacAddress macAddress1() const;
    virtual OMacAddress macAddress2() const;
    virtual OMacAddress macAddress3() const;
    virtual OMacAddress macAddress4() const;
    bool usesPowerManagement() const;
    int type() const;
    int subType() const;
    int version() const;
    bool usesWep() const;

  private:
    const struct ieee_802_11_header* _wlanhdr;
};


/*======================================================================================
 * OWaveLanManagementPacket - type: management (T_MGMT)
 *======================================================================================*/

class OWaveLanManagementPacket : public QObject
{
  Q_OBJECT

  public:
    OWaveLanManagementPacket( const unsigned char*, const struct ieee_802_11_mgmt_header*, OWaveLanPacket* parent = 0 );
    virtual ~OWaveLanManagementPacket();

    QString managementType() const;

    int beaconInterval() const;
    int capabilities() const; // generic

    bool canESS() const;
    bool canIBSS() const;
    bool canCFP() const;
    bool canCFP_REQ() const;
    bool canPrivacy() const;

  private:
    const struct ieee_802_11_mgmt_header* _header;
    const struct ieee_802_11_mgmt_body* _body;
};


/*======================================================================================
 * OWaveLanManagementSSID
 *======================================================================================*/

class OWaveLanManagementSSID : public QObject
{
  Q_OBJECT

  public:
    OWaveLanManagementSSID( const unsigned char*, const struct ssid_t*, QObject* parent = 0 );
    virtual ~OWaveLanManagementSSID();

    QString ID( bool decloak = false ) const;

  private:
    const struct ssid_t* _data;
};

/*======================================================================================
 * OWaveLanManagementRates
 *======================================================================================*/

class OWaveLanManagementRates : public QObject
{
  Q_OBJECT

  public:
    OWaveLanManagementRates( const unsigned char*, const struct rates_t*, QObject* parent = 0 );
    virtual ~OWaveLanManagementRates();

  private:
    const struct rates_t* _data;
};

/*======================================================================================
 * OWaveLanManagementCF
 *======================================================================================*/

class OWaveLanManagementCF : public QObject
{
  Q_OBJECT

  public:
    OWaveLanManagementCF( const unsigned char*, const struct cf_t*, QObject* parent = 0 );
    virtual ~OWaveLanManagementCF();

  private:
    const struct cf_t* _data;
};

/*======================================================================================
 * OWaveLanManagementFH
 *======================================================================================*/

class OWaveLanManagementFH : public QObject
{
  Q_OBJECT

  public:
    OWaveLanManagementFH( const unsigned char*, const struct fh_t*, QObject* parent = 0 );
    virtual ~OWaveLanManagementFH();

  private:
    const struct fh_t* _data;
};

/*======================================================================================
 * OWaveLanManagementDS
 *======================================================================================*/

class OWaveLanManagementDS : public QObject
{
  Q_OBJECT

  public:
    OWaveLanManagementDS( const unsigned char*, const struct ds_t*, QObject* parent = 0 );
    virtual ~OWaveLanManagementDS();

    int channel() const;

  private:
    const struct ds_t* _data;
};

/*======================================================================================
 * OWaveLanManagementTim
 *======================================================================================*/

class OWaveLanManagementTim : public QObject
{
  Q_OBJECT

  public:
    OWaveLanManagementTim( const unsigned char*, const struct tim_t*, QObject* parent = 0 );
    virtual ~OWaveLanManagementTim();

  private:
    const struct tim_t* _data;
};

/*======================================================================================
 * OWaveLanManagementIBSS
 *======================================================================================*/

class OWaveLanManagementIBSS : public QObject
{
  Q_OBJECT

  public:
    OWaveLanManagementIBSS( const unsigned char*, const struct ibss_t*, QObject* parent = 0 );
    virtual ~OWaveLanManagementIBSS();

  private:
    const struct ibss_t* _data;
};

/*======================================================================================
 * OWaveLanManagementChallenge
 *======================================================================================*/

class OWaveLanManagementChallenge : public QObject
{
  Q_OBJECT

  public:
    OWaveLanManagementChallenge( const unsigned char*, const struct challenge_t*, QObject* parent = 0 );
    virtual ~OWaveLanManagementChallenge();

  private:
    const struct challenge_t* _data;
};

/*======================================================================================
 * OWaveLanDataPacket - type: data (T_DATA)
 *======================================================================================*/

class OWaveLanDataPacket : public QObject
{
  Q_OBJECT

  public:
    OWaveLanDataPacket( const unsigned char*, const struct ieee_802_11_data_header*, OWaveLanPacket* parent = 0 );
    virtual ~OWaveLanDataPacket();

  private:
    const struct ieee_802_11_data_header* _header;
};

/*======================================================================================
 * OWaveLanControlPacket - type: control (T_CTRL)
 *======================================================================================*/

class OWaveLanControlPacket : public QObject
{
  Q_OBJECT

  public:
    OWaveLanControlPacket( const unsigned char*, const struct ieee_802_11_control_header*, OWaveLanPacket* parent = 0 );
    virtual ~OWaveLanControlPacket();

    QString controlType() const;

  private:
    const struct ieee_802_11_control_header* _header;
};

/*======================================================================================
 * OLLCPacket - IEEE 802.2 Link Level Control
 *======================================================================================*/

class OLLCPacket : public QObject
{
  Q_OBJECT

  public:
    OLLCPacket( const unsigned char*, const struct ieee_802_11_802_2_header* data, QObject* parent = 0 );
    virtual ~OLLCPacket();

  private:
    const struct ieee_802_11_802_2_header* _header;
};

/*======================================================================================
 * OIPPacket
 *======================================================================================*/

class OIPPacket : public QObject
{
  Q_OBJECT

  public:
    OIPPacket( const unsigned char*, const struct iphdr*, QObject* parent = 0 );
    virtual ~OIPPacket();

    QHostAddress fromIPAddress() const;
    QHostAddress toIPAddress() const;

    int tos() const;
    int len() const;
    int id() const;
    int offset() const;
    int ttl() const;
    int protocol() const;
    int checksum() const;

  private:
    const struct iphdr* _iphdr;
};

/*======================================================================================
 * OARPPacket
 *======================================================================================*/

class OARPPacket : public QObject
{
  Q_OBJECT

  public:
    OARPPacket( const unsigned char*, const struct myarphdr*, QObject* parent = 0 );
    virtual ~OARPPacket();

    QHostAddress senderIPV4Address() const;
    OMacAddress senderMacAddress() const;
    QHostAddress targetIPV4Address() const;
    OMacAddress targetMacAddress() const;

    //int type() const;
    QString type() const;

  private:
    const struct myarphdr* _arphdr;
};

/*======================================================================================
 * OUDPPacket
 *======================================================================================*/

class OUDPPacket : public QObject
{
  Q_OBJECT

  public:
    OUDPPacket( const unsigned char*, const struct udphdr*, QObject* parent = 0 );
    virtual ~OUDPPacket();

    int fromPort() const;
    int toPort() const;
    int length() const;
    int checksum() const;

  private:
    const struct udphdr* _udphdr;
};

/*======================================================================================
 * ODHCPPacket
 *======================================================================================*/

class ODHCPPacket : public QObject
{
  Q_OBJECT

  public:
    ODHCPPacket( const unsigned char*, const struct dhcp_packet*, QObject* parent = 0 );
    virtual ~ODHCPPacket();

    QHostAddress clientAddress() const;
    QHostAddress yourAddress() const;
    QHostAddress serverAddress() const;
    QHostAddress relayAddress() const;

    OMacAddress clientMacAddress() const;

    bool isRequest() const;
    bool isReply() const;
    QString type() const;

  private:
    const struct dhcp_packet* _dhcphdr;
    unsigned char _type;
};

/*======================================================================================
 * OTCPPacket
 *======================================================================================*/

class OTCPPacket : public QObject
{
  Q_OBJECT

  public:
    OTCPPacket( const unsigned char*, const struct tcphdr*, QObject* parent = 0 );
    virtual ~OTCPPacket();

    int fromPort() const;
    int toPort() const;
    int seq() const;
    int ack() const;
    int window() const;
    int checksum() const;

  private:
    const struct tcphdr* _tcphdr;
};


/*======================================================================================
 * OPacketCapturer
 *======================================================================================*/

/**
 * @brief A class based wrapper for network packet capturing.
 *
 * This class is the base of a high-level interface to the well known packet capturing
 * library libpcap.
 * @see http://tcpdump.org
 */
class OPacketCapturer : public QObject
{
  Q_OBJECT

  public:
    /**
     * Constructor.
     */
    OPacketCapturer( QObject* parent = 0, const char* name = 0 );
    /**
     * Destructor.
     */
    ~OPacketCapturer();
    /**
     * Set the packet capturer to use blocking or non-blocking IO. This can be useful when
     * not using the socket notifier, e.g. without an application object.
     */
    void setBlocking( bool );
    /**
     * @returns true if the packet capturer uses blocking IO calls.
     */
    bool blocking() const;
    /**
     * Close the packet capturer. This is automatically done in the destructor.
     */
    void close();
    /**
     * Close the output capture file.
     */
    void closeDumpFile();
    /**
     * @returns the data link type.
     * @see <pcap.h> for possible values.
     */
    int dataLink() const;
    /**
     * Dump a packet to the output capture file.
     */
    void dump( OPacket* );
    /**
     * @returns the file descriptor of the packet capturer. This is only useful, if
     * not using the socket notifier, e.g. without an application object.
     */
    int fileno() const;
    /**
     * @returns the next @ref OPacket from the packet capturer.
     * @note If blocking mode is true then this call might block.
     */
    OPacket* next();
    /**
     * @returns the next @ref OPacket from the packet capturer, if
     * one arrives within @a time milliseconds.
     */
    OPacket* next( int time );
    /**
     * Open the packet capturer to capture packets in live-mode from @a interface.
     */
    bool open( const QString& interface );
    /**
     * Open the packet capturer to capture packets in offline-mode from @a file.
     */
    bool open( const QFile& file );
    /**
     * Open a prerecorded tcpdump compatible capture file for use with @ref dump()
     */
    bool openDumpFile( const QString& filename );
    /**
     * @returns true if the packet capturer is open
     */
    bool isOpen() const;
    /**
     * @returns the snapshot length of this packet capturer
     */
    int snapShot() const;
    /**
     * @returns true if the input capture file has a different byte-order
     * than the byte-order of the running system.
     */
    bool swapped() const;
    /**
     * @returns the libpcap version string used to write the input capture file.
     */
    QString version() const;
    /**
     * @returns the packet statistic database.
     * @see QMap
     */
    const QMap<QString,int>& statistics() const;

  signals:
    /**
     * This signal is emitted, when a packet has been received.
     */
    void receivedPacket( OPacket* );

  protected slots:
    void readyToReceive();

  protected:
    QString _name;                                  // devicename
    bool _open;                                     // check this before doing pcap calls
    pcap_t* _pch;                                   // pcap library handle
    pcap_dumper_t* _pcd;                            // pcap dumper handle
    QSocketNotifier* _sn;                           // socket notifier for main loop
    mutable char _errbuf[PCAP_ERRBUF_SIZE];         // holds error strings from libpcap
    QMap<QString, int> _stats;                      // statistics;
};

#endif // OPCAP_H

