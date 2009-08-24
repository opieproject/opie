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

      RuntimeInfo * runtime( void )
        {  return
           ( RT ) ? RT : ( RT = new CableRun( this, Data ) );
        }

      QWidget * edit( QWidget * parent );
      QString acceptable( void );
      void commit( void );

      virtual void * data( void )
        { return (void *)&Data; }

      short generateFileEmbedded( SystemFile & Sf,
                                  long DevNr );
protected :

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );

private :

      CableEdit * GUI;
      CableData Data;
      CableRun * RT;

};

#endif
