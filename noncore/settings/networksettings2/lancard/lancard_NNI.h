#ifndef LANCARD_H
#define LANCARD_H

#include <netnode.h>
#include "lancarddata.h"
#include "lancardrun.h"

class LanCardNetNode;
class LanCardEdit;

class ALanCard : public ANetNodeInstance {

public :

      ALanCard( LanCardNetNode * PNN );

      QWidget * edit( QWidget * parent );
      QString acceptable( void );
      void commit( void );

      RuntimeInfo * runtime( void ) 
        { if( RT == 0 ) 
            RT = new LanCardRun( this, Data );
          return RT;
        }

      virtual void * data( void ) 
        { return (void *)&Data; }

protected :

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );

private :
      
      LanCardEdit * GUI;
      LanCardData Data;
      LanCardRun * RT;

};

#endif
