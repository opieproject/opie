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
#include <qhostaddress.h>
#include <qobject.h>
#include <qstring.h>

/* OPIE */
#include <opie2/onetutils.h>
#include "802_11_user.h"

/* TYPEDEFS */
typedef struct timeval timevalstruct;
typedef struct pcap_pkthdr packetheaderstruct;

/* FORWARDS */
class OPacketCapturer;

/*======================================================================================
 * OPacket - A frame on the wire
 *======================================================================================*/

class OPacket : public QObject
{
  Q_OBJECT

  public:
    OPacket( packetheaderstruct, const unsigned char*, QObject* parent );
    virtual ~OPacket();

    timevalstruct timeval() const;

    OPacketCapturer* packetCapturer() const;

    int caplen() const;
    int len() const;
    void dump() const;

  private:
    const packetheaderstruct _hdr;  // pcap packet header
    const unsigned char* _data;     // pcap packet data
};

/*======================================================================================
 * OEthernetPacket - DLT_EN10MB frame
 *======================================================================================*/

class OEthernetPacket : public QObject
{
  Q_OBJECT

  public:
    OEthernetPacket( const struct ether_header*, QObject* parent = 0 );
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
    OWaveLanPacket( const struct ieee_802_11_header*, QObject* parent = 0 );
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
    OWaveLanManagementPacket( const struct ieee_802_11_mgmt_header*, OWaveLanPacket* parent = 0 );
    virtual ~OWaveLanManagementPacket();

    QString SSID() const;

  private:
    const struct ieee_802_11_mgmt_header* _header;
    const struct ieee_802_11_mgmt_body* _body;
};


/*======================================================================================
 * OWaveLanDataPacket - type: data (T_DATA)
 *======================================================================================*/

class OWaveLanDataPacket : public QObject
{
  Q_OBJECT

  public:
    OWaveLanDataPacket( const struct ieee_802_11_data_header*, OWaveLanPacket* parent = 0 );
    virtual ~OWaveLanDataPacket();

  private:
    const struct ieee_802_11_data_header* _header;
};

/*======================================================================================
 * OLLCPacket - IEEE 802.2 Link Level Control
 *======================================================================================*/

class OLLCPacket : public QObject
{
  Q_OBJECT

  public:
    OLLCPacket( const struct ieee_802_11_802_2_header* data, QObject* parent = 0 );
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
    OIPPacket( const struct iphdr*, QObject* parent = 0 );
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
    OUDPPacket( const struct udphdr*, QObject* parent = 0 );
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
    OTCPPacket( const struct tcphdr*, QObject* parent = 0 );
    virtual ~OTCPPacket();

    int fromPort() const;
    int toPort() const;

  private:
    const struct tcphdr* _tcphdr;
};


/*======================================================================================
 * OPacketCapturer
 *======================================================================================*/

class OPacketCapturer : public QObject
{
  Q_OBJECT

  public:
    OPacketCapturer( QObject* parent = 0, const char* name = 0 );
    ~OPacketCapturer();

    void setBlocking( bool );
    bool blocking() const;

    void close();
    int dataLink() const;
    int fileno() const;
    OPacket* next();
    bool open( const QString& name );
    bool isOpen() const;

  signals:
    void receivedPacket( OPacket* );

  protected slots:
    void readyToReceive();

  protected:
    QString _name;  // devicename
    bool _open;     // check this before doing pcap calls
    pcap_t* _pch;   // pcap library handle
    mutable char _errbuf[PCAP_ERRBUF_SIZE];
};

#endif // OPCAP_H

