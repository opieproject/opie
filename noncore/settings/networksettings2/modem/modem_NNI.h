#ifndef Modem_H
#define Modem_H

#include <netnode.h>
#include "modemdata.h"
#include "modemrun.h"

class ModemNetNode;
class ModemEdit;

class AModem : public ANetNodeInstance {

public :

      AModem( ModemNetNode * PNN );

      RuntimeInfo * runtime( void ) 
        {  return 
           ( RT ) ? RT : ( RT = new ModemRun( this, Data ) );
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

      ModemEdit * GUI;
      ModemData Data;
      ModemRun * RT;

};

#endif
