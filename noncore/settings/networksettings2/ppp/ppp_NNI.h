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
        { return (RT) ? RT : (RT = new PPPRun( this, Data ) ); }

      QWidget * edit( QWidget * parent );
      QString acceptable( void );
      void commit( void );

      virtual void * data( void )
        { return (void *)&Data; }

      virtual bool openFile( SystemFile & Sf, QStringList &SL );
      short generateFile( SystemFile & TS,
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
