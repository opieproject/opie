#ifndef ASFULLSETUP_H
#define ASFULLSETUP_H

#include <netnode.h>

// pure virtual (component oriented) interface of any
// plugin that offers a full setup
class AsFullSetup : public RuntimeInfo {

public :

    AsFullSetup( ANetNodeInstance * NNI ) :
        RuntimeInfo( NNI ) { 
    }

    virtual const QString & description( void ) = 0;

};

#endif
