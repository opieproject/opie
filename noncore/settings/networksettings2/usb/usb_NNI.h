#ifndef USB_H
#define USB_H

#include <netnode.h>
#include "usbdata.h"
#include "usbrun.h"

class USBNetNode;
class USBEdit;

class AUSB : public ANetNodeInstance {

public :

      AUSB( USBNetNode * PNN );

      QWidget * edit( QWidget * parent );
      QString acceptable( void );
      void commit( void );

      RuntimeInfo * runtime( void ) 
        { if( RT == 0 ) 
            RT = new USBRun( this, Data );
          return RT;
        }

      virtual void * data( void ) 
        { return (void *)&Data; }

      bool generateDataForCommonFile( SystemFile & S, long DevNr );
      bool generateDeviceDataForCommonFile( SystemFile & S, long DevNr );

protected :

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );

private :

      USBEdit * GUI;
      USBData Data;
      USBRun * RT;
};

#endif
