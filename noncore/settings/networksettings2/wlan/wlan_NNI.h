#ifndef WLAN_H
#define WLAN_H

#include <netnode.h>
#include "wlandata.h"
#include "wlanrun.h"

class WLanNetNode;
class WLanEdit;

class AWLan : public ANetNodeInstance {

public :

      AWLan( WLanNetNode * PNN );

      QWidget * edit( QWidget * parent );
      QString acceptable( void );
      void commit( void );

      RuntimeInfo * runtime( void ) 
        { if( RT == 0 ) 
            RT = new WLanRun( this, Data );
          return RT;
        }

      virtual void  * data( void ) 
        { return (void *)&Data; }

      bool generateDeviceDataForCommonFile( SystemFile & S, long DevNr );

protected :

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );

private :

      WLanEdit * GUI;
      WLanData Data;
      WLanRun * RT;

};

#endif
