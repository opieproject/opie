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

private :

    InterfaceInfo * AssignedInterface;

};

#endif
