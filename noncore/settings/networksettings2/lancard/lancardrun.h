#ifndef LANCARDRUN_H
#define LANCARDRUN_H

#include <netnode.h>
#include <qregexp.h>
#include "lancarddata.h"

class LanCardRun  : public RuntimeInfo {

public :

      LanCardRun( ANetNodeInstance * NNI, 
                  LanCardData & D ) : RuntimeInfo( NNI ),
                                      Pat( "eth[0-9]" ) { 
        Data = &D;
      }

      virtual RuntimeInfo * device( void ) 
        { return this; }

      bool handlesInterface( const QString & I );
      bool handlesInterface( const InterfaceInfo & II );

      State_t detectState( void );

protected :

      QString setMyState( NetworkSetup * , Action_t, bool );

private :

      InterfaceInfo * getInterface( void );
      QRegExp Pat;
      LanCardData * Data;

};
#endif
