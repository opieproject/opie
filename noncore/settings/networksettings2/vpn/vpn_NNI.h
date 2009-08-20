#ifndef VPN_H
#define VPN_H

#include <netnode.h>
#include "vpndata.h"
#include "vpnrun.h"

class VPNNetNode;
class VPNEdit;

class AVPN : public ANetNodeInstance {

public :

      AVPN( VPNNetNode * PNN );

      RuntimeInfo * runtime( void )
        { return
            ( RT ) ? RT : ( RT = new VPNRun( this, Data ) );
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

      VPNEdit * GUI;
      VPNData Data;
      VPNRun * RT;

};

#endif
