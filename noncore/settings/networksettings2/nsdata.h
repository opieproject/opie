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

      QList<NetworkSetup> collectPossible( const QString & Interface );
      // return TRUE if we need gui to decide
      bool canStart( const QString & Interface );
      // return TRUE if there are vpns that could be triggered
      // by this interface
      bool couldBeTriggered( const QString & Interface );

private :

      QList<NetworkSetup> collectTriggered( const QString &Interface );
      bool ForceModified;

      // collect strings in config file nobody wants
      QStringList LeftOvers;
};

#endif
