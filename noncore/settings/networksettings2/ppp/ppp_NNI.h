#ifndef PPP_H
#define PPP_H

#include <netnode.h>
#include "pppdata.h"
#include "ppprun.h"

class PPPNetNode;
class PPPEdit;
class QTextStream;

class APPP : public ANetNodeInstance {

public :

      APPP( PPPNetNode * PNN );

      RuntimeInfo * runtime( void ) 
        { if( RT == 0 ) {
            RT = new PPPRun( this, Data );
          }
          return RT->runtimeInfo();
        }

      QWidget * edit( QWidget * parent );
      QString acceptable( void );
      void commit( void );

      virtual void * data( void ) 
        { return (void *)&Data; }

      virtual QFile * openFile( const QString & ID );
      short generateFile( const QString & ID, 
                         const QString & Path,
                         QTextStream & TS,
                         long DevNr );

protected :

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );

private :

      PPPEdit * GUI;
      PPPData Data;
      PPPRun * RT;

};

#endif
