#include <fcntl.h>
#include <unistd.h>
#include "cablerun.h"

State_t CableRun::detectState( void ) { 

      int fd = open( Data->Device.latin1(), O_RDWR );

      if( fd < 0 ) {
        return Unavailable;
      }
      close( fd );
      return Available;
}

QString CableRun::setMyState( NodeCollection *, Action_t , bool ) { 
      return QString();
}

QString CableRun::deviceFile( void ) {
      return Data->Device;
}
