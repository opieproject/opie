#ifndef CABLE_H
#define CABLE_H

#include <netnode.h>
#include "cabledata.h"
#include "cablerun.h"

class CableNetNode;
class CableEdit;

class ACable : public ANetNodeInstance {

public :

      ACable( CableNetNode * PNN );

      QWidget * edit( QWidget * parent );
      QString acceptable( void );
      void commit( void );

      RuntimeInfo * runtime( void ) 
        { if( RT == 0 ) 
            RT = new CableRun( this, Data );
          return RT;
        }

      virtual void * data( void ) 
        { return (void *)&Data; }

      virtual bool hasDataFor( const QString & )
        { return 0; }
      virtual bool generateDataForCommonFile( 
          SystemFile & SF, long DevNr );

protected :

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );

private :

      CableEdit * GUI;
      CableData_t Data;
      CableRun * RT;

};

#endif
