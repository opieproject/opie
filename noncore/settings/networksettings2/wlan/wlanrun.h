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

      virtual AsDevice * device( void ) 
        { return (AsDevice *)this; }
      virtual AsDevice * asDevice( void ) 
        { return (AsDevice *)this; }

protected :

      void detectState( NodeCollection *  );
      bool setState( NodeCollection *, Action_t, bool );
      bool canSetState( State_t, Action_t );
      bool handlesInterface( const QString & I );
      bool handlesInterface( const InterfaceInfo & II );

private :

      InterfaceInfo * getInterface( void );
      QRegExp Pat;
      WLanData * Data;
};

#endif
