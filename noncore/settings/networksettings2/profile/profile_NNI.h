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

      QWidget * edit( QWidget * parent );
      QString acceptable( void );
      void commit( void );

      const QString & description( void ) 
        { return Data.Description; }
      RuntimeInfo * runtime( void ) 
        { if( RT == 0 ) 
            RT = new ProfileRun( this, Data );
          return RT;
        }

      virtual void * data( void ) 
        { return (void *)&Data; }

protected :

      virtual void setSpecificAttribute( QString & Attr, QString & Value );
      virtual void saveSpecificAttribute( QTextStream & TS );
      
private :

      ProfileEdit * GUI;
      ProfileData Data;
      ProfileRun * RT;

};

#endif
