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

      RuntimeInfo * runtime( void )
        {  return 
           ( RT ) ? RT : ( RT = new IRDARun( this, Data ) );
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

      IRDAEdit * GUI;
      IRDAData Data;
      IRDARun * RT;

};

#endif
