
#ifndef OPIE_RECORD_LIST_H
#define OPIE_RECORD_LIST_H

#include <qarray.h>

#include "otemplatebase.h"
#include "opimrecord.h"


template <class T = OPimRecord>
class ORecordListIterator {
public:
    typedef OTemplateBase<T> Base;
    ORecordListIterator( const QArray<int>, const Base* );
    ORecordListIterator();
    ~ORecordListIterator();
    ORecordListIterator( const ORecordListIterator& );
    ORecordListIterator &operator=(const ORecordListIterator& );
    T &operator*();
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

template <class T = OPimRecord >
class ORecordList {
public:
    typedef OTemplateBase<T> Base;
    typedef ORecordListIterator<T> Iterator;
    ORecordList( const QArray<int>& ids,
                 const Base* );
    ~ORecordList();
    Iterator begin();
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
}
template <class T>
ORecordListIterator<T>::~ORecordListIterator() {
/* nothing to delete */
}

template <class T>
ORecordListIterator<T>::ORecordListIterator( const ORecordListIterator<T>& it) {
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
    m_record = it.m_record;

    return *this;
}

template <class T>
T &ORecordListIterator<T>::operator*() {
    if (!m_end )
        m_record = m_temp->find( m_uids[m_current] );
    else
        m_record = T();

    return m_record;
}

template <class T>
ORecordListIterator<T> &ORecordListIterator<T>::operator++() {
    if (m_current < m_uids.count() ) {
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
}
#endif
