#ifndef USBRUN_H
#define USBRUN_H

#include <netnode.h>
#include <qregexp.h>
#include "usbdata.h"

class USBRun  : public RuntimeInfo {

public :

      USBRun( ANetNodeInstance * NNI, 
              USBData & Data ) : 
                RuntimeInfo( NNI ),
                Pat( "usb[0-9abcdef]" ) {
      }

      virtual RuntimeInfo * device( void ) 
        { return this; }

      bool handlesInterface( const QString & I );
      bool handlesInterface( InterfaceInfo * );

      State_t detectState( void );

protected :

      QString setMyState( NodeCollection * , Action_t, bool );

private :

      InterfaceInfo * getInterface( void );
      QRegExp Pat;

};
#endif
