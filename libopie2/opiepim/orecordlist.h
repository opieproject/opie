/*
                             This file is part of the Opie Project
                             Copyright (C) The Main Author <main-author@whereever.org>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#ifndef ORECORDLIST_H
#define ORECORDLIST_H

/* OPIE */
#include <opie2/otemplatebase.h>
#include <opie2/opimrecord.h>

/* QT */
#include <qarray.h>

namespace Opie
{

class ORecordListIteratorPrivate;
/**
 * Our List Iterator
 * it behaves like STL or Qt
 *
 * for(it = list.begin(); it != list.end(); ++it )
 *   doSomeCoolStuff( (*it) );
 */
template <class T> class ORecordList;
template <class T = OPimRecord>
class ORecordListIterator
{
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
    ORecordListIterator &operator=( const ORecordListIterator& );

    /**
     * a * operator ;)
     * use it like this T = (*it);
     */
    T operator*();
    ORecordListIterator &operator++();
    ORecordListIterator &operator--();

    bool operator==( const ORecordListIterator& it );
    bool operator!=( const ORecordListIterator& it );

    /**
     * the current item
     */
    uint current() const;

    /**
     * the number of items
     */
    uint count() const;

    /**
     * sets the current item
     */
    void setCurrent( uint cur );

  private:
    QArray<int> m_uids;
    uint m_current;
    const Base* m_temp;
    bool m_end : 1;
    T m_record;
    bool m_direction : 1;

    /* d pointer for future versions */
    ORecordListIteratorPrivate *d;
};


class ORecordListPrivate;
/**
 * The recordlist used as a return type
 * from OPimAccessTemplate
 */
template <class T = OPimRecord >
class ORecordList
{
  public:
    typedef OTemplateBase<T> Base;
    typedef ORecordListIterator<T> Iterator;

    /**
     * c'tor
     */
    ORecordList ()
    {}
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

    /**
     * the number of items in the list
     */
    uint count() const;

    T operator[] ( uint i );
    int uidAt( uint i );

    /**
     * Remove the contact with given uid
     */
    bool remove( int uid );

    /*
      ConstIterator begin()const;
      ConstIterator end()const;
    */
  private:
    QArray<int> m_ids;
    const Base* m_acc;
    ORecordListPrivate *d;
};


/* ok now implement it  */
template <class T>
ORecordListIterator<T>::ORecordListIterator()
{
    m_current = 0;
    m_temp = 0l;
    m_end = true;
    m_record = T();
    /* forward */
    m_direction = TRUE;
}


template <class T>
ORecordListIterator<T>::~ORecordListIterator()
{
    /* nothing to delete */
}


template <class T>
ORecordListIterator<T>::ORecordListIterator( const ORecordListIterator<T>& it )
{
    //    qWarning("ORecordListIterator copy c'tor");
    m_uids = it.m_uids;
    m_current = it.m_current;
    m_temp = it.m_temp;
    m_end = it.m_end;
    m_record = it.m_record;
    m_direction = it.m_direction;
}


template <class T>
ORecordListIterator<T> &ORecordListIterator<T>::operator=( const ORecordListIterator<T>& it )
{
    m_uids = it.m_uids;
    m_current = it.m_current;
    m_temp = it.m_temp;
    m_end = it.m_end;
    m_record = it.m_record;

    return *this;
}


template <class T>
T ORecordListIterator<T>::operator*()
{
    //qWarning("operator* %d %d", m_current,  m_uids[m_current] );
    if ( !m_end )
        m_record = m_temp->find( m_uids[ m_current ], m_uids, m_current,
                                 m_direction ? Base::Forward :
                                 Base::Reverse );
    else
        m_record = T();

    return m_record;
}


template <class T>
ORecordListIterator<T> &ORecordListIterator<T>::operator++()
{
    m_direction = true;
    if ( m_current < m_uids.count() )
    {
        m_end = false;
        ++m_current;
    }
    else
        m_end = true;

    return *this;
}


template <class T>
ORecordListIterator<T> &ORecordListIterator<T>::operator--()
{
    m_direction = false;
    if ( m_current > 0 )
    {
        --m_current;
        m_end = false;
    }
    else
        m_end = true;

    return *this;
}


template <class T>
bool ORecordListIterator<T>::operator==( const ORecordListIterator<T>& it )
{

    /* if both are at we're the same.... */
    if ( m_end == it.m_end ) return true;

    if ( m_uids != it.m_uids ) return false;
    if ( m_current != it.m_current ) return false;
    if ( m_temp != it.m_temp ) return false;

    return true;
}


template <class T>
bool ORecordListIterator<T>::operator!=( const ORecordListIterator<T>& it )
{
    return !( *this == it );
}


template <class T>
ORecordListIterator<T>::ORecordListIterator( const QArray<int> uids,
        const Base* t )
        : m_uids( uids ), m_current( 0 ), m_temp( t ), m_end( false ),
        m_direction( false )
{
    /* if the list is empty we're already at the end of the list */
    if ( uids.count() == 0 )
        m_end = true;
}


template <class T>
uint ORecordListIterator<T>::current() const
{
    return m_current;
}


template <class T>
void ORecordListIterator<T>::setCurrent( uint cur )
{
    if ( cur < m_uids.count() )
    {
        m_end = false;
        m_current = cur;
    }
}
template <class T>
uint ORecordListIterator<T>::count() const
{
    return m_uids.count();
}


template <class T>
ORecordList<T>::ORecordList( const QArray<int>& ids,
                             const Base* acc )
        : m_ids( ids ), m_acc( acc )
{}


template <class T>
ORecordList<T>::~ORecordList()
{
    /* nothing to do here */
}


template <class T>
typename ORecordList<T>::Iterator ORecordList<T>::begin()
{
    Iterator it( m_ids, m_acc );
    return it;
}


template <class T>
typename ORecordList<T>::Iterator ORecordList<T>::end()
{
    Iterator it( m_ids, m_acc );
    it.m_end = true;
    it.m_current = m_ids.count();

    return it;
}


template <class T>
uint ORecordList<T>::count() const
{
    return m_ids.count();
}


template <class T>
T ORecordList<T>::operator[] ( uint i )
{
    if ( i >= m_ids.count() )
        return T();
    /* forward */
    return m_acc->find( m_ids[ i ], m_ids, i );
}


template <class T>
int ORecordList<T>::uidAt( uint i )
{
    return m_ids[ i ];
}


template <class T>
bool ORecordList<T>::remove( int uid )
{
    QArray<int> copy( m_ids.count() );
    int counter = 0;
    bool ret_val = false;

    for ( uint i = 0; i < m_ids.count(); i++ )
    {
        if ( m_ids[ i ] != uid )
        {
            copy[ counter++ ] = m_ids[ i ];

        }
        else
            ret_val = true;
    }

    copy.resize( counter );
    m_ids = copy;


    return ret_val;
}

}
#endif
