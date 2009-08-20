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

      RuntimeInfo * runtime( void )
        { return
            ( RT ) ? RT : ( RT = new WLanRun( this, Data ) );
        }

      QWidget * edit( QWidget * parent );
      QString acceptable( void );
      void commit( void );

      virtual void  * data( void )
        { return (void *)&Data; }

      virtual short generateFileEmbedded( SystemFile & Sf,
                                         long DevNr );

protected :

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );

private :

      WLanEdit * GUI;
      WLanData Data;
      WLanRun * RT;

};

#endif
