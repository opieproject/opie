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

#include <assert.h>
#include <sys/poll.h>
#include <string.h>

#include <bluezlib.h>
#include <qarray.h>
#include <qtextstream.h>

#include <opie2/odebug.h>

#include <OTDeviceAddress.h>
#include <OTSDPAttribute.h>
#include <OTSDPService.h>
#include <OTPeer.h>
#include <OTGateway.h>
#include <OTDriver.h>

using namespace Opietooth2;

OTPeer::OTPeer( OTGateway * _OT ) {
      OT = _OT;
      State = Peer_Unknown;
      ConnectedTo = 0;
}

OTPeer::OTPeer( QTextStream & TS, OTGateway * _OT ) {
      OT = _OT;
      State = Peer_Unknown;
      ConnectedTo = 0;

      load( TS );
}

OTPeer::~OTPeer( ) {

}

void OTPeer::updateServices( void ) {
    sdp_session_t *session;

    serviceList.clear();

    odebug << "Get services from " << Addr.toString() << oendl;

    session = sdp_connect( &(OTDeviceAddress::any.getBDAddr()),
                           &(Addr.getBDAddr()),
                           0);

    if (!session) {
      odebug << "sdp_connect("
            << Addr.toString()
            << ") failed"
            << oendl;
        return; // error
    }

    uint32_t range = 0x0000ffff;
    sdp_list_t* attrId = sdp_list_append(0, &range);

    // search all public features
    uuid_t grp;
    sdp_uuid16_create( &grp, PUBLIC_BROWSE_GROUP );
    sdp_list_t * search = sdp_list_append(0, &grp );

    // get data from peer
    sdp_list_t* seq;
    if (sdp_service_search_attr_req( session,
                                     search,
                                     SDP_ATTR_REQ_RANGE,
                                     attrId,
                                     &seq ) ) {
      odebug << "Service Search failed" << oendl;
      sdp_close(session);
      return;
    }

    sdp_list_free(attrId, 0);
    sdp_list_free(search, 0);

    // process result
    sdp_list_t* next = NULL;

    for (; seq; seq = next) {
      sdp_record_t *rec = (sdp_record_t *) seq->data;

      sdp_list_t* attrlist = rec->attrlist;
      AttributeVector alist;
      OTSDPService * service;

      service = new OTSDPService();

      for (; attrlist; attrlist = attrlist->next) {
        int attrID = ((sdp_data_t*)(attrlist->data))->attrId;
        service->addAttribute(
              attrID,
              new OTSDPAttribute( (sdp_data_t*)(attrlist->data) )
            );
      }

      serviceList.resize( serviceList.size() + 1 );
      serviceList.insert( serviceList.size() - 1, service );

      next = seq->next;
      free(seq);
      sdp_record_free(rec);
    }
    sdp_close(session);
}

bool OTPeer::hasServiceClassID( const OTUUID & uuid) {
    for( unsigned int i = 0;
         i < serviceList.count();
         i ++ ) {
      if( serviceList[i]->hasClassID(uuid))
        return true;
    }
    return false;
}
/** Get a vector of Rfcomm channels of the services having "uuid" in the class ID List*/
QArray<int> OTPeer::rfcommList( const OTUUID & uuid) {

    QArray<int> rfcommList;
    unsigned int channel;

    for( unsigned int i = 0;
         i < serviceList.count();
         i ++ ) {
      if( serviceList[i]->hasClassID(uuid)) {
        if( serviceList[i]->rfcommChannel(channel) ) {
          rfcommList.resize( rfcommList.size()+1 );
          rfcommList[rfcommList.size()-1] = channel;
        }
      }
    }
    return rfcommList;
}

void OTPeer::save( QTextStream & TS ) {
        TS << "bdaddr " << address().toString() << endl;
        TS << "name " << name() << endl;
        TS << "class " << deviceClass() << endl;
}

void OTPeer::load( QTextStream & TS ) {
      QString S;
      S = TS.readLine();
      setAddress( OTDeviceAddress( S.mid( 7 ) ) );

      S = TS.readLine();
      setName( S.mid( 5 ) );

      S = TS.readLine();
      setDeviceClass( S.mid( 6 ).toLong() );
}

#define MAGICNR         -99999
#define POLLDELAY       1000
#define PREMAGICNR      (MAGICNR+POLLDELAY)

void OTPeer::findOutState( int timeoutInSec, bool Force ) {
      ProbeFD = -1;
      if( Force && ConnectedTo == 0 ) {
        State = OTPeer::Peer_Unknown;
      } // else keep state or is connected to us

      if( State == OTPeer::Peer_Unknown ) {
        ProbePhase = 0;
        ProbeTimeout = timeoutInSec*1000;
        odebug << "Ping " << address().toString() << oendl;
        startTimer( POLLDELAY );
      } else {
        ProbeTimeout = 0;
        startTimer( 0 );
      }
}

#define PINGSIZE        20
void OTPeer::timerEvent( QTimerEvent * ev ) {

      ProbeTimeout -= POLLDELAY;

      if( State == OTPeer::Peer_Unknown ) {
        switch( ProbePhase ) {
          case 0 : // connect nonblock
            { struct sockaddr_l2 addr;

              if (( ProbeFD = ::socket(PF_BLUETOOTH, SOCK_RAW, BTPROTO_L2CAP)) < 0) {
                ProbeTimeout = 0;
                break;
              }

              memset(&addr, 0, sizeof(addr));

              addr.l2_family = AF_BLUETOOTH;
              addr.l2_bdaddr = OTDeviceAddress::any.getBDAddr();

              if( ::bind( ProbeFD, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
                ProbeTimeout = 0;
                break;
              }

              // non blocking
              if( ::fcntl( ProbeFD, F_SETFL, O_NONBLOCK ) < 0 ) {
                ProbeTimeout = 0;
                break;
              }

              // to this peer
              addr.l2_bdaddr = address().getBDAddr();
              if( ::connect( ProbeFD, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
                if( errno != EAGAIN && errno != EINPROGRESS ) {
                  ProbeTimeout = 0;
                  break;
                } // wait for connect to fail or succeed
              }
            }
            ProbePhase = 1; // wait for connect
            break;
          case 1 :
            { struct pollfd pf[1];
              char buf[L2CAP_CMD_HDR_SIZE + PINGSIZE + 20];
              int n;

              pf[0].fd = ProbeFD;
              pf[0].events = POLLOUT;
              if( (n = ::poll(pf, 1, 0)) < 0 ) {
                odebug << address().toString()
                      << " : errno "
                      << errno
                      << " "
                      << strerror(errno)<<oendl;
                ProbeTimeout = 0;
                break;
              }

              if( ! n ) {
                // not ready -> try again
                break;
              }

              // send ping
              for( unsigned int i = L2CAP_CMD_HDR_SIZE; i < sizeof(buf); i++)
                  buf[i] = (i % 40) + 'A';

              l2cap_cmd_hdr *cmd = (l2cap_cmd_hdr *) buf;

              /* Build command header */
              cmd->code  = L2CAP_ECHO_REQ;
              cmd->ident = *(char *)this; // get some byte
              cmd->len   = PINGSIZE;

              /* Send Echo Request */
              if( ::send(ProbeFD, buf, PINGSIZE + L2CAP_CMD_HDR_SIZE, 0) <= 0) {
                if( errno == EACCES ) {
                  // permission denied means that we could not
                  // connect because the device does not allow us
                  // but it is UP
                  odebug << address().toString()
                        << " good send error "
                        << errno
                        << " "
                        << strerror( errno)
                        << oendl;
                  State = OTPeer::Peer_Up;
                  ProbeTimeout = 0;
                  break;
                } else if( errno != EBUSY ) {
                  odebug << address().toString()
                        << " : errno "
                        << errno
                        << " "
                        << strerror(errno)
                        << oendl;
                  ProbeTimeout = 0;
                  break;
                } // else want some more
              }

              ProbePhase = 2; // wait for ping reply
            }
            break;
          case 2 : // wait for reply
            { struct pollfd pf[1];
              char buf[L2CAP_CMD_HDR_SIZE + PINGSIZE + 20];
              l2cap_cmd_hdr *cmd = (l2cap_cmd_hdr *) buf;
              int n;

              pf[0].fd = ProbeFD;
              pf[0].events = POLLIN;
              if( (n = ::poll(pf, 1, 0)) < 0 ) {
                odebug << address().toString()
                      << " : errno "
                      << errno
                      << " "
                      << strerror(errno)
                      <<oendl;
                ProbeTimeout = 0;
                break;
              }

              if( ! n ) {
                // not ready -> try again
                break;
              }

              if( (n = ::recv( ProbeFD, buf, sizeof(buf), 0)) < 0) {
                odebug << address().toString()
                      << "errno "
                      << errno
                      << " "
                      << strerror(errno)
                      << oendl;
                ProbeTimeout = 0;
                break;
              }

              /* Check for our id */
              if( cmd->ident != *(char *)this )
                // not our reply
                break;

              odebug << "reply from "
                    << address().toString()
                    << oendl;
              // whatever reply we get is a valid reply
              State = OTPeer::Peer_Up;
              ProbeTimeout = 0;
            }
            break;
        }

        if( State != OTPeer::Peer_Unknown ) {
          ProbeTimeout = 0;
        }
      }

      if( ProbeTimeout <= 0 ) {
        // regular timeout
        emit peerStateReport( this );
        if( State == Peer_Unknown ) {
          State = Peer_Down;
        }
        if( ProbeFD >= 0 ) {
          // requested to stop by caller -> stop probing
          ::close( ProbeFD );
        }
        // no more waiting
        killTimer( ev->timerId() );
      }  // else sleep some more
}

void OTPeer::stopFindingOutState( void ) {
      ProbeTimeout = PREMAGICNR;
}

