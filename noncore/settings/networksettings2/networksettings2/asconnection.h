#ifndef ASCONNECTION_H
#define ASCONNECTION_H

#include <resources.h>

// pure virtual (component oriented) interface of any
// plugin that offers a connection
class AsConnection : public RuntimeInfo {

public :

    AsConnection( ANetNodeInstance * NNI ) :
        RuntimeInfo( NNI ) { 
    }

};

#endif
