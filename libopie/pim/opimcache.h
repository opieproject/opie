#ifndef OPIE_PIM_CACHE_H
#define OPIE_PIM_CACHE_H

#include <qintcache.h>

#include "opimrecord.h"

template <class T = OPimRecord>
class OPimCacheItem {
public:
    OPimCacheItem( const T& t = T() );
    OPimCacheItem( const OPimCacheItem& );
    ~OPimCacheItem();

    OPimCacheItem &operator=( const OPimCacheItem& );

    T record()const;
    void setRecord( const T& );
private:
    T m_t;
};

/**
 * OPimCache for caching the items
 * We support adding, removing
 * and finding
 */
template <class T = OPimRecord>
class OPimCache {
public:
    typedef OPimCacheItem<T> Item;
    OPimCache();
    OPimCache( const OPimCache& );
    ~OPimCache();

    OPimCache &operator=( const OPimCache& );

    bool contains(int uid)const;
    void invalidate();
    void setSize( int size );

    T find(int uid )const;
    void add( const T& );
    void remove( int uid );
    void replace( const T& );

private:
    QIntCache<Item> m_cache;
};

// Implementation
template <class T>
OPimCacheItem<T>::OPimCacheItem( const T& t )
    : m_t(t) {
}
template <class T>
OPimCacheItem<T>::~OPimCacheItem() {

}
template <class T>
T OPimCacheItem<T>::record()const {
    return m_t;
}
template <class T>
void OPimCacheItem<T>::setRecord( const T& t ) {
    m_t = t;
}
// Cache
template <class T>
OPimCache<T>::OPimCache()
    : m_cache(100, 53 )
{
    m_cache.setAutoDelete( TRUE );
}
template <class T>
OPimCache<T>::~OPimCache() {

}
template <class T>
bool OPimCache<T>::contains(int uid )const {
    Item* it = m_cache.find( uid, FALSE );
    if (!it)
        return false;
    return true;
}
template <class T>
void OPimCache<T>::invalidate() {
    m_cache.clear();
}
template <class T>
void OPimCache<T>::setSize( int size ) {
    m_cache.setMaxCost( size );
}
template <class T>
T OPimCache<T>::find(int uid )const {
    Item *it = m_cache.find( uid );
    if (it)
        return it->record();
    return T();
}
template <class T>
void OPimCache<T>::add( const T& t ) {
    Item* it = 0l;
    it = m_cache.find(t.uid(), FALSE );

    if (it )
        it->setRecord( t );

    it = new Item( t );
    if (!m_cache.insert( t.uid(), it ) )
        delete it;
}
template <class T>
void OPimCache<T>::remove( int uid ) {
    m_cache.remove( uid );
}
template <class T>
void OPimCache<T>::replace( const T& t) {
    Item *it = m_cache.find( t.uid() );
    if ( it ) {
        it->setRecord( t );
    }
}

#endif
