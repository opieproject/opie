#include <netnode.h>
#include <qregexp.h>
#include "GPRSdata.h"

class GPRSRun  : public RuntimeInfo {

public :

      GPRSRun( ANetNodeInstance * NNI, 
               GPRSData & D ) : RuntimeInfo( NNI ), 
                                   Pat( "ppp[0-9]" ) { 
        PPPPid = 0;
      }

      bool handlesInterface( const QString & I );
      bool handlesInterface( InterfaceInfo * );

      virtual RuntimeInfo * device( void ) 
        { return this; }
      virtual RuntimeInfo * connection( void ) 
        { return this; }

      State_t detectState( void );

protected :

      QString setMyState( NetworkSetup * , Action_t, bool );

private :

      QRegExp Pat;
      size_t  PPPPid;

};
