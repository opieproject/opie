#ifndef OPIE_TEMPLATE_BASE_H
#define OPIE_TEMPLATE_BASE_H

#include <qarray.h>

#include "opimrecord.h"

/**
 * internal template base
 */
template <class T = OPimRecord>
class OTemplateBase {
public:
    enum CacheDirection { Forward=0, Reverse };
    OTemplateBase() {
    };
    virtual ~OTemplateBase() {
    }
    virtual T find( int uid )const = 0;

    /**
     * read ahead find
     */
    virtual T find( int uid, const QArray<int>& items,
                    uint current, CacheDirection dir = Forward )const = 0;
    virtual void cache( const T& )const = 0;
    virtual void setSaneCacheSize( int ) = 0;

};


#endif
