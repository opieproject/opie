#ifndef OPIE_PIM_ACCESS_BACKEND
#define OPIE_PIM_ACCESS_BACKEND

#include <qarray.h>

#include <opie/opimrecord.h>


/**
 * OPimAccessBackend is the base class
 * for all private backends
 * it operates on OPimRecord as the base class
 * and it's responsible for fast manipulating
 * the resource the implementation takes care
 *  of
 */
template <class T = OPimRecord>
class OPimAccessBackend {
public:
    OPimAccessBackend();
    virtual ~OPimAccessBackend();

    /**
     * load the resource
     */
    virtual bool load() = 0;

    /**
     * reload the resource
     */
    virtual bool reload() = 0;

    /**
     * save the resource and
     * all it's changes
     */
    virtual bool save() = 0;

    /**
     * return an array of
     * all available uids
     */
    virtual QArray<int> allRecords()const = 0;

    /**
     * queryByExample for T with the SortOrder
     * sort
     */
    virtual QArray<int> queryByExample( const T& t, int sort ) = 0;

    /**
     * find the OPimRecord with uid @param uid
     * returns T and T.isEmpty() if nothing was found
     */
    virtual T find(int uid )const  = 0;

    /**
     * clear the back end
     */
    virtual void clear() = 0;

    /**
     * add T
     */
    virtual bool add( const T& t ) = 0;

    /**
     * remove
     */
    virtual bool remove( int uid ) = 0;

    /**
     * replace a record with T.uid()
     */
    virtual bool replace( const T& t ) = 0;


};

template <class T>
OPimAccessBackend<T>::OPimAccessBackend() {

}
template <class T>
OPimAccessBackend<T>::~OPimAccessBackend() {

}

#endif
