#ifndef USBRUN_H
#define USBRUN_H

#include <asdevice.h>
#include <qregexp.h>
#include "usbdata.h"

class USBRun  : public AsDevice {

public :

      USBRun( ANetNodeInstance * NNI, 
              USBData & Data ) : 
                AsDevice( NNI ),
                Pat( "usb[0-9abcdef]" )
        { }

      virtual long count( void )
        { return 1; }
      virtual QString genNic( long nr );
      virtual AsDevice * device( void ) 
        { return asDevice(); }

protected :

      void detectState( NodeCollection * );
      bool setState( NodeCollection * , Action_t A );
      bool canSetState( State_t , Action_t A );

      bool handlesInterface( const QString & I );

private :

      InterfaceInfo * getInterface( void );
      QRegExp Pat;

};
#endif
