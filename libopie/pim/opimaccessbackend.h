#ifndef OPIE_PIM_ACCESS_BACKEND
#define OPIE_PIM_ACCESS_BACKEND

#include <qarray.h>

#include <opie/otemplatebase.h>
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
    typedef OTemplateBase<T> Frontend;

    /** The access hint from the frontend */
    OPimAccessBackend(int access = 0);
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

    virtual T find(int uid, const QArray<int>& items,
                   uint current, typename Frontend::CacheDirection )const ;
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

    /*
     * setTheFrontEnd!!!
     */
    void setFrontend( Frontend* front );

    /**
     * set the read ahead count
     */
    void setReadAhead( uint count );
protected:
    int access()const;
    void cache( const T& t )const;

    /**
     * use a prime number here!
     */
    void setSaneCacheSize( int );

    uint readAhead()const;

private:
    class Private;
    Private* d;
    Frontend* m_front;
    uint m_read;
    int m_acc;

};

template <class T>
OPimAccessBackend<T>::OPimAccessBackend(int acc)
    : m_acc( acc )
{
    m_front = 0l;
}
template <class T>
OPimAccessBackend<T>::~OPimAccessBackend() {

}
template <class T>
void OPimAccessBackend<T>::setFrontend( Frontend* fr ) {
    m_front = fr;
}
template <class T>
void OPimAccessBackend<T>::cache( const T& t )const {
    if (m_front )
        m_front->cache( t );
}
template <class T>
void OPimAccessBackend<T>::setSaneCacheSize( int size) {
    if (m_front )
        m_front->setSaneCacheSize( size );
}
template <class T>
T OPimAccessBackend<T>::find( int uid, const QArray<int>&,
                              uint, typename Frontend::CacheDirection )const {
    return find( uid );
}
template <class T>
void OPimAccessBackend<T>::setReadAhead( uint count ) {
    m_read = count;
}
template <class T>
uint OPimAccessBackend<T>::readAhead()const {
    return m_read;
}
template <class T>
int OPimAccessBackend<T>::access()const {
    return m_acc;
}
#endif
