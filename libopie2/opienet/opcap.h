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
#include "802_11_user.h"

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

  public:
    OPacket( int datalink, packetheaderstruct, const unsigned char*, QObject* parent );
    virtual ~OPacket();

    timevalstruct timeval() const;

    int caplen() const;
    int len() const;
    QString dump( int = 32 ) const;

    void updateStats( QMap<QString,int>&, QObjectList* );

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

    QString ID() const;

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

// Qobject do we need that??
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
// Qobject?
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

  private:
    const struct udphdr* _udphdr;
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
 * library libpcap. ...
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
     * Setting the packet capturer to use blocking IO calls can be useful when
     * not using the socket notifier, e.g. without an application object.
     */
    void setBlocking( bool );
    /**
     * @returns true if the packet capturer uses blocking IO calls.
     */
    bool blocking() const;
    /**
     * Closes the packet capturer. This is automatically done in the destructor.
     */
    void close();
    /**
     * @returns the data link type.
     * @see <pcap.h> for possible values.
     */
    int dataLink() const;
    /**
     * @returns the filedescriptor of the packet capturer. This is only useful, if
     * not using the socket notifier, e.g. without an application object.
     */
    int fileno() const;
    /**
     * @returns the next @ref OPacket from the packet capturer.
     * @note If blocking mode is true then this call might block.
     */
    OPacket* next();
    /**
     * Open the packet capturer to capture packets in live-mode from @a interface.
     * If a @a filename is given, all captured packets are output to a tcpdump-compatible capture file.
     */
    bool open( const QString& interface, const QString& filename = QString::null );
    /**
     * Open the packet capturer to capture packets in offline-mode from @a file.
     */
    bool open( const QFile& file );
    /**
     * @returns true if the packet capturer is open
     */
    bool isOpen() const;

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

