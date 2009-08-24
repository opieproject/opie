#ifndef NETWORK_H
#define NETWORK_H

#include <netnode.h>
#include "networkdata.h"
#include "networkrun.h"

class NetworkNetNode;
class NetworkEdit;
class SystemFile;

class ANetwork : public ANetNodeInstance{

public :

      ANetwork( NetworkNetNode * PNN );

      RuntimeInfo * runtime( void )
        { return
           ( RT ) ? RT : ( RT = new NetworkRun( this, Data ) );
        }

      QWidget * edit( QWidget * parent );
      QString acceptable( void );
      void commit( void );

      virtual void * data( void )
        { return (void *)&Data; }

      virtual bool hasDataForFile( SystemFile & SF );
      virtual short generateFile( SystemFile &TS,
                                   long DevNr );

protected :

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );

private :

      NetworkEdit * GUI;
      NetworkData Data;
      NetworkRun * RT;
};

#endif
