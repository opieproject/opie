#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <resources.h>
#include "bluetoothBNEPrun.h"

QDict<QString> *     BluetoothBNEPRun::PANConnections = 0;

void BluetoothBNEPRun::detectState( NodeCollection * NC ) { 
    // unavailable : no card found
    // available : card found and assigned to us or free
    // up : card found and assigned to us and up
    QString S = QString( "/tmp/profile-%1.up" ).arg(NC->number());
    System & Sys = NSResources->system();
    InterfaceInfo * Run;
    QFile F( S );

    Log(("Detecting for %s\n", NC->name().latin1() ));

    if( F.open( IO_ReadOnly ) ) {
      // could open file -> read interface and assign
      QString X;
      bool accepted = 0;
      QTextStream TS(&F);
      X = TS.readLine();
      Log(("%s exists : %s\n", S.latin1(), X.latin1() ));
      // find interface
      if( handlesInterface( X ) ) {

        Log(("Handles interface %s, PANC %p\n", X.latin1(), PANConnections ));
        if( PANConnections == 0 ) {
          // load connections that are active
          // format : bnep0 00:60:57:02:71:A2 PANU
          FILE * OutputOfCmd = popen( "pand --show", "r" ) ;

          PANConnections = new QDict<QString>;

          if( OutputOfCmd ) {
            char ch;
            // could fork
            // read all data
            QString Line =  "";
            while( 1 ) {
              if( fread( &ch, 1, 1, OutputOfCmd ) < 1 ) {
                // eof
                break;
              }
              if( ch == '\n' || ch == '\r' ) {
                if( ! Line.isEmpty() ) {
                  if( Line.startsWith( "bnep" ) ) {
                    QStringList SL = QStringList::split( " ", Line );
                    Log(("Detected PAN %s %s\n", 
                      SL[0].latin1(), SL[1].latin1() ));
                    PANConnections->insert( SL[0], new QString(SL[1]));
                  }
                  Line="";
                }
              } else {
                Line += ch;
              }
            }
          }

          pclose( OutputOfCmd );
        }

        // check if this runtime allows connection to node
        if( ! Data.AllowAll ) {
          // has addresses
          for ( QStringList::Iterator it = Data.BDAddress.begin(); 
                ! accepted && it != Data.BDAddress.end(); 
                ++ it ) {
            for( QDictIterator<QString> it2( *(PANConnections) );
                 it2.current();
                 ++ it2 ) {
              if( X == it2.currentKey() &&
                  (*it) == *(it2.current()) 
                ) {
                // found
                Log(("%s accepts connections to %s\n",
                    NC->name().latin1(),
                    it2.current()->latin1() ));
                accepted = 1;
                break;
              }
            }
          }
        } else {
          Log(("%s accepts any connection\n", NC->name().latin1() ));
          // accept any
          accepted = 1;
        }

        if( accepted ) {
          // matches and is allowed for this node
          for( QDictIterator<InterfaceInfo> It(Sys.interfaces());
               It.current();
               ++It ) {
            Run = It.current();
            if( X == Run->Name ) {
              Log(("%s Assigned %p\n", NC->name().latin1(), Run ));
              Run->assignNode( netNode() );
              assignInterface( Run );
              NC->setCurrentState( IsUp );
              return;
            }
          }
        }
      }
    } 

    Log(("Assigned %p\n", assignedInterface() ));
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

      Log(("%s %d %d=%d %d\n",
          Run->Name.latin1(),
          handlesInterface( Run->Name ),
              Run->CardType, ARPHRD_ETHER,
              ! Run->IsUp ));

      if( handlesInterface( Run->Name ) &&
          Run->CardType == ARPHRD_ETHER &&
          ! Run->IsUp
        ) {
        Log(("Released(OFF)\n" ));
        // proper type, and Not UP -> free
        NC->setCurrentState( Off );
        return;
      }
    }
    // no free found
    Log(("None available\n" ));

    NC->setCurrentState( Unavailable );
}

bool BluetoothBNEPRun::setState( NodeCollection * NC, Action_t A, bool  ) {

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
          Log(("Assing %p\n", N ));
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

bool BluetoothBNEPRun::canSetState( State_t Curr , Action_t A ) {
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
InterfaceInfo * BluetoothBNEPRun::getInterface( void ) {

    System & S = NSResources->system();
    InterfaceInfo * best = 0, * Run;

    for( QDictIterator<InterfaceInfo> It(S.interfaces());
         It.current();
         ++It ) {
      Run = It.current();
      if( handlesInterface( Run->Name ) &&
          Run->CardType == ARPHRD_ETHER
        ) {
        // this is a bluetooth card
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

bool BluetoothBNEPRun::handlesInterface( const QString & S ) {
    return Pat.match( S ) >= 0;
}
