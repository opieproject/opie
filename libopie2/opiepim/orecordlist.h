
#ifndef OPIE_RECORD_LIST_H
#define OPIE_RECORD_LIST_H

#include <opie/opimaccesstemplate.h>
#include <opie/opimrecord.h>

template <class T = OPimRecord >
class ORecordList {
public:
    class Iterator {
        friend class ORecordList;
    public:
        Iterator();
        ~Iterator();
        Iterator(const Iterator& );
        Iterator &operator=(const Iterator& );
        T &operator*();
        Iterator &operator++();
        Iterator &operator--();

        bool operator==( const Iterator& it );
        bool operator!=( const Iterator& it );

    private:
        Iterator( const QArray<int>,
                  OPimAccessTemplate<T>* );

        QArray<int> m_uids;
        int m_current;
        OPimAccessTemplate* m_temp;
        bool m_end : 1;
        T m_record;

        /* d pointer for future versions */
        class IteratorPrivate;
        IteratorPrivate *d;
    };
    ORecordList( const QArray<int>& ids,
                 OPimAccessTemplate<T>* acc );
    ~ORecordList();
    Iterator begin();
    Iterator end();
    /*
      ConstIterator begin()const;
      ConstIterator end()const;
    */
private:
    QArray<int> m_ids;
    OPimAccessTemplate<T>* m_acc;
};

/* ok now implement it  */
template <class T= OPimRecord>
ORecordList<T>::Iterator::Iterator() {
    m_current = 0;
    m_temp = 0l;
    m_end = true;
}
template <class T= OPimRecord>
ORecordList<T>::Iterator::~Iterator() {
/* nothing to delete */
}

template <class T = OPimRecord>
ORecordList<T>::Iterator::Iterator( const ORecordList<T>::Iterator& it) {
    m_uids = it.m_uids;
    m_current = it.m_current;
    m_temp = it.m_temp;
    m_end = it.m_end;
    m_record = it.m_record;
}

template <class T = OPimRecord>
ORecordList<T>::Iterator &ORecordList::Iterator::operator=( const ORecordList<T>::Iterator& it) {
    m_uids = it.m_uids;
    m_current = it.m_current;
    m_temp = it.m_temp;
    m_end = it.m_end;
    m_record = it.m_record;

    return *this;
}

template <class T = OPimRecord>
T &ORecordList<T>::Iterator::operator*() {
    if (!m_end )
        m_record = m_temp->find( m_uids[m_current] );
    else
        m_record = T;

    return m_record;
}

template <class T = OPimRecord>
ORecordList<T>::Iterator &ORecordList<T>::Iterator::operator++() {
    if (m_current < m_uids.count() ) {
        m_end = false;
        ++m_current;
    }else
        m_end = true;

    return *this;
}
template <class T = OPimRecord>
ORecordList<T>::Iterator &ORecordList<T>::Iterator::operator--() {
    if ( m_current > 0 ) {
        --m_current;
        m_end = false;
    }  else
        m_end = true;

    return *this;
}

template <class T = OPimRecord>
bool ORecordList<T>::Iterator::operator==( const ORecordList<T>::Iterator& it ) {

    /* if both are at we're the same.... */
    if ( m_end == it.m_end ) return true;

    if ( m_uids != it.m_uids ) return false;
    if ( m_current != it.m_current ) return false;
    if ( m_temp != it.m_temp ) return false;

    return true;
}
template <class T = ORecordList>
bool ORecordList<T>::Iterator::operator!=( const ORecordList<T>::Iterator it ) {
    return !(*this == it );
}
template <class T = ORecordList>
ORecordList<T>::Iterator::Iterator( const QArray<int> uids,
                                    OPimAccessTemplate<T>* t )
    : m_uids( uids ), m_current( 0 ),  m_temp( t ), m_end( false )
{
}
template <class T = ORecordList>
ORecordList<T>::ORecordList( const QArray<int>& ids,
                             OPimAccessTemplate<T>* acc )
    : m_ids( ids ), m_acc( acc )
{
}
template <class T = ORecordList>
ORecordList<T>::~ORecordList() {
/* nothing to do here */
}
template <class T = ORecordList>
ORecordList<T>::Iterator ORecordList<T>::begin() {
    Iterator it( m_ids, m_acc );
    return it;
}
template <class T = ORecordList>
ORecordList<T>::Iterator ORecordList<T>::end() {
    Iterator it( m_ids, m_acc );
    it.m_end = true;
    it.m_current = m_ids.count();
}
#endif
