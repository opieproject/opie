#ifndef __NSDATA_H
#define __NSDATA_H

#include <netnode.h>

class NetworkSettingsData {

public :

      NetworkSettingsData( void );
      ~NetworkSettingsData( void );

      void loadSettings( void );
      QString saveSettings( void );

      QString generateSettings( void );

      bool isModified( void );
      inline void setModified( bool M ) 
        { ForceModified = M; }

      QList<NodeCollection> collectPossible( const char * Interface );
      // return TRUE if we need gui to decide
      bool canStart( const char * Interface );

private :

      bool ForceModified;

      // collect strings in config file nobody wants
      QStringList LeftOvers;
};

#endif
