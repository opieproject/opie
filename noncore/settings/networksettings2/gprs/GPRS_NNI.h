#ifndef GPRS_H
#define GPRS_H

#include <netnode.h>
#include "GPRSdata.h"
#include "GPRSrun.h"

class GPRSNetNode;
class GPRSEdit;
class SystemFile;

class AGPRSDevice : public ANetNodeInstance{

public :

      AGPRSDevice( GPRSNetNode * PNN );

      RuntimeInfo * runtime( void ) 
        { return
           ( RT ) ? RT : ( RT = new GPRSRun( this, Data ) );
        }

      QWidget * edit( QWidget * parent );
      QString acceptable( void );
      void commit( void );

      virtual bool openFile( SystemFile & SF,
                             QStringList & SL );

      virtual void * data( void ) 
        { return (void *)&Data; }

      virtual bool hasDataForFile( SystemFile & S );
      virtual short generateFile( SystemFile & SF,
                                 long DevNr );

protected :

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );

private :

      GPRSEdit * GUI;
      GPRSData Data;
      GPRSRun * RT;
};

#endif
