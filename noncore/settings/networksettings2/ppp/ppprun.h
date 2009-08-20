#ifndef PPPRUN_H
#define PPPRUN_H

#include <qregexp.h>
#include <netnode.h>
#include "pppdata.h"

class PPPRun  : public RuntimeInfo {

public :

      PPPRun( ANetNodeInstance * NNI,
              PPPData & Data );

      bool handlesInterface( const QString & I );
      bool handlesInterface( InterfaceInfo * );

      State_t detectState( void );
      virtual RuntimeInfo * device( void )
        { return this; }
      virtual RuntimeInfo * connection( void )
        { return this; }

protected :

      QString setMyState( NetworkSetup * , Action_t, bool );

private :

      bool isMyPPPDRunning( void );
      bool isMyPPPUp( void );

      PPPData * D;
      QRegExp Pat;
};

#endif
