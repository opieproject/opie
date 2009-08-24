//-*-c++-*-
/***************************************************************************
 *   Copyright (C) 2003 by Fred Schaettgen                                 *
 *   kdebluetooth@0xF.de                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef OTPEER_H
#define OTPEER_H

#include <qstring.h>
#include <qobject.h>
#include <qvector.h>
#include <bluezlib.h>
#include <OTDeviceAddress.h>

class QTextStream;
class QTimerEvent;

namespace Opietooth2 {

class OTGateway;

class OTDriver;
class OTSDPService;
class OTSDPAttribute;
class OTUUID;
typedef QVector<OTSDPService> ServiceVector;

class OTPeer : public QObject {

      Q_OBJECT;

public :

      typedef enum PeerState_E {
        Peer_Unknown = -1,
        Peer_Down = 0,
        Peer_Up = 1
      } PeerState_t;

      OTPeer( OTGateway * _OT );
      OTPeer( QTextStream & TS, OTGateway * _TS );
      ~OTPeer();

      inline OTGateway * gateway() const
        { return OT; }

      inline int deviceClass( void ) const
        { return Class; }
      inline void setDeviceClass( int cls )
        { Class = cls; }

      void setAddress( const OTDeviceAddress & A )
        { Addr = A; }

      const OTDeviceAddress & address( void )
        { return Addr; }

      inline QString name( void ) const
        { return Name; }
      inline void setName( const QString & n )
        { Name = n; }

      ServiceVector & services( bool Force = 0 )
        { if( Force || serviceList.count() == 0 ) {
            updateServices();
          }
          return serviceList;
        }

      /* Returns TRUE if the device has at least a service
         with inside the required class Id.*/
      bool hasServiceClassID( const OTUUID & uuid);

      /* Get a vector of Rfcomm channels of the services
         having "uuid" in the class ID List */
      QArray<int> rfcommList( const OTUUID & uuid );

      inline void setState( PeerState_t S)
        { State = S; }
      inline PeerState_t state( void )
        { return State; }

      // go and find out state
      // will report back with signal
      void findOutState( int timeoutInSec = 1, bool Force = 0 );
      void stopFindingOutState( void );

      // return driver to which this peer is connected to
      // if it is connected
      inline OTDriver * connectedTo( void ) const
        { return ConnectedTo; }
      inline void setConnectedTo( OTDriver * D )
        { ConnectedTo = D; }

      void save( QTextStream& TS );
      void load( QTextStream& TS );

signals :

      // report back state
      void peerStateReport( OTPeer *);
      void error( const QString & );

protected :

      // while polling for result of ping
      void timerEvent( QTimerEvent * ev );

private:

      void updateServices();

      OTGateway * OT;
      OTDeviceAddress Addr;
      QString Name;
      int Class;
      ServiceVector serviceList;
      // -1 : don't know, 0 no, 1 yes
      PeerState_t State;
      OTDriver * ConnectedTo;

      int ProbeFD;
      int ProbePhase; // see OTDriver
      long ProbeTimeout;
};

}

#endif
