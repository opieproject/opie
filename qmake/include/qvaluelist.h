/****************************************************************************
** $Id: qvaluelist.h,v 1.1 2002-11-01 00:10:43 kergoth Exp $
**
** Definition of QValueList class
**
** Created : 990406
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the tools module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QVALUELIST_H
#define QVALUELIST_H

#ifndef QT_H
#include "qtl.h"
#include "qshared.h"
#include "qdatastream.h"
#endif // QT_H

#ifndef QT_NO_STL
#include <iterator>
#include <list>
#endif

//#define QT_CHECK_VALUELIST_RANGE

#if defined(Q_CC_MSVC)
#pragma warning(disable:4284) // "return type for operator -> is not a UDT"
#endif

template <class T>
class QValueListNode
{
public:
    QValueListNode( const T& t ) : data( t ) { }
    QValueListNode() { }
#if defined(Q_TEMPLATEDLL)
    // Workaround MS bug in memory de/allocation in DLL vs. EXE
    virtual ~QValueListNode() { }
#endif

    QValueListNode<T>* next;
    QValueListNode<T>* prev;
    T data;
};

template<class T>
class QValueListIterator
{
 public:
    /**
     * Typedefs
     */
    typedef QValueListNode<T>* NodePtr;
#ifndef QT_NO_STL
    typedef std::bidirectional_iterator_tag  iterator_category;
#endif
    typedef T        value_type;
    typedef size_t size_type;
#ifndef QT_NO_STL
    typedef ptrdiff_t  difference_type;
#else
    typedef int difference_type;
#endif
    typedef T*   pointer;
    typedef T& reference;

    /**
     * Variables
     */
    NodePtr node;

    /**
     * Functions
     */
    QValueListIterator() : node( 0 ) {}
    QValueListIterator( NodePtr p ) : node( p ) {}
    QValueListIterator( const QValueListIterator<T>& it ) : node( it.node ) {}

    bool operator==( const QValueListIterator<T>& it ) const { return node == it.node; }
    bool operator!=( const QValueListIterator<T>& it ) const { return node != it.node; }
    const T& operator*() const { return node->data; }
    T& operator*() { return node->data; }
    // UDT for T = x*
    // T* operator->() const { return &node->data; }

    QValueListIterator<T>& operator++() {
	node = node->next;
	return *this;
    }

    QValueListIterator<T> operator++(int) {
	QValueListIterator<T> tmp = *this;
	node = node->next;
	return tmp;
    }

    QValueListIterator<T>& operator--() {
	node = node->prev;
	return *this;
    }

    QValueListIterator<T> operator--(int) {
	QValueListIterator<T> tmp = *this;
	node = node->prev;
	return tmp;
    }
    
    QValueListIterator<T>& operator+=( int j ) {
	while ( j-- )
	    node = node->next;
	return *this;
    }

    QValueListIterator<T>& operator-=( int j ) {
	while ( j-- )
	    node = node->prev;
	return *this;
    }

};

template<class T>
class QValueListConstIterator
{
 public:
    /**
     * Typedefs
     */
    typedef QValueListNode<T>* NodePtr;
#ifndef QT_NO_STL
    typedef std::bidirectional_iterator_tag  iterator_category;
#endif
    typedef T        value_type;
    typedef size_t size_type;
#ifndef QT_NO_STL
    typedef ptrdiff_t  difference_type;
#else
    typedef int difference_type;
#endif
    typedef const T*   pointer;
    typedef const T& reference;

    /**
     * Variables
     */
    NodePtr node;

    /**
     * Functions
     */
    QValueListConstIterator() : node( 0 ) {}
    QValueListConstIterator( NodePtr p ) : node( p ) {}
    QValueListConstIterator( const QValueListConstIterator<T>& it ) : node( it.node ) {}
    QValueListConstIterator( const QValueListIterator<T>& it ) : node( it.node ) {}

    bool operator==( const QValueListConstIterator<T>& it ) const { return node == it.node; }
    bool operator!=( const QValueListConstIterator<T>& it ) const { return node != it.node; }
    const T& operator*() const { return node->data; }
    // UDT for T = x*
    // const T* operator->() const { return &node->data; }

    QValueListConstIterator<T>& operator++() {
	node = node->next;
	return *this;
    }

    QValueListConstIterator<T> operator++(int) {
	QValueListConstIterator<T> tmp = *this;
	node = node->next;
	return tmp;
    }

    QValueListConstIterator<T>& operator--() {
	node = node->prev;
	return *this;
    }

    QValueListConstIterator<T> operator--(int) {
	QValueListConstIterator<T> tmp = *this;
	node = node->prev;
	return tmp;
    }
};

template <class T>
class QValueListPrivate : public QShared
{
public:
    /**
     * Typedefs
     */
    typedef QValueListIterator<T> Iterator;
    typedef QValueListConstIterator<T> ConstIterator;
    typedef QValueListNode<T> Node;
    typedef QValueListNode<T>* NodePtr;
    typedef size_t size_type;

    /**
     * Functions
     */
    QValueListPrivate();
    QValueListPrivate( const QValueListPrivate<T>& _p );

    void derefAndDelete() // ### hack to get around hp-cc brain damage
    {
	if ( deref() )
	    delete this;
    }

#if defined(Q_TEMPLATEDLL)
    // Workaround MS bug in memory de/allocation in DLL vs. EXE
    virtual
#endif
    ~QValueListPrivate();

    Iterator insert( Iterator it, const T& x );
    Iterator remove( Iterator it );
    NodePtr find( NodePtr start, const T& x ) const;
    int findIndex( NodePtr start, const T& x ) const;
    uint contains( const T& x ) const;
    uint remove( const T& x );
    NodePtr at( size_type i ) const;
    void clear();

    NodePtr node;
    size_type nodes;
};

template <class T>
Q_INLINE_TEMPLATES QValueListPrivate<T>::QValueListPrivate()
{
    node = new Node; node->next = node->prev = node; nodes = 0;
}

template <class T>
Q_INLINE_TEMPLATES QValueListPrivate<T>::QValueListPrivate( const QValueListPrivate<T>& _p )
    : QShared()
{
    node = new Node; node->next = node->prev = node; nodes = 0;
    Iterator b( _p.node->next );
    Iterator e( _p.node );
    Iterator i( node );
    while( b != e )
	insert( i, *b++ );
}

template <class T>
Q_INLINE_TEMPLATES QValueListPrivate<T>::~QValueListPrivate() {
    NodePtr p = node->next;
    while( p != node ) {
	NodePtr x = p->next;
	delete p;
	p = x;
    }
    delete node;
}

template <class T>
Q_INLINE_TEMPLATES Q_TYPENAME QValueListPrivate<T>::Iterator QValueListPrivate<T>::insert( Q_TYPENAME QValueListPrivate<T>::Iterator it, const T& x )
{
    NodePtr p = new Node( x );
    p->next = it.node;
    p->prev = it.node->prev;
    it.node->prev->next = p;
    it.node->prev = p;
    nodes++;
    return p;
}

template <class T>
Q_INLINE_TEMPLATES Q_TYPENAME QValueListPrivate<T>::Iterator QValueListPrivate<T>::remove( Q_TYPENAME QValueListPrivate<T>::Iterator it )
{
    Q_ASSERT ( it.node != node );
    NodePtr next = it.node->next;
    NodePtr prev = it.node->prev;
    prev->next = next;
    next->prev = prev;
    delete it.node;
    nodes--;
    return Iterator( next );
}

template <class T>
Q_INLINE_TEMPLATES Q_TYPENAME QValueListPrivate<T>::NodePtr QValueListPrivate<T>::find( Q_TYPENAME QValueListPrivate<T>::NodePtr start, const T& x ) const
{
    ConstIterator first( start );
    ConstIterator last( node );
    while( first != last) {
	if ( *first == x )
	    return first.node;
	++first;
    }
    return last.node;
}

template <class T>
Q_INLINE_TEMPLATES int QValueListPrivate<T>::findIndex( Q_TYPENAME QValueListPrivate<T>::NodePtr start, const T& x ) const
{
    ConstIterator first( start );
    ConstIterator last( node );
    int pos = 0;
    while( first != last) {
	if ( *first == x )
	    return pos;
	++first;
	++pos;
    }
    return -1;
}

template <class T>
Q_INLINE_TEMPLATES uint QValueListPrivate<T>::contains( const T& x ) const
{
    uint result = 0;
    Iterator first = Iterator( node->next );
    Iterator last = Iterator( node );
    while( first != last) {
	if ( *first == x )
	    ++result;
	++first;
    }
    return result;
}

template <class T>
Q_INLINE_TEMPLATES uint QValueListPrivate<T>::remove( const T& x )
{
    uint result = 0;
    Iterator first = Iterator( node->next );
    Iterator last = Iterator( node );
    while( first != last) {
	if ( *first == x ) {
	    first = remove( first );
	    ++result;
	} else
	    ++first;
    }
    return result;
}

template <class T>
Q_INLINE_TEMPLATES Q_TYPENAME QValueListPrivate<T>::NodePtr QValueListPrivate<T>::at( size_type i ) const
{
    Q_ASSERT( i <= nodes );
    NodePtr p = node->next;
    for( size_type x = 0; x < i; ++x )
	p = p->next;
    return p;
}

template <class T>
Q_INLINE_TEMPLATES void QValueListPrivate<T>::clear()
{
    nodes = 0;
    NodePtr p = node->next;
    while( p != node ) {
	NodePtr next = p->next;
	delete p;
	p = next;
    }
    node->next = node->prev = node;
}

#ifdef QT_CHECK_RANGE
# if !defined( QT_NO_DEBUG ) && defined( QT_CHECK_VALUELIST_RANGE )
#  define QT_CHECK_INVALID_LIST_ELEMENT if ( empty() ) qWarning( "QValueList: Warning invalid element" )
#  define QT_CHECK_INVALID_LIST_ELEMENT_FATAL Q_ASSERT( !empty() );
# else
#  define QT_CHECK_INVALID_LIST_ELEMENT
#  define QT_CHECK_INVALID_LIST_ELEMENT_FATAL
# endif
#else
# define QT_CHECK_INVALID_LIST_ELEMENT
# define QT_CHECK_INVALID_LIST_ELEMENT_FATAL
#endif

template <class T> class QDeepCopy;

template <class T>
class QValueList
{
public:
    /**
     * Typedefs
     */
    typedef QValueListIterator<T> iterator;
    typedef QValueListConstIterator<T> const_iterator;
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef size_t size_type;
#ifndef QT_NO_STL
    typedef ptrdiff_t  difference_type;
#else
    typedef int difference_type;
#endif

    /**
     * API
     */
    QValueList() { sh = new QValueListPrivate<T>; }
    QValueList( const QValueList<T>& l ) { sh = l.sh; sh->ref(); }
#ifndef QT_NO_STL
#  ifdef Q_CC_HPACC    // HP-UX aCC does require typename in some place
#    undef Q_TYPENAME  // but not accept them at others.
#    define Q_TYPENAME // also doesn't like re-defines ...
#  endif
    QValueList( const Q_TYPENAME std::list<T>& l )
    {
	sh = new QValueListPrivate<T>;
	qCopy( l.begin(), l.end(), std::back_inserter( *this ) );
    }
#endif
    ~QValueList() { sh->derefAndDelete(); }

    QValueList<T>& operator= ( const QValueList<T>& l )
    {
	l.sh->ref();
	sh->derefAndDelete();
	sh = l.sh;
	return *this;
    }
#ifndef QT_NO_STL
    QValueList<T>& operator= ( const Q_TYPENAME std::list<T>& l )
    {
	detach();
	qCopy( l.begin(), l.end(), std::back_inserter( *this ) );
	return *this;
    }
    bool operator== ( const Q_TYPENAME std::list<T>& l ) const
    {
	if ( size() != l.size() )
	    return FALSE;
	const_iterator it2 = begin();
#if !defined(Q_CC_MIPS)
	typename
#endif
	std::list<T>::const_iterator it = l.begin();
	for ( ; it2 != end(); ++it2, ++it )
	if ( !((*it2) == (*it)) )
	    return FALSE;
	return TRUE;
    }
#  ifdef Q_CC_HPACC    // undo the HP-UX aCC hackery done above
#    undef Q_TYPENAME 
#    define Q_TYPENAME typename
#  endif
#endif
    bool operator== ( const QValueList<T>& l ) const;
    bool operator!= ( const QValueList<T>& l ) const { return !( *this == l ); }
    iterator begin() { detach(); return iterator( sh->node->next ); }
    const_iterator begin() const { return const_iterator( sh->node->next ); }
    iterator end() { detach(); return iterator( sh->node ); }
    const_iterator end() const { return const_iterator( sh->node ); }
    iterator insert( iterator it, const T& x ) { detach(); return sh->insert( it, x ); }
    uint remove( const T& x ) { detach(); return sh->remove( x ); }
    void clear();

    QValueList<T>& operator<< ( const T& x )
    {
	append( x );
	return *this;
    }

    size_type size() const { return sh->nodes; }
    bool empty() const { return sh->nodes == 0; }
    void push_front( const T& x ) { detach(); sh->insert( begin(), x ); }
    void push_back( const T& x ) { detach(); sh->insert( end(), x ); }
    iterator erase( iterator pos ) { detach(); return sh->remove( pos ); }
    iterator erase( iterator first, iterator last );
    reference front() { QT_CHECK_INVALID_LIST_ELEMENT_FATAL; return *begin(); }
    const_reference front() const { QT_CHECK_INVALID_LIST_ELEMENT_FATAL; return *begin(); }
    reference back() { QT_CHECK_INVALID_LIST_ELEMENT_FATAL; return *(--end()); }
    const_reference back() const { QT_CHECK_INVALID_LIST_ELEMENT_FATAL; return *(--end()); }
    void pop_front() { QT_CHECK_INVALID_LIST_ELEMENT; erase( begin() ); }
    void pop_back() {
	QT_CHECK_INVALID_LIST_ELEMENT;
	iterator tmp = end();
	erase( --tmp );
    }
    void insert( iterator pos, size_type n, const T& x );
    // Some compilers (incl. vc++) would instantiate this function even if
    // it is not used; this would constrain QValueList to classes that provide
    // an operator<
    /*
    void sort()
    {
	qHeapSort( *this );
    }
    */

    QValueList<T> operator+ ( const QValueList<T>& l ) const;
    QValueList<T>& operator+= ( const QValueList<T>& l );

    iterator fromLast() { detach(); return iterator( sh->node->prev ); }
    const_iterator fromLast() const { return const_iterator( sh->node->prev ); }

    bool isEmpty() const { return ( sh->nodes == 0 ); }

    iterator append( const T& x ) { detach(); return sh->insert( end(), x ); }
    iterator prepend( const T& x ) { detach(); return sh->insert( begin(), x ); }

    iterator remove( iterator it ) { detach(); return sh->remove( it ); }

    T& first() { QT_CHECK_INVALID_LIST_ELEMENT; detach(); return sh->node->next->data; }
    const T& first() const { QT_CHECK_INVALID_LIST_ELEMENT; return sh->node->next->data; }
    T& last() { QT_CHECK_INVALID_LIST_ELEMENT; detach(); return sh->node->prev->data; }
    const T& last() const { QT_CHECK_INVALID_LIST_ELEMENT; return sh->node->prev->data; }

    T& operator[] ( size_type i ) { QT_CHECK_INVALID_LIST_ELEMENT; detach(); return sh->at(i)->data; }
    const T& operator[] ( size_type i ) const { QT_CHECK_INVALID_LIST_ELEMENT; return sh->at(i)->data; }
    iterator at( size_type i ) { QT_CHECK_INVALID_LIST_ELEMENT; detach(); return iterator( sh->at(i) ); }
    const_iterator at( size_type i ) const { QT_CHECK_INVALID_LIST_ELEMENT; return const_iterator( sh->at(i) ); }
    iterator find ( const T& x ) { detach(); return iterator( sh->find( sh->node->next, x) ); }
    const_iterator find ( const T& x ) const { return const_iterator( sh->find( sh->node->next, x) ); }
    iterator find ( iterator it, const T& x ) { detach(); return iterator( sh->find( it.node, x ) ); }
    const_iterator find ( const_iterator it, const T& x ) const { return const_iterator( sh->find( it.node, x ) ); }
    int findIndex( const T& x ) const { return sh->findIndex( sh->node->next, x) ; }
    size_type contains( const T& x ) const { return sh->contains( x ); }

    size_type count() const { return sh->nodes; }

    QValueList<T>& operator+= ( const T& x )
    {
	append( x );
	return *this;
    }
    typedef QValueListIterator<T> Iterator;
    typedef QValueListConstIterator<T> ConstIterator;
    typedef T ValueType;

protected:
    /**
     * Helpers
     */
    void detach() { if ( sh->count > 1 ) detachInternal(); }

    /**
     * Variables
     */
    QValueListPrivate<T>* sh;

private:
    void detachInternal();

    friend class QDeepCopy< QValueList<T> >;
};

template <class T>
Q_INLINE_TEMPLATES bool QValueList<T>::operator== ( const QValueList<T>& l ) const
{
    if ( size() != l.size() )
	return FALSE;
    const_iterator it2 = begin();
    const_iterator it = l.begin();
    for( ; it != l.end(); ++it, ++it2 )
	if ( !( *it == *it2 ) )
	    return FALSE;
    return TRUE;
}

template <class T>
Q_INLINE_TEMPLATES void QValueList<T>::clear()
{
    if ( sh->count == 1 ) sh->clear(); else { sh->deref(); sh = new QValueListPrivate<T>; }
}

template <class T>
Q_INLINE_TEMPLATES Q_TYPENAME QValueList<T>::iterator QValueList<T>::erase( Q_TYPENAME QValueList<T>::iterator first, Q_TYPENAME QValueList<T>::iterator last )
{
    while ( first != last )
	erase( first++ );
    return last;
}


template <class T>
Q_INLINE_TEMPLATES void QValueList<T>::insert( Q_TYPENAME QValueList<T>::iterator pos, size_type n, const T& x )
{
    for ( ; n > 0; --n )
	insert( pos, x );
}

template <class T>
Q_INLINE_TEMPLATES QValueList<T> QValueList<T>::operator+ ( const QValueList<T>& l ) const
{
    QValueList<T> l2( *this );
    for( const_iterator it = l.begin(); it != l.end(); ++it )
	l2.append( *it );
    return l2;
}

template <class T>
Q_INLINE_TEMPLATES QValueList<T>& QValueList<T>::operator+= ( const QValueList<T>& l )
{
    for( const_iterator it = l.begin(); it != l.end(); ++it )
	append( *it );
    return *this;
}

template <class T>
Q_INLINE_TEMPLATES void QValueList<T>::detachInternal()
{
    sh->deref(); sh = new QValueListPrivate<T>( *sh );
}

#ifndef QT_NO_DATASTREAM
template <class T>
Q_INLINE_TEMPLATES QDataStream& operator>>( QDataStream& s, QValueList<T>& l )
{
    l.clear();
    Q_UINT32 c;
    s >> c;
    for( Q_UINT32 i = 0; i < c; ++i )
    {
	T t;
	s >> t;
	l.append( t );
	if ( s.atEnd() )
	    break;
    }
    return s;
}

template <class T>
Q_INLINE_TEMPLATES QDataStream& operator<<( QDataStream& s, const QValueList<T>& l )
{
    s << (Q_UINT32)l.size();
    QValueListConstIterator<T> it = l.begin();
    for( ; it != l.end(); ++it )
	s << *it;
    return s;
}
#endif // QT_NO_DATASTREAM
#endif // QVALUELIST_H
