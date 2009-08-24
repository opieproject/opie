#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <qdir.h>
#include <system.h>
#include <resources.h>
#include <netnode.h>
#include "GPRSrun.h"

State_t GPRSRun::detectState( void ) {

      // is pppd still running ?
      // is rfcomm still active
      NetworkSetup * NC = networkSetup();
      InterfaceInfo * I = NC->assignedInterface();

      QDir D("/var/run");

      if( I ) {
        // has some pppx attached
        return ( I->IsUp ) ? IsUp : Available;
      }

      // check ppp itself and figure out interface

      odebug << "Check for ppp " << NC->name() << oendl;
      if( D.exists( QString("ppp-")+removeSpaces(NC->name())+".pid") ) {
        // get pid and check if pppd is still running
        QFile F( D.path()+"/ppp-"+removeSpaces(NC->name())+".pid");

        odebug << "PPP PID " << F.name() << oendl;
        if( F.open( IO_ReadOnly ) ) {
          QTextStream TS(&F);
          QString X = TS.readLine();
          PPPPid = X.toULong();
          int rv;

          rv = ::kill( PPPPid, 0 );
          if( rv == 0 ||
              ( rv < 0 && errno == EPERM )
            ) {
            // pppd is still up
            X = TS.readLine();
            I = NSResources->system().findInterface(X);

            odebug << "ppp running : IFace " << X << " = " << (long)I << oendl;

            if( I ) {
              NC->assignInterface( I );
              return (I->IsUp) ? IsUp : Available;
            }

            return Available;

          } else {
            // pppd is down
            PPPPid = 0;
          }
        } // else pppd is down
      }
      NC->assignInterface( 0 );
      return Unknown;
}

QString GPRSRun::setMyState( NetworkSetup * NC, Action_t A , bool ) {

    if( A == Up ) {
      // start ppp on deviceFile
      QStringList SL;
      SL << "pon"
         << removeSpaces( NC->name() )
         << NC->device()->deviceFile();

      if( ! NSResources->system().execAsUser( SL ) ) {
        return QString("Cannot start pppd for %1").arg(NC->name());
      }
    } else if ( A == Down ) {
      if( PPPPid == 0 ) {
        detectState();
      }
      if( PPPPid ) {
        QStringList SL;

        SL << "poff"
           << removeSpaces( NC->name() );

        if( ! NSResources->system().execAsUser( SL ) ) {
          return QString("Cannot terminate pppd for %1").arg(NC->name());
        }
        NC->assignInterface( 0 );
        odebug << "ppp stopped " << oendl;
        PPPPid = 0;
      }
    }

    return QString();
}

bool GPRSRun::handlesInterface( const QString & S ) {
    return Pat.match( S ) >= 0;
}

bool GPRSRun::handlesInterface( InterfaceInfo * I ) {
    return handlesInterface( I->Name );
}
