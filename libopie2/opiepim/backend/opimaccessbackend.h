#ifndef OPIE_PIM_ACCESS_BACKEND
#define OPIE_PIM_ACCESS_BACKEND

#include <qarray.h>

#include <opie/opimrecord.h>

template <class T = OPimRecord>
class OPimAccessBackend {
public:
    OPimAccessBackend() {
    }
    ~OPimAccessBackend() {
    }
    virtual void load() = 0;
    virtual void reload() = 0;
    virtual void save() = 0;
    virtual QArray<int> allRecords() = 0;
    virtual QArray<int> queryByExample( const T& t, int sort ) = 0;
    virtual T find(int uid ) = 0;
    virtual void clear() = 0;
    virtual bool add( const T& t ) = 0;
    virtual bool remove( int uid ) = 0;
    virtual void replace( const T& t ) = 0;


};

#endif
