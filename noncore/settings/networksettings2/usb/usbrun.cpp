#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <resources.h>
#include "usbrun.h"

void USBRun::detectState( NodeCollection * NC ) {
    // unavailable : no card found
    // available : card found and assigned to us or free
    // up : card found and assigned to us and up
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
            Run->assignNode( netNode() );
            assignInterface( Run );
            NC->setCurrentState( IsUp );
            return;
          }
        }
      }
    } 

    fprintf( stderr, "Assigned %p\n", assignedInterface() );
    if( ( Run = assignedInterface() ) ) {
      // we already have an interface assigned -> still present ?
      if( ! Run->IsUp ) {
        // usb is still free -> keep assignment
        NC->setCurrentState( Available );
        return;
      } // else interface is up but NOT us -> some other profile
    }

    // nothing (valid) assigned to us
    assignInterface( 0 );

    // find possible interface
    for( QDictIterator<InterfaceInfo> It(Sys.interfaces());
         It.current();
         ++It ) {
      Run = It.current();

      fprintf( stderr, "%s %d %d=%d %d\n",
          Run->Name.latin1(),
          handlesInterface( Run->Name ),
              Run->CardType, ARPHRD_ETHER,
              ! Run->IsUp );

      if( handlesInterface( Run->Name ) &&
          Run->CardType == ARPHRD_ETHER &&
          ! Run->IsUp
        ) {
        fprintf( stderr, "Released(OFF)\n" );
        // proper type, and Not UP -> free
        NC->setCurrentState( Off );
        return;
      }
    }
    // no free found
    fprintf( stderr, "UNA\n" );

    NC->setCurrentState( Unavailable );
}

bool USBRun::setState( NodeCollection * NC, Action_t A ) {

    // we only handle activate and deactivate
    switch( A ) {
      case Activate :
        { 
          if( NC->currentState() != Off ) {
            return 0;
          }
          InterfaceInfo * N = getInterface();
          if( ! N ) {
            // no interface available
            NC->setCurrentState( Unavailable );
            return 0;
          }
          // because we were OFF the interface
          // we get back is NOT assigned
          N->assignNode( netNode() );
          assignInterface( N );
          fprintf( stderr, "Assing %p\n", N );
          NC->setCurrentState( Available );
          return 1;
        }
      case Deactivate :
        if( NC->currentState() == IsUp ) {
          // bring down first
          if( ! connection()->setState( Down ) )
            // could not ...
            return 0;
        } else if( NC->currentState() != Available ) {
          return 1;
        }
        assignedInterface()->assignNode( 0 ); // release
        assignInterface( 0 );
        NC->setCurrentState( Off );
        return 1;
      default :
        // FT
        break;
    }
    return 0;
}

bool USBRun::canSetState( State_t Curr, Action_t A ) {
    // we only handle up down activate and deactivate
    switch( A ) {
      case Activate :
        { // at least available 
          if( Curr == Available ) {
            return 1;
          }
          // or we can make one available
          InterfaceInfo * N = getInterface();
          if( ! N || N->assignedNode() != 0 ) {
            // non available or assigned
            return 0;
          }
          return 1;
        }
      case Deactivate :
        return ( Curr >= Available );
      default :
        // FT
        break;
    }
    return 0;
}

// get interface that is free or assigned to us
InterfaceInfo * USBRun::getInterface( void ) {

    System & S = NSResources->system();
    InterfaceInfo * best = 0, * Run;
    QRegExp R( "usb[0-9abcdef]" );

    for( QDictIterator<InterfaceInfo> It(S.interfaces());
         It.current();
         ++It ) {
      Run = It.current();
      if( handlesInterface( Run->Name ) &&
          Run->CardType == ARPHRD_ETHER
        ) {
        // this is a USB card
        if( Run->assignedNode() == netNode() ) {
          // assigned to us
          return Run;
        } else if( Run->assignedNode() == 0 ) {
          // free
          best = Run;
        }
      }
    }
    return best; // can be 0
}

bool USBRun::handlesInterface( const QString & S ) {
    return Pat.match( S ) >= 0;
}

QString USBRun::genNic( long nr ) {
    QString S; 
    S.sprintf( "usbf" ); 
    return S;
}

