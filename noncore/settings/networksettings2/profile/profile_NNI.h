#ifndef PROFILE_H
#define PROFILE_H

#include <netnode.h>
#include "profiledata.h"
#include "profilerun.h"

class ProfileNetNode;
class ProfileEdit;

class AProfile : public ANetNodeInstance {

public :

      AProfile( ProfileNetNode * PNN );

      RuntimeInfo * runtime( void ) 
        { return
            ( RT ) ? RT : ( RT = new ProfileRun( this, Data ) );
        }

      QWidget * edit( QWidget * parent );
      QString acceptable( void );
      void commit( void );

      virtual void * data( void ) 
        { return (void *)&Data; }

      virtual short generateFileEmbedded( SystemFile & TS,
                                         long DevNr );

      const QString & description( void ) 
        { return Data.Description; }

protected :

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );
      
private :

      ProfileEdit * GUI;
      ProfileData Data;
      ProfileRun * RT;

};

#endif
