#ifndef OPIE_TEMPLATE_BASE_H
#define OPIE_TEMPLATE_BASE_H

#include "opimrecord.h"

template <class T = OPimRecord>
class OTemplateBase {
public:
    OTemplateBase() {
    };
    virtual ~OTemplateBase() {
    }
    virtual T find( int uid ) = 0;

};


#endif
