#include <fcntl.h>
#include <unistd.h>
#include "cablerun.h"

void CableRun::detectState( NodeCollection * NC ) { 

      int fd = open( D->Device.latin1(), O_RDWR );

      if( fd < 0 ) {
        NC->setCurrentState( Unavailable );
      }
      close( fd );
      NC->setCurrentState( Available );
}

bool CableRun::setState( NodeCollection * NC, Action_t A ) { 
      if( A == Activate ) {
        detectState(NC);
        return (NC->currentState() == Available);
      }
      return 1;
}

bool CableRun::canSetState( State_t , Action_t  ) { 
      return 1;
}
