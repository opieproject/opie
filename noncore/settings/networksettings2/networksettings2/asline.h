#ifndef ASLINE_H
#define ASLINE_H

#include <netnode.h>

// pure virtual (component oriented) interface of any
// plugin that offers a line
class AsLine : public RuntimeInfo {

public :

    AsLine( ANetNodeInstance * NNI ) :
        RuntimeInfo( NNI ) { 
    }

};

#endif
