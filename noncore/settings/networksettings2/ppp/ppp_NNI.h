#ifndef PPP_H
#define PPP_H

#include <netnode.h>
#include "pppdata.h"
#include "ppprun.h"

class PPPNetNode;
class PPPEdit;

class APPP : public ANetNodeInstance {

public :

      APPP( PPPNetNode * PNN );

      QWidget * edit( QWidget * parent );
      QString acceptable( void );
      void commit( void );

      RuntimeInfo * runtime( void ) 
        { if( RT == 0 ) 
            RT = new PPPRun( this, Data );
          return RT->runtimeInfo();
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

      PPPEdit * GUI;
      PPPData Data;
      PPPRun * RT;

};

#endif
