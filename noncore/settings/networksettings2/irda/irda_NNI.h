#ifndef IRDA_H
#define IRDA_H

#include <netnode.h>
#include "irdadata.h"
#include "irdarun.h"

class IRDANetNode;
class IRDAEdit;

class AIRDA : public ANetNodeInstance {

public :

      AIRDA( IRDANetNode * PNN );

      QWidget * edit( QWidget * parent );
      QString acceptable( void );
      void commit( void );

      RuntimeInfo * runtime( void ) 
        { if( RT == 0 ) 
            RT = new IRDARun( this, Data );
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

      IRDAEdit * GUI;
      IRDAData Data;
      IRDARun * RT;

};

#endif
