#ifndef PPPRUN_H
#define PPPRUN_H

#include <qregexp.h>
#include <asconnection.h>
#include <asdevice.h>
#include "pppdata.h"

class PPPRun  : public AsConnection, public AsDevice {

public :

      PPPRun( ANetNodeInstance * NNI, 
              PPPData & Data );
      virtual QString genNic( long NicNr )
        { QString S; return S.sprintf( "ppp%ld", NicNr ); }

      virtual AsDevice * asDevice( void ) 
        { return (AsDevice *)this; }
      virtual AsConnection * asConnection( void ) 
        { return (AsConnection *)this; }

      virtual AsDevice * device( void ) 
        { return (AsDevice *)this; }

      virtual RuntimeInfo * runtimeInfo( void ) 
        { return ( AsConnection *)this; }

 protected :

      void detectState( NodeCollection * NC );
      bool setState( NodeCollection * NC, Action_t A );
      bool canSetState( State_t S, Action_t A )
        { return AsDevice::connection()->findNext( 
            AsDevice::netNode() )->runtime()->canSetState( S,A ); }

      bool handlesInterface( const QString & I );

private :

      bool isMyPPPDRunning( void );
      bool isMyPPPUp( void );

      PPPData_t * D;
      QRegExp Pat;

};

#endif
