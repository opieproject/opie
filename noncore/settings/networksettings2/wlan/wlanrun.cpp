
#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <resources.h>
#include "wlanrun.h"

State_t WLanRun::detectState( void ) {

    // unavailable : no card found
    // available : card found and assigned to us or free
    // up : card found and assigned to us and up

    NetworkSetup * NC = networkSetup();
    QString S = QString( "/tmp/profile-%1.up" ).arg(NC->number());
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

    return Unavailable;

}

QString WLanRun::setMyState( NetworkSetup * , Action_t , bool ) {

    // we only handle activate and deactivate
    return QString();
}

// get interface that is free or assigned to us
InterfaceInfo * WLanRun::getInterface( void ) {

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
        if( Run->assignedToNetworkSetup() == netNode()->networkSetup() ) {
          // assigned to us
          return Run;
        } else if( Run->assignedToNetworkSetup() == 0 ) {
          // free
          best = Run;
        }
      }
    }
    return best; // can be 0
}

bool WLanRun::handlesInterface( const QString & S ) {
    InterfaceInfo * II = NSResources->system().interface( S );
    if( II ) {
      return handlesInterface( *II );
    }
    return Pat.match( S ) >= 0;
}

bool WLanRun::handlesInterface( const InterfaceInfo & II ) {
    return ( Pat.match( II.Name ) < 0 );
}
