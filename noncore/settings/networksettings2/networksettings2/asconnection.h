#ifndef ASCONNECTION_H
#define ASCONNECTION_H

#include <resources.h>

// pure virtual (component oriented) interface of any
// plugin that offers a networkSetup
class AsNetworkSetup : public RuntimeInfo {

public :

    AsNetworkSetup( ANetNodeInstance * NNI ) :
        RuntimeInfo( NNI ) {
    }

};

#endif
