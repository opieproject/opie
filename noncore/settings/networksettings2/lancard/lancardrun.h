#ifndef LANCARDRUN_H
#define LANCARDRUN_H

#include <asdevice.h>
#include <qregexp.h>
#include "lancarddata.h"

class LanCardRun  : public AsDevice {

public :

      LanCardRun( ANetNodeInstance * NNI, 
                  LanCardData & D ) : AsDevice( NNI ),
                                         Pat( "eth[0-9]" )
        { Data = &D; }

      virtual AsDevice * device( void ) 
        { return (AsDevice *)this; }

      virtual AsDevice * asDevice( void ) 
        { return (AsDevice *)this; }

protected :

      void detectState( NodeCollection * NC );
      bool setState( NodeCollection * NC, Action_t A );
      bool canSetState( State_t Curr, Action_t A );

      bool handlesInterface( const QString & I );
      bool handlesInterface( const InterfaceInfo & II );

private :

      InterfaceInfo * getInterface( void );
      QRegExp Pat;
      LanCardData * Data;

};
#endif
