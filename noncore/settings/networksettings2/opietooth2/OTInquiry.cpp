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


#include <qcstring.h>
#include <qsocket.h>
#include <qdatetime.h>
#include <qtimer.h>
#include <qthread.h>
#include <qapplication.h>

#include <bluezlib.h>

#include <OTGateway.h>
#include <OTDriver.h>
#include <OTPeer.h>
#include <OTHCISocket.h>
#include <OTInquiry.h>

#include <opie2/odebug.h>

using namespace Opietooth2;

#define max(a,b)        (((a)>(b)) ? (a) : (b))
#define min(a,b)        (((a)>(b)) ? (b) : (a))

OTInquiry::OTInquiry( OTDriver * Drv ) : QObject( Drv ) {

    reset();

    InquiryTimeoutTimer = new QTimer(this);

    connect( InquiryTimeoutTimer, 
             SIGNAL(timeout()),
             this, 
             SLOT(slotInquiryTimeout()));

    Driver = Drv;
    Socket = Drv->openSocket();
    Socket->open();

    connect( Socket, 
             SIGNAL( event(unsigned char, QByteArray)),
             this, 
             SLOT(slotHCIEvent(unsigned char, QByteArray)));
}

OTInquiry::~OTInquiry() {
    stopInquiring();
}

void OTInquiry::stopInquiring( void ) {
    if( Socket ) {
      odebug << "Stop inquiry" << oendl;
      Driver->closeSocket();
      Socket = 0;
    }
}

bool OTInquiry::inquire( double timeout, int numResponses, int lap) {

    QByteArray cmdBuf(5);

    cmdBuf[0] = lap & 0xFF;
    cmdBuf[1] = (lap >> 8) & 0xFF;
    cmdBuf[2] = (lap >> 16) & 0xFF;
    cmdBuf[3] = max(0x01, min(0x30, int(timeout/1.28)));
    cmdBuf[4] = (unsigned char)numResponses;

    odebug << "Send HCI inquiry command. wait for " << cmdBuf[3] << oendl;

    Socket->sendCommand(0x01, 0x0001, cmdBuf);

    int status;

    if( Socket->readStatus(0x01, 0x0001, &status)) {
      if (status == 0) {
        SuccessfullyStarted = true;
        InquiryTimeoutTimer->start( int(1000*(timeout+1.0)), true);
        return true;
      }
      else {
        QString S =QString().sprintf( "%x", status );
        odebug << "OTInquiry::inquiry() failed: 0x" << S << oendl;
        emit finished();
        return false;
      }
    } else {
      odebug << "OTInquiry::inquiry(): Timeout." << oendl;
      return false;
    }
}

bool OTInquiry::isInquiring() {
    return InquiryTimeoutTimer->isActive();
}

bool OTInquiry::isFinished() {
    return SuccessfullyStarted && SuccessfullyEnded;
}
      
void OTInquiry::reset() {

    SuccessfullyStarted = false;
    SuccessfullyEnded = false;
    //addrCache.clear();
    //infoQueue.clear();
}


void OTInquiry::onPeerFound( OTPeer * Peer, bool IsNew ) {
    emit peerFound( Peer, IsNew );
}

void OTInquiry::slotInquiryTimeout() {
    emit error( tr( "Timeout while waiting for end of inquiry.") );
}

void OTInquiry::slotHCIEvent(unsigned char eventCode, QByteArray buf) {

    odebug << "OTInquiry: hci packet received: eventCode="
          << (unsigned int)eventCode 
          << " packetLength="
          << (unsigned int)buf.size() 
          << oendl;

    unsigned char *data = (unsigned char*)buf.data();
    switch (eventCode) {
      case EVT_INQUIRY_COMPLETE:
        { unsigned char status = data[0];
          odebug << "EVT_INQUIRY_COMPLETE status=" << status << oendl;
          InquiryTimeoutTimer->stop();
          if (status == 0) {
            if( SuccessfullyStarted == true) {
              odebug << "OTInquiry ended successfully" << oendl;
              SuccessfullyEnded = true;
            }
            emit finished();
          }
          else {
            emit error( tr( "OTInquiry completed with error (code %1)" ).
                          arg(status));
          }
        }
        break;
      case EVT_INQUIRY_RESULT:
        { int numResults = data[0];
          OTPeer * P = 0;
          bool IsNew;
          OTDeviceAddress Addr;
          QString N;

          inquiry_info *results = (inquiry_info*)(data+1);

          for (int n=0; n<numResults; n++) {
            Addr.setBDAddr( results[n].bdaddr );

            odebug << "INQUIRY_RESULT: "
                  << Addr.toString() 
                  << oendl;

            P = Driver->gateway()->findPeer( Addr );

            if( P ) {
              // peer known
              if( P->state() != OTPeer::Peer_Up ) {
                P->setState( OTPeer::Peer_Up );
              }
              IsNew = 0;
            } else {
              IsNew = 1;
              // push the address to the address queue
              // where it can be consumed by nextNeighbour()
              P = new OTPeer( Driver->gateway() );
              P->setState( OTPeer::Peer_Up ); // we just detected it
              P->setAddress( Addr );
              //if( addrCache.find(info.addr) == addrCache.end()) {
              // addrCache.insert(info.addr);

              P->setDeviceClass( (results[n].dev_class[0] << 16) |
                                 (results[n].dev_class[1] << 8) |
                                 (results[n].dev_class[2] << 0) );
              // infoQueue.push_back(info);
              P->setName( Driver->getPeerName( Addr ) ); 
            }

            // call the handler. Emits a signal if not overwritten
            onPeerFound( P, IsNew );

            // }
          }
        }
        break;
      case EVT_CMD_STATUS :
        { int status = data[0];
          int numHciCmdPkts = data[1];
          int cmdOpcode = *((uint16_t*)(data+2));
          odebug << "EVT_CMD_STATUS status=" 
                << status
                << " numPkts=" 
                << numHciCmdPkts 
                << " cmdOpcode="
                << cmdOpcode 
                << oendl;
          if (cmdOpcode == OCF_INQUIRY) {

          }
        }
        break;
    }
}
