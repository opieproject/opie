#ifndef __NSDATA_H
#define __NSDATA_H

#include "netnode.h"

class NetworkSettingsData {

public :

      NetworkSettingsData( void );
      ~NetworkSettingsData( void );

      void loadSettings( void );
      QString saveSettings( void );

      QString generateSettings( void );

      bool isModified( void ) 
        { return IsModified; }
      void setModified( bool m ) 
        { IsModified = m; }

      QList<NodeCollection> collectPossible( const char * Interface );
      // return TRUE if we need gui to decide
      bool canStart( const char * Interface );
      bool regenerate( void );

private :

      bool IsModified;

      // collect strings in config file nobody wants
      QStringList LeftOvers;

};

#endif
