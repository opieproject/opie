#ifndef LANCARDRUN_H
#define LANCARDRUN_H

#include <asdevice.h>
#include <qregexp.h>
#include "lancarddata.h"

class LanCardRun  : public AsDevice {

public :

      LanCardRun( ANetNodeInstance * NNI, 
                  LanCardData & Data ) : AsDevice( NNI ),
                                         Pat( "eth[0-9]" )
        { }

      virtual long count( void )
        { return 2; }
      virtual QString genNic( long nr )
        { QString S; return S.sprintf( "eth%ld", nr ); }
      virtual AsDevice * device( void ) 
        { return asDevice(); }

protected :

      void detectState( NodeCollection * NC );
      bool setState( NodeCollection * NC, Action_t A );
      bool canSetState( State_t Curr, Action_t A );

      bool handlesInterface( const QString & I );

private :

      InterfaceInfo * getInterface( void );
      QRegExp Pat;

};
#endif
