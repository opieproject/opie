#ifndef PPPRUN_H
#define PPPRUN_H

#include <qregexp.h>
#include <asconnection.h>
#include "pppdata.h"

class PPPRun  : public AsConnection {

public :

      PPPRun( ANetNodeInstance * NNI, 
              PPPData & Data ) : 
                AsConnection( NNI ),
                Pat( "eth[0-9]" )
        { D = &Data; }

protected :

      void detectState( NodeCollection * NC );
      bool setState( NodeCollection * NC, Action_t A );
      bool canSetState( State_t S, Action_t A )
        { return connection()->findNext( netNode() )->runtime()->canSetState( S,A ); }

      bool handlesInterface( const QString & I );

private :

      bool isMyPPPDRunning( void );
      bool isMyPPPUp( void );

      PPPData_t * D;
      QRegExp Pat;

};

#endif
