#ifndef WLANRUN_H
#define WLANRUN_H

#include <qregexp.h>
#include <asdevice.h>
#include "wlandata.h"

class WLanRun  : public AsDevice {

public :

      WLanRun( ANetNodeInstance * NNI, WLanData & Data ) : 
               AsDevice( NNI ),
               Pat( "wlan[0-9]" )
        { }

      virtual long count( void )
        { return 2; }
      virtual QString genNic( long nr )
        { QString S; return S.sprintf( "wlan%ld", nr ); }
      virtual AsDevice * device( void ) 
        { return (AsDevice *)this; }
      virtual AsDevice * asDevice( void ) 
        { return (AsDevice *)this; }

protected :

      void detectState( NodeCollection *  )
        { }

      bool setState( NodeCollection *, Action_t )
        { return 0; }

      bool canSetState( State_t, Action_t )
        { return 0; }

      bool handlesInterface( const QString & I );

private :

      QRegExp Pat;
};

#endif
