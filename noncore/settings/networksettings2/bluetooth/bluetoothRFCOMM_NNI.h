#ifndef BLUETOOTHRFCOMM_H
#define BLUETOOTHRFCOMM_H

#include <netnode.h>
#include "bluetoothRFCOMMdata.h"
#include "bluetoothRFCOMMrun.h"

class BluetoothRFCOMMNetNode;
class BluetoothRFCOMMEdit;

class ABluetoothRFCOMM : public ANetNodeInstance {

public :

      ABluetoothRFCOMM( BluetoothRFCOMMNetNode * PNN );

      RuntimeInfo * runtime( void ) 
        { return 
           ( RT ) ? RT : ( RT = new BluetoothRFCOMMRun( this, Data ) );
        }

      QWidget * edit( QWidget * parent );
      QString acceptable( void );
      void commit( void );

      virtual void * data( void )
        { return (void *)&Data; }

protected :

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );

private :

      BluetoothRFCOMMEdit * GUI;
      BluetoothRFCOMMData Data;
      BluetoothRFCOMMRun  * RT;
};

#endif
