#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <resources.h>
#include "lancardrun.h"

State_t LanCardRun::detectState( void ) {

    // unavailable : no card found
    // available : card found and assigned to us or free
    // up : card found and assigned to us and up

    NodeCollection * NC = nodeCollection();
    QString S = QString( "/tmp/profile-%1.up" ).
                arg( NC->number());
    System & Sys = NSResources->system();
    InterfaceInfo * Run;

    QFile F( S );

    if( F.open( IO_ReadOnly ) ) {
      // could open file -> read interface and assign
      QString X;
      QTextStream TS(&F);
      X = TS.readLine();
      // find interface
      if( handlesInterface( X ) ) {
        for( QDictIterator<InterfaceInfo> It(Sys.interfaces());
             It.current();
             ++It ) {
          Run = It.current();
          if( X == Run->Name ) {
            NC->assignInterface( Run );
            return (Run->IsUp) ? IsUp : Available;
          }
        }
      }
    } 

    if( ( Run = NC->assignedInterface() ) ) {
      // we already have an interface assigned -> still present ?
      if( ! Run->IsUp ) {
        // usb is still free -> keep assignment
        return Available;
      } // else interface is up but NOT us -> some other profile
    }

    // nothing (valid) assigned to us
    NC->assignInterface( 0 );

    // find possible interface
    for( QDictIterator<InterfaceInfo> It(Sys.interfaces());
         It.current();
         ++It ) {
      Run = It.current();
      if( handlesInterface( *Run ) &&
          ( Run->CardType == ARPHRD_ETHER
#ifdef ARPHRD_IEEE1394
            || Run->CardType == ARPHRD_IEEE1394
#endif
          ) &&
          ! Run->IsUp
        ) {
        // proper type, and Not UP -> free
        return Off;
      }
    }
    // no free found

    return Unavailable;
}

QString LanCardRun::setMyState( NodeCollection * NC, Action_t A, bool ) {

    if( A == Activate ) {
      InterfaceInfo * N = getInterface();

      if( ! N ) {
        // no interface available
        NC->setCurrentState( Unavailable );
        return tr("No interface found");
      }

      // because we were OFF the interface
      // we get back is NOT assigned
      NC->assignInterface( N );
      NC->setCurrentState( Available );
      return QString();
    }

    if( A == Deactivate ) {
      NC->assignInterface( 0 );
      NC->setCurrentState( Off );
    }

    return QString();
}

// get interface that is free or assigned to us
InterfaceInfo * LanCardRun::getInterface( void ) {

    System & S = NSResources->system();
    InterfaceInfo * best = 0, * Run;

    for( QDictIterator<InterfaceInfo> It(S.interfaces());
         It.current();
         ++It ) {
      Run = It.current();
      if( handlesInterface( *Run ) &&
          ( Run->CardType == ARPHRD_ETHER
#ifdef ARPHRD_IEEE1394
            || Run->CardType == ARPHRD_IEEE1394
#endif
          )
        ) {
        // this is a LAN card
        if( Run->assignedConnection() == netNode()->connection() ) {
          // assigned to us
          return Run;
        } else if( Run->assignedConnection() == 0 ) {
          // free
          best = Run;
        }
      }
    }
    return best; // can be 0
}

bool LanCardRun::handlesInterface( const QString & S ) {
    InterfaceInfo * II;
    II = NSResources->system().interface( S );
    if( ( II = NSResources->system().interface( S ) ) ) {
      return handlesInterface( *II );
    }
    return Pat.match( S ) >= 0;
}

bool LanCardRun::handlesInterface( const InterfaceInfo & II ) {
    if( Pat.match( II.Name ) < 0 )
      return 0;

    if( Data->AnyLanCard ) {
      return 1;
    }

    // must also match hardware address
    return ( Data->HWAddresses.findIndex( II.MACAddress ) >= 0 );
}
