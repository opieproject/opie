#ifndef ASDEVICE_H
#define ASDEVICE_H

#include "system.h"
#include "netnode.h"

// pure virtual (component oriented) interface of any
// plugin that offers a device
class AsDevice : public RuntimeInfo {

public :

    AsDevice( ANetNodeInstance * NNI ) :
        RuntimeInfo( NNI ) { 
      AssignedInterface = 0;
    }

    // return the interface assigned to this device
    // e.g eth0, wlan, ...
    InterfaceInfo * assignedInterface( void )
      { return AssignedInterface; }
    virtual void assignInterface( InterfaceInfo * NI )
      { AssignedInterface = NI; }

    // number of device to configure for this Device type
    virtual long count( void )
      { return 1; }
    // generate NIC name of device number ...
    virtual QString genNic( long NicNr ) = 0;

private :

    InterfaceInfo * AssignedInterface;

};

#endif
