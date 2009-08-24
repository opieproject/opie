#ifndef WLANRUN_H
#define WLANRUN_H

#include <qregexp.h>
#include <netnode.h>
#include "wlandata.h"

class WLanRun  : public RuntimeInfo {

public :

      WLanRun( ANetNodeInstance * NNI, WLanData & Data ) :
               RuntimeInfo( NNI ),
               Pat( "wlan[0-9]" ) {
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
      WLanData * Data;
};

#endif
