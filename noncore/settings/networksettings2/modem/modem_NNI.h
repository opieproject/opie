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

      QWidget * edit( QWidget * parent );
      QString acceptable( void );
      void commit( void );

      RuntimeInfo * runtime( void ) 
        { if( RT == 0 ) 
            RT = new ModemRun( this, Data );
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

      ModemEdit * GUI;
      ModemData Data;
      ModemRun * RT;

};

#endif
