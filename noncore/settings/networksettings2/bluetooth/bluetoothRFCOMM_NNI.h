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

      QWidget * edit( QWidget * parent );
      QString acceptable( void );
      void commit( void );

      RuntimeInfo * runtime( void ) 
        { if( RT == 0 ) 
            RT = new BluetoothRFCOMMRun( this, Data );
          return RT;
        }

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
