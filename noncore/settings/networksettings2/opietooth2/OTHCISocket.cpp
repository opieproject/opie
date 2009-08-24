//-*-c++-*-
/***************************************************************************
 *   Copyright (C) 2003 by Fred Schaettgen                                 *
 *   kdebluetooth@schaettgen.de                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <qbuffer.h>
#include <qtimer.h>
#include <qdatastream.h>
#include <opie2/odebug.h>

#include <bluezlib.h>

// #include "deviceaddress.h"
#include <OTHCISocket.h>
#include <OTDriver.h>

using namespace Opietooth2;

OTHCISocket::OTHCISocket( OTDriver * D ) :
                            QObject( D, D->devname() ) {
    BStatusSet = false;
    Driver = D;
    HCIReadNotifier = 0;
}

OTHCISocket::~OTHCISocket() {
    close();
}

void OTHCISocket::close() {
    odebug << "OTHCISocket::close()" << oendl;
    if( HCIReadNotifier ) {
        delete HCIReadNotifier;
    }

    if( HCISocket.isValid() ) {
        HCISocket.close();
    }
}

bool OTHCISocket::open() {

    odebug << "OTHCISocket::open()" << oendl;
    int s;

    s = ::socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);

    if (s < 0) {
      emit error( tr( "Error creating socket on %1 : %2 %3").
                    arg( Driver->devname() ).
                    arg( errno ).
                    arg( strerror(errno) )
                );
      return false;
    }

    /* Bind socket to the HCI device */
    struct sockaddr_hci sa;
    sa.hci_family = AF_BLUETOOTH;
    sa.hci_dev = Driver->devId();
    if (bind(s, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
      ::close(s);
      emit error( tr( "Error binding to socket to %1 : %2 %3").
                    arg( Driver->devname() ).
                    arg( errno ).
                    arg( strerror(errno) )
                );
      return false;
    }

    struct hci_filter flt;
    hci_filter_clear(&flt);
    hci_filter_set_ptype(HCI_EVENT_PKT, &flt);
    hci_filter_all_events(&flt);
    if( setsockopt(s, SOL_HCI, HCI_FILTER, &flt, sizeof(flt)) < 0 ) {
      ::close(s);
      emit error( tr( "HCI filter setup failed on %1 : %2 %3").
                    arg( Driver->devname() ).
                    arg( errno ).
                    arg( strerror(errno) )
                );
      return false;
    }

    if( HCIReadNotifier ) {
      delete HCIReadNotifier;
    }

    HCISocket.setSocket(s, QSocketDevice::Datagram);
    HCIReadNotifier = new QSocketNotifier(
                  s, QSocketNotifier::Read, this);

    connect( HCIReadNotifier,
             SIGNAL(activated(int)),
             this,
             SLOT(slotSocketActivated())
           );

    //connect(hciSocket, SIGNAL(error(int)),
    //    this, SLOT(slotSocketError(int)));
    //connect(hciSocket, SIGNAL(connectionClosed()),
    //    this, SLOT(slotConnectionClosed()));
    //hciSocket->setSocket(s);

    return true;
}

void OTHCISocket::slotSocketError(int e) {
    close();
    emit error( tr( "HCI socket error 0x%1 on %1 : %2 %3").
                    arg(e,2,16).
                    arg( Driver->devname() ).
                    arg( errno ).
                    arg( strerror(errno) )
                );
}

void OTHCISocket::slotSocketActivated() {

    QSocketDevice::Error err = HCISocket.error();

    if( (err == QSocketDevice::NoError ) &&
        ( HCISocket.isValid() ) ) {
      //kdDebug() << "HCI socket ready read." << endl;

      unsigned char buf[512];
      int psize = HCISocket.readBlock((char*)buf, 512);

      if (psize <= 0) {
        slotSocketError(HCISocket.error());
        HCISocket.close();
        return;
      }

      //unsigned char packetType = buf[0];
      unsigned char eventCode = buf[1];
      unsigned char len = buf[2];

      if (psize-3 == len) {

        QByteArray databuf;
        databuf.duplicate((char*)(buf+3), len);
        emit event(eventCode, databuf);
        if (eventCode == EVT_CMD_STATUS) {
          updateStatus( databuf );
        }
      } else {
        odebug << "Error reading hci packet: packetSize("
              << psize
              << ")-3 != dataSize("
              << len
              << ")"
              << oendl;
      }
    } else if (err == QSocketDevice::NoError) {
      slotConnectionClosed();
    } else {
        HCISocket.close();
        slotSocketError(err);
    }
}

void OTHCISocket::updateStatus(const QByteArray& data) {

    QDataStream stream(data, IO_ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    Q_UINT8 status, dummy;
    Q_UINT16 opcode;

    BStatusSet = true;

    stream >> status >> dummy >> opcode;
    //kdDebug() << "updatestatus opcode=" << uint32_t(opcode) << endl;
    LastStatus = status;
    LastStatusOgf = cmd_opcode_ogf(opcode);
    LastStatusOcf = cmd_opcode_ocf(opcode);
}

void OTHCISocket::slotConnectionClosed() {
    odebug << "HCI connection closed." << oendl;
    emit connectionClosed();
}

void OTHCISocket::readEvent() {

    if (HCIReadNotifier) {
        slotSocketActivated();
    }
}

bool OTHCISocket::sendCommand( unsigned char ogf,
                               unsigned short ocf,
                               QByteArray buf
                             ) {
    QBuffer packet;
    QDataStream stream(&packet);

    stream.setByteOrder(QDataStream::LittleEndian);
    packet.open(IO_WriteOnly);

    if (buf.size() > 255) return false;

    //kdDebug() << "sendCommand. ogf=" << ogf << " ocf=" << ocf << endl;
    Q_UINT16 opcode = cmd_opcode_pack(ogf, ocf);
    Q_UINT8 pType = HCI_COMMAND_PKT;
    Q_UINT8 buflen = buf.size();

    stream << pType << opcode << buflen;
    stream.writeRawBytes(buf.data(), buflen);
    packet.close();
    HCISocket.writeBlock((const char*)packet.buffer(),
                         packet.buffer().size());
    return true;
}

bool OTHCISocket::readStatus( unsigned char ogf,
                              unsigned short ocf,
                              int *status,
                              int timeout_ms) {
    QTimer timer;

    timer.start(timeout_ms, true);
    BStatusSet = false;

    while (timer.isActive() && HCISocket.isValid()) {

        odebug << "OTHCISocket::readStatus()" << oendl;
        bool timeout = false;

        if( HCISocket.bytesAvailable() == 0) {
          int rv = HCISocket.waitForMore(timeout_ms);
          timeout = (rv == 0);
        }

        if (!timeout) {
          slotSocketActivated();
        }

        if( BStatusSet == true &&
            ogf == LastStatusOgf &&
            ocf == LastStatusOcf) {
            *status = LastStatus;
            odebug << "OTHCISocket::readStatus(ogf="
                  << ogf
                  << ",ocf="
                  << ocf
                  << ",timeout="
                  << LastStatus
                  << ")"
                  << oendl;
            return true;
        }
    }

    odebug << "OTHCISocket::readStatus(ogf="
          << ogf
          << ",ocf="
          << ocf
          << ",timeout="
          << LastStatus
          << ") : timeout "
          << oendl;
    return false;
}

int OTHCISocket::socket() {
    return HCISocket.socket();
}
