#ifndef BLUETOOTHBNEP_H
#define BLUETOOTHBNEP_H

#include <netnode.h>
#include "bluetoothBNEPdata.h"
#include "bluetoothBNEPrun.h"

class BluetoothBNEPNetNode;
class BluetoothBNEPEdit;

class ABluetoothBNEP : public ANetNodeInstance {

public :

      ABluetoothBNEP( BluetoothBNEPNetNode * PNN );

      QWidget * edit( QWidget * parent );
      QString acceptable( void );
      void commit( void );

      RuntimeInfo * runtime( void ) 
        { if( RT == 0 ) 
            RT = new BluetoothBNEPRun( this, Data );
          return RT;
        }

      virtual void * data( void )
        { return (void *)&Data; }

      virtual bool hasDataFor( const QString & S );
      virtual bool generateDataForCommonFile( SystemFile & SF, long );

protected :

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );

private :

      BluetoothBNEPEdit * GUI;
      BluetoothBNEPData Data;
      BluetoothBNEPRun * RT;

};

#endif
