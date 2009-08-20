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

      RuntimeInfo * runtime( void )
        { return
            ( RT ) ? RT : ( RT = new USBRun( this, Data ) );
        }

      QWidget * edit( QWidget * parent );
      QString acceptable( void );
      void commit( void );

      virtual void * data( void )
        { return (void *)&Data; }

      virtual short generateFileEmbedded( SystemFile & TS,
                                         long DevNr );

protected :

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );

private :

      USBEdit * GUI;
      USBData Data;
      USBRun * RT;
};

#endif
