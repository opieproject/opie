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

      QWidget * edit( QWidget * parent );
      QString acceptable( void );
      void commit( void );

      RuntimeInfo * runtime( void ) 
        { if( RT == 0 ) 
            RT = new NetworkRun( this, Data );
          return RT;
        }

      virtual void * data( void ) 
        { return (void *)&Data; }

      virtual bool hasDataFor( const QString & S );
      virtual bool generateDataForCommonFile( 
          SystemFile & SF, long DevNr );

protected :

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );

private :

      NetworkEdit * GUI;
      NetworkData_t Data;
      NetworkRun * RT;
};

#endif
