#ifndef OPIE_PIM_ACCESS_TEMPLATE_H
#define OPIE_PIM_ACCESS_TEMPLATE_H

#include <qarray.h>

#include <opie/opimrecord.h>
#include <opie/opimaccessbackend.h>
#include <opie/orecordlist.h>

#include "opimcache.h"
#include "otemplatebase.h"

/**
 * Thats the frontend to our OPIE PIM
 * Library. Either you want to use it's
 * interface or you want to implement
 * your own Access lib
 * Just create a OPimRecord and inherit from
 * the plugins
 */

template <class T = OPimRecord >
class OPimAccessTemplate : public OTemplateBase<T> {
public:
    typedef ORecordList<T> List;
    typedef OPimAccessBackend<T> BackEnd;
    typedef OPimCache<T> Cache;

    /**
     * c'tor BackEnd
     */
    OPimAccessTemplate( BackEnd* end);
    virtual ~OPimAccessTemplate();

    /**
     * load from the backend
     */
    virtual bool load();

    /** Reload database.
     * You should execute this function if the external database
     * was changed.
     * This function will load the external database and afterwards
     * rejoin the local changes. Therefore the local database will be set consistent.
     */
    virtual bool reload();

    /** Save contacts database.
     * Save is more a "commit". After calling this function, all changes are public available.
     * @return true if successful
     */
    virtual bool save();

    /**
     * if the resource was changed externally
     * You should use the signal handling instead of polling possible changes !
     * zecke: Do you implement a signal for otodoaccess ?
     */
    bool wasChangedExternally()const;

    /**
     * return a List of records
     * you can iterate over them
     */
    virtual List allRecords()const;

    /**
     * queryByExample.
     * @see otodoaccess, ocontactaccess
     */
    virtual List queryByExample( const T& t, int querySettings );

    /**
     * find the OPimRecord uid
     */
    virtual T find( int uid )const;

    /**
     * read ahead cache find method ;)
     */
    virtual T find( int uid, const QArray<int>&,
                    uint current, CacheDirection dir = Forward )const;

    /* invalidate cache here */
    /**
     * clears the backend and invalidates the backend
     */
    virtual void clear() ;

    /**
     * add T to the backend
     * @param t The item to add.
     * @return <i>true</i> if added successfully.
     */
    virtual bool add( const T& t ) ;

    /* only the uid matters */
    /**
     * remove T from the backend
     * @param t The item to remove
     * @return <i>true</i> if successful.
     */
    virtual bool remove( const T& t );

    /**
     * remove the OPimRecord with uid
     * @param uid The ID of the item to remove
     * @return <i>true</i> if successful.
     */
    virtual bool remove( int uid );

    /**
     * replace T from backend
     * @param t The item to replace
     * @return <i>true</i> if successful.
     */
    virtual bool replace( const T& t) ;

    void setReadAhead( uint count );
    /**
     * @internal
     */
    void cache( const T& )const;
    void setSaneCacheSize( int );
protected:
    /**
     * invalidate the cache
     */
    void invalidateCache();

    void setBackEnd( BackEnd* end );
    /**
     * returns the backend
     */
    BackEnd* backEnd();
    BackEnd* m_backEnd;
    Cache m_cache;

};

template <class T>
OPimAccessTemplate<T>::OPimAccessTemplate( BackEnd* end )
    : OTemplateBase<T>(), m_backEnd( end )
{
    if (end )
        end->setFrontend( this );
}
template <class T>
OPimAccessTemplate<T>::~OPimAccessTemplate() {
    qWarning("~OPimAccessTemplate<T>");
    delete m_backEnd;
}
template <class T>
bool OPimAccessTemplate<T>::load() {
    invalidateCache();
    return m_backEnd->load();
}
template <class T>
bool OPimAccessTemplate<T>::reload() {
	invalidateCache();  // zecke: I think this should be added (se)
	return m_backEnd->reload();
}
template <class T>
bool OPimAccessTemplate<T>::save() {
    return m_backEnd->save();
}
template <class T>
OPimAccessTemplate<T>::List OPimAccessTemplate<T>::allRecords()const {
    QArray<int> ints = m_backEnd->allRecords();
    List lis(ints, this );
    return lis;
}
template <class T>
OPimAccessTemplate<T>::List
OPimAccessTemplate<T>::queryByExample( const T& t, int sortOrder ) {
    QArray<int> ints = m_backEnd->queryByExample( t, sortOrder );

    List lis(ints, this );
    return lis;
}
template <class T>
T OPimAccessTemplate<T>::find( int uid ) const{
    T t = m_backEnd->find( uid );
    cache( t );
    return t;
}
template <class T>
T OPimAccessTemplate<T>::find( int uid, const QArray<int>& ar,
                               uint current, CacheDirection dir )const {
    /*
     * better do T.isEmpty()
     * after a find this way we would
     * avoid two finds in QCache...
     */
    // qWarning("find it now %d", uid );
    if (m_cache.contains( uid ) ) {
        return m_cache.find( uid );
    }

    T t = m_backEnd->find( uid, ar, current, dir );
    cache( t );
    return t;
}
template <class T>
void OPimAccessTemplate<T>::clear() {
    invalidateCache();
    m_backEnd->clear();
}
template <class T>
bool OPimAccessTemplate<T>::add( const T& t ) {
    cache( t );
    return m_backEnd->add( t );
}
template <class T>
bool OPimAccessTemplate<T>::remove( const T& t ) {
    return remove( t.uid() );
}
template <class T>
bool OPimAccessTemplate<T>::remove( int uid ) {
    m_cache.remove( uid );
    return m_backEnd->remove( uid );
}
template <class T>
bool OPimAccessTemplate<T>::replace( const T& t ) {
    m_cache.replace( t );
    return m_backEnd->replace( t );
}
template <class T>
void OPimAccessTemplate<T>::invalidateCache() {
    m_cache.invalidate();
}
template <class T>
OPimAccessTemplate<T>::BackEnd* OPimAccessTemplate<T>::backEnd() {
    return m_backEnd;
}
template <class T>
bool OPimAccessTemplate<T>::wasChangedExternally()const {
    return false;
}
template <class T>
void OPimAccessTemplate<T>::setBackEnd( BackEnd* end ) {
    m_backEnd = end;
    if (m_backEnd )
        m_backEnd->setFrontend( this );
}
template <class T>
void OPimAccessTemplate<T>::cache( const T& t ) const{
    /* hacky we need to work around the const*/
    ((OPimAccessTemplate<T>*)this)->m_cache.add( t );
}
template <class T>
void OPimAccessTemplate<T>::setSaneCacheSize( int size ) {
    m_cache.setSize( size );
}
template <class T>
void OPimAccessTemplate<T>::setReadAhead( uint count ) {
    m_backEnd->setReadAhead( count );
}
#endif
