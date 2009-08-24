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

      RuntimeInfo * runtime( void )
        {  return
           ( RT ) ? RT : ( RT = new LanCardRun( this, Data ) );
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

      LanCardEdit * GUI;
      LanCardData Data;
      LanCardRun * RT;

};

#endif
