
#ifndef OPIE_RECORD_LIST_H
#define OPIE_RECORD_LIST_H

#include <qarray.h>

#include "otemplatebase.h"
#include "opimrecord.h"

/**
 * Our List Iterator
 * it behaves like STL or Qt
 *
 * for(it = list.begin(); it != list.end(); ++it )
 *   doSomeCoolStuff( (*it) );
 */
template <class T> class ORecordList;
template <class T = OPimRecord>
class ORecordListIterator {
    friend class ORecordList<T>;
public:
    typedef OTemplateBase<T> Base;

    /**
     * The c'tor used internally from
     * ORecordList
     */
    ORecordListIterator( const QArray<int>, const Base* );

    /**
     * The standard c'tor
     */
    ORecordListIterator();
    ~ORecordListIterator();

    ORecordListIterator( const ORecordListIterator& );
    ORecordListIterator &operator=(const ORecordListIterator& );

    /**
     * a * operator ;)
     * use it like this T = (*it);
     */
    T operator*();
    ORecordListIterator &operator++();
    ORecordListIterator &operator--();

    bool operator==( const ORecordListIterator& it );
    bool operator!=( const ORecordListIterator& it );

private:
    QArray<int> m_uids;
    int m_current;
    const Base* m_temp;
    bool m_end : 1;
    T m_record;

    /* d pointer for future versions */
    class IteratorPrivate;
    IteratorPrivate *d;
};
/**
 * The recordlist used as a return type
 * from OPimAccessTemplate
 */
template <class T = OPimRecord >
class ORecordList {
public:
    typedef OTemplateBase<T> Base;
    typedef ORecordListIterator<T> Iterator;

    /**
     * c'tor
     */
    ORecordList( const QArray<int>& ids,
                 const Base* );
    ~ORecordList();

    /**
     * the first iterator
     */
    Iterator begin();

    /**
     * the end
     */
    Iterator end();
    /*
      ConstIterator begin()const;
      ConstIterator end()const;
    */
private:
    QArray<int> m_ids;
    const Base* m_acc;
};

/* ok now implement it  */
template <class T>
ORecordListIterator<T>::ORecordListIterator() {
    m_current = 0;
    m_temp = 0l;
    m_end = true;
    m_record = T();
}
template <class T>
ORecordListIterator<T>::~ORecordListIterator() {
/* nothing to delete */
}

template <class T>
ORecordListIterator<T>::ORecordListIterator( const ORecordListIterator<T>& it) {
//    qWarning("ORecordListIterator copy c'tor");
    m_uids = it.m_uids;
    m_current = it.m_current;
    m_temp = it.m_temp;
    m_end = it.m_end;
    m_record = it.m_record;
}

template <class T>
ORecordListIterator<T> &ORecordListIterator<T>::operator=( const ORecordListIterator<T>& it) {
    m_uids = it.m_uids;
    m_current = it.m_current;
    m_temp = it.m_temp;
    m_end = it.m_end;
//    m_record = it.m_record;

    return *this;
}

template <class T>
T ORecordListIterator<T>::operator*() {
//    qWarning("operator* %d %d", m_current,  m_uids[m_current] );
    if (!m_end )
        m_record = m_temp->find( m_uids[m_current] );
    else
        m_record = T();

    return m_record;
}

template <class T>
ORecordListIterator<T> &ORecordListIterator<T>::operator++() {
    if (m_current < (int)m_uids.count() ) {
        m_end = false;
        ++m_current;
    }else
        m_end = true;

    return *this;
}
template <class T>
ORecordListIterator<T> &ORecordListIterator<T>::operator--() {
    if ( m_current > 0 ) {
        --m_current;
        m_end = false;
    }  else
        m_end = true;

    return *this;
}

template <class T>
bool ORecordListIterator<T>::operator==( const ORecordListIterator<T>& it ) {

    /* if both are at we're the same.... */
    if ( m_end == it.m_end ) return true;

    if ( m_uids != it.m_uids ) return false;
    if ( m_current != it.m_current ) return false;
    if ( m_temp != it.m_temp ) return false;

    return true;
}
template <class T>
bool ORecordListIterator<T>::operator!=( const ORecordListIterator<T>& it ) {
    return !(*this == it );
}
template <class T>
ORecordListIterator<T>::ORecordListIterator( const QArray<int> uids,
                                  const Base* t )
    : m_uids( uids ), m_current( 0 ),  m_temp( t ), m_end( false )
{
}

template <class T>
ORecordList<T>::ORecordList( const QArray<int>& ids,
                             const Base* acc )
    : m_ids( ids ), m_acc( acc )
{
}
template <class T>
ORecordList<T>::~ORecordList() {
/* nothing to do here */
}
template <class T>
ORecordList<T>::Iterator ORecordList<T>::begin() {
    Iterator it( m_ids, m_acc );
    return it;
}
template <class T>
ORecordList<T>::Iterator ORecordList<T>::end() {
    Iterator it( m_ids, m_acc );
    it.m_end = true;
    it.m_current = m_ids.count();

    return it;
}
#endif
