/****************************************************************************
** $Id: qmap.h,v 1.1 2002-11-01 00:10:43 kergoth Exp $
**
** Definition of QMap class
**
** Created : 990406
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
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

#ifndef QMAP_H
#define QMAP_H

#ifndef QT_H
#include "qglobal.h"
#include "qshared.h"
#include "qdatastream.h"
#include "qpair.h"
#include "qvaluelist.h"
#endif // QT_H

#ifndef QT_NO_STL
#include <iterator>
#include <map>
#endif

//#define QT_CHECK_MAP_RANGE

struct Q_EXPORT QMapNodeBase
{
    enum Color { Red, Black };

    QMapNodeBase* left;
    QMapNodeBase* right;
    QMapNodeBase* parent;

    Color color;

    QMapNodeBase* minimum() {
	QMapNodeBase* x = this;
	while ( x->left )
	    x = x->left;
	return x;
    }

    QMapNodeBase* maximum() {
	QMapNodeBase* x = this;
	while ( x->right )
	    x = x->right;
	return x;
    }
};


template <class K, class T>
struct QMapNode : public QMapNodeBase
{
    QMapNode( const K& _key, const T& _data ) { data = _data; key = _key; }
    QMapNode( const K& _key )	   { key = _key; }
    QMapNode( const QMapNode<K,T>& _n ) { key = _n.key; data = _n.data; }
    QMapNode() { }
    T data;
    K key;
};


template<class K, class T>
class QMapIterator
{
 public:
    /**
     * Typedefs
     */
    typedef QMapNode< K, T >* NodePtr;
#ifndef QT_NO_STL
    typedef std::bidirectional_iterator_tag  iterator_category;
#endif
    typedef T          value_type;
#ifndef QT_NO_STL
    typedef ptrdiff_t  difference_type;
#else
    typedef int difference_type;
#endif
    typedef T*         pointer;
    typedef T&         reference;

    /**
     * Variables
     */
    QMapNode<K,T>* node;

    /**
     * Functions
     */
    QMapIterator() : node( 0 ) {}
    QMapIterator( QMapNode<K,T>* p ) : node( p ) {}
    QMapIterator( const QMapIterator<K,T>& it ) : node( it.node ) {}

    bool operator==( const QMapIterator<K,T>& it ) const { return node == it.node; }
    bool operator!=( const QMapIterator<K,T>& it ) const { return node != it.node; }
    T& operator*() { return node->data; }
    const T& operator*() const { return node->data; }
    // UDT for T = x*
    // T* operator->() const { return &node->data; }

    const K& key() const { return node->key; }
    T& data() { return node->data; }
    const T& data() const { return node->data; }

private:
    int inc();
    int dec();

public:
    QMapIterator<K,T>& operator++() {
	inc();
	return *this;
    }

    QMapIterator<K,T> operator++(int) {
	QMapIterator<K,T> tmp = *this;
	inc();
	return tmp;
    }

    QMapIterator<K,T>& operator--() {
	dec();
	return *this;
    }

    QMapIterator<K,T> operator--(int) {
	QMapIterator<K,T> tmp = *this;
	dec();
	return tmp;
    }
};

template <class K, class T>
Q_INLINE_TEMPLATES int QMapIterator<K,T>::inc()
{
    QMapNodeBase* tmp = node;
    if ( tmp->right ) {
	tmp = tmp->right;
	while ( tmp->left )
	    tmp = tmp->left;
    } else {
	QMapNodeBase* y = tmp->parent;
	while (tmp == y->right) {
	    tmp = y;
	    y = y->parent;
	}
	if (tmp->right != y)
	    tmp = y;
    }
    node = (NodePtr)tmp;
    return 0;
}

template <class K, class T>
Q_INLINE_TEMPLATES int QMapIterator<K,T>::dec()
{
    QMapNodeBase* tmp = node;
    if (tmp->color == QMapNodeBase::Red &&
	tmp->parent->parent == tmp ) {
	tmp = tmp->right;
    } else if (tmp->left != 0) {
	QMapNodeBase* y = tmp->left;
	while ( y->right )
	    y = y->right;
	tmp = y;
    } else {
	QMapNodeBase* y = tmp->parent;
	while (tmp == y->left) {
	    tmp = y;
	    y = y->parent;
	}
	tmp = y;
    }
    node = (NodePtr)tmp;
    return 0;
}

template<class K, class T>
class QMapConstIterator
{
 public:
    /**
     * Typedefs
     */
    typedef QMapNode< K, T >* NodePtr;
#ifndef QT_NO_STL
    typedef std::bidirectional_iterator_tag  iterator_category;
#endif
    typedef T          value_type;
#ifndef QT_NO_STL
    typedef ptrdiff_t  difference_type;
#else
    typedef int difference_type;
#endif
    typedef const T*   pointer;
    typedef const T&   reference;


    /**
     * Variables
     */
    QMapNode<K,T>* node;

    /**
     * Functions
     */
    QMapConstIterator() : node( 0 ) {}
    QMapConstIterator( QMapNode<K,T>* p ) : node( p ) {}
    QMapConstIterator( const QMapConstIterator<K,T>& it ) : node( it.node ) {}
    QMapConstIterator( const QMapIterator<K,T>& it ) : node( it.node ) {}

    bool operator==( const QMapConstIterator<K,T>& it ) const { return node == it.node; }
    bool operator!=( const QMapConstIterator<K,T>& it ) const { return node != it.node; }
    const T& operator*()  const { return node->data; }
    // UDT for T = x*
    // const T* operator->() const { return &node->data; }

    const K& key() const { return node->key; }
    const T& data() const { return node->data; }

private:
    int inc();
    int dec();

public:
    QMapConstIterator<K,T>& operator++() {
	inc();
	return *this;
    }

    QMapConstIterator<K,T> operator++(int) {
	QMapConstIterator<K,T> tmp = *this;
	inc();
	return tmp;
    }

    QMapConstIterator<K,T>& operator--() {
	dec();
	return *this;
    }

    QMapConstIterator<K,T> operator--(int) {
	QMapConstIterator<K,T> tmp = *this;
	dec();
	return tmp;
    }
};

template <class K, class T>
Q_INLINE_TEMPLATES int QMapConstIterator<K,T>::inc()
{
    QMapNodeBase* tmp = node;
    if ( tmp->right ) {
	tmp = tmp->right;
	while ( tmp->left )
	    tmp = tmp->left;
    } else {
	QMapNodeBase* y = tmp->parent;
	while (tmp == y->right) {
	    tmp = y;
	    y = y->parent;
	}
	if (tmp->right != y)
	    tmp = y;
    }
    node = (NodePtr)tmp;
    return 0;
}

template <class K, class T>
Q_INLINE_TEMPLATES int QMapConstIterator<K,T>::dec()
{
    QMapNodeBase* tmp = node;
    if (tmp->color == QMapNodeBase::Red &&
	tmp->parent->parent == tmp ) {
	tmp = tmp->right;
    } else if (tmp->left != 0) {
	QMapNodeBase* y = tmp->left;
	while ( y->right )
	    y = y->right;
	tmp = y;
    } else {
	QMapNodeBase* y = tmp->parent;
	while (tmp == y->left) {
	    tmp = y;
	    y = y->parent;
	}
	tmp = y;
    }
    node = (NodePtr)tmp;
    return 0;
}

class Q_EXPORT QMapPrivateBase : public QShared
{
public:
    QMapPrivateBase() {
	node_count = 0;
    }
    QMapPrivateBase( const QMapPrivateBase* _map) {
	node_count = _map->node_count;
    }

    /**
     * Implementations of basic tree algorithms
     */
    void rotateLeft( QMapNodeBase* x, QMapNodeBase*& root);
    void rotateRight( QMapNodeBase* x, QMapNodeBase*& root );
    void rebalance( QMapNodeBase* x, QMapNodeBase*& root );
    QMapNodeBase* removeAndRebalance( QMapNodeBase* z, QMapNodeBase*& root,
				      QMapNodeBase*& leftmost,
				      QMapNodeBase*& rightmost );

    /**
     * Variables
     */
    int node_count;
};


template <class Key, class T>
class QMapPrivate : public QMapPrivateBase
{
public:
    /**
     * Typedefs
     */
    typedef QMapIterator< Key, T > Iterator;
    typedef QMapConstIterator< Key, T > ConstIterator;
    typedef QMapNode< Key, T > Node;
    typedef QMapNode< Key, T >* NodePtr;

    /**
     * Functions
     */
    QMapPrivate();
    QMapPrivate( const QMapPrivate< Key, T >* _map );
    ~QMapPrivate() { clear(); delete header; }

    NodePtr copy( NodePtr p );
    void clear();
    void clear( NodePtr p );

    Iterator begin()	{ return Iterator( (NodePtr)(header->left ) ); }
    Iterator end()	{ return Iterator( header ); }
    ConstIterator begin() const { return ConstIterator( (NodePtr)(header->left ) ); }
    ConstIterator end() const { return ConstIterator( header ); }

    ConstIterator find(const Key& k) const;

    void remove( Iterator it ) {
	NodePtr del = (NodePtr) removeAndRebalance( it.node, header->parent, header->left, header->right );
	delete del;
	--node_count;
    }

#ifdef QT_QMAP_DEBUG
    void inorder( QMapNodeBase* x = 0, int level = 0 ){
	if ( !x )
	    x = header->parent;
	if ( x->left )
	    inorder( x->left, level + 1 );
    //cout << level << " Key=" << key(x) << " Value=" << ((NodePtr)x)->data << endl;
	if ( x->right )
	    inorder( x->right, level + 1 );
    }
#endif

#if 0
    Iterator insertMulti(const Key& v){
	QMapNodeBase* y = header;
	QMapNodeBase* x = header->parent;
	while (x != 0){
	    y = x;
	    x = ( v < key(x) ) ? x->left : x->right;
	}
	return insert(x, y, v);
    }
#endif

    Iterator insertSingle( const Key& k );
    Iterator insert( QMapNodeBase* x, QMapNodeBase* y, const Key& k );

protected:
    /**
     * Helpers
     */
    const Key& key( QMapNodeBase* b ) const { return ((NodePtr)b)->key; }

    /**
     * Variables
     */
    NodePtr header;
};


template <class Key, class T>
Q_INLINE_TEMPLATES QMapPrivate<Key,T>::QMapPrivate() {
    header = new Node;
    header->color = QMapNodeBase::Red; // Mark the header
    header->parent = 0;
    header->left = header->right = header;
}
template <class Key, class T>
Q_INLINE_TEMPLATES QMapPrivate<Key,T>::QMapPrivate( const QMapPrivate< Key, T >* _map ) : QMapPrivateBase( _map ) {
    header = new Node;
    header->color = QMapNodeBase::Red; // Mark the header
    if ( _map->header->parent == 0 ) {
	header->parent = 0;
	header->left = header->right = header;
    } else {
	header->parent = copy( (NodePtr)(_map->header->parent) );
	header->parent->parent = header;
	header->left = header->parent->minimum();
	header->right = header->parent->maximum();
    }
}

template <class Key, class T>
Q_INLINE_TEMPLATES Q_TYPENAME QMapPrivate<Key,T>::NodePtr QMapPrivate<Key,T>::copy( Q_TYPENAME QMapPrivate<Key,T>::NodePtr p )
{
    if ( !p )
	return 0;
    NodePtr n = new Node( *p );
    n->color = p->color;
    if ( p->left ) {
	n->left = copy( (NodePtr)(p->left) );
	n->left->parent = n;
    } else {
	n->left = 0;
    }
    if ( p->right ) {
	n->right = copy( (NodePtr)(p->right) );
	n->right->parent = n;
    } else {
	n->right = 0;
    }
    return n;
}

template <class Key, class T>
Q_INLINE_TEMPLATES void QMapPrivate<Key,T>::clear()
{
    clear( (NodePtr)(header->parent) );
    header->color = QMapNodeBase::Red;
    header->parent = 0;
    header->left = header->right = header;
    node_count = 0;
}

template <class Key, class T>
Q_INLINE_TEMPLATES void QMapPrivate<Key,T>::clear( Q_TYPENAME QMapPrivate<Key,T>::NodePtr p )
{
    while ( p != 0 ) {
	clear( (NodePtr)p->right );
	NodePtr y = (NodePtr)p->left;
	delete p;
	p = y;
    }
}

template <class Key, class T>
Q_INLINE_TEMPLATES Q_TYPENAME QMapPrivate<Key,T>::ConstIterator QMapPrivate<Key,T>::find(const Key& k) const
{
    QMapNodeBase* y = header;        // Last node
    QMapNodeBase* x = header->parent; // Root node.

    while ( x != 0 ) {
	// If as k <= key(x) go left
	if ( !( key(x) < k ) ) {
	    y = x;
	    x = x->left;
	} else {
	    x = x->right;
	}
    }

    // Was k bigger/smaller then the biggest/smallest
    // element of the tree ? Return end()
    if ( y == header || k < key(y) )
	return ConstIterator( header );
    return ConstIterator( (NodePtr)y );
}

template <class Key, class T>
Q_INLINE_TEMPLATES Q_TYPENAME QMapPrivate<Key,T>::Iterator QMapPrivate<Key,T>::insertSingle( const Key& k )
{
    // Search correct position in the tree
    QMapNodeBase* y = header;
    QMapNodeBase* x = header->parent;
    bool result = TRUE;
    while ( x != 0 ) {
	result = ( k < key(x) );
	y = x;
	x = result ? x->left : x->right;
    }
    // Get iterator on the last not empty one
    Iterator j( (NodePtr)y );
    if ( result ) {
	// Smaller then the leftmost one ?
	if ( j == begin() ) {
	    return insert(x, y, k );
	} else {
	    // Perhaps daddy is the right one ?
	    --j;
	}
    }
    // Really bigger ?
    if ( (j.node->key) < k )
	return insert(x, y, k );
    // We are going to replace a node
    return j;
}


template <class Key, class T>
Q_INLINE_TEMPLATES Q_TYPENAME QMapPrivate<Key,T>::Iterator QMapPrivate<Key,T>::insert( QMapNodeBase* x, QMapNodeBase* y, const Key& k )
{
    NodePtr z = new Node( k );
    if (y == header || x != 0 || k < key(y) ) {
	y->left = z;                // also makes leftmost = z when y == header
	if ( y == header ) {
	    header->parent = z;
	    header->right = z;
	} else if ( y == header->left )
	    header->left = z;           // maintain leftmost pointing to min node
    } else {
	y->right = z;
	if ( y == header->right )
	    header->right = z;          // maintain rightmost pointing to max node
    }
    z->parent = y;
    z->left = 0;
    z->right = 0;
    rebalance( z, header->parent );
    ++node_count;
    return Iterator(z);
}


#ifdef QT_CHECK_RANGE
# if !defined( QT_NO_DEBUG ) && defined( QT_CHECK_MAP_RANGE )
#  define QT_CHECK_INVALID_MAP_ELEMENT if ( empty() ) qWarning( "QMap: Warning invalid element" )
#  define QT_CHECK_INVALID_MAP_ELEMENT_FATAL Q_ASSERT( !empty() );
# else
#  define QT_CHECK_INVALID_MAP_ELEMENT
#  define QT_CHECK_INVALID_MAP_ELEMENT_FATAL
# endif
#else
# define QT_CHECK_INVALID_MAP_ELEMENT
# define QT_CHECK_INVALID_MAP_ELEMENT_FATAL
#endif

template <class T> class QDeepCopy;

template<class Key, class T>
class QMap
{
public:
    /**
     * Typedefs
     */
    typedef Key key_type;
    typedef T mapped_type;
    typedef QPair<const key_type, mapped_type> value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
#ifndef QT_NO_STL
    typedef ptrdiff_t  difference_type;
#else
    typedef int difference_type;
#endif
    typedef size_t      size_type;
    typedef QMapIterator<Key,T> iterator;
    typedef QMapConstIterator<Key,T> const_iterator;
    typedef QPair<iterator,bool> insert_pair;

    typedef QMapIterator< Key, T > Iterator;
    typedef QMapConstIterator< Key, T > ConstIterator;
    typedef T ValueType;
    typedef QMapPrivate< Key, T > Priv;

    /**
     * API
     */
    QMap()
    {
	sh = new QMapPrivate< Key, T >;
    }
    QMap( const QMap<Key,T>& m )
    {
	sh = m.sh; sh->ref();
    }

#ifndef QT_NO_STL
#  ifdef Q_CC_HPACC    // HP-UX aCC does require typename in some place
#    undef Q_TYPENAME  // but not accept them at others.
#    define Q_TYPENAME // also doesn't like re-defines ...
#  endif
    QMap( const Q_TYPENAME std::map<Key,T>& m )
    {
	sh = new QMapPrivate<Key,T>;
#if defined(Q_OS_WIN32)
	std::map<Key,T>::const_iterator it = m.begin();
#else
	QMapConstIterator<Key,T> it = m.begin();
#endif
	for ( ; it != m.end(); ++it ) {
	    value_type p( (*it).first, (*it).second );
	    insert( p );
	}
    }
#endif
    ~QMap()
    {
	if ( sh->deref() )
	    delete sh;
    }
    QMap<Key,T>& operator= ( const QMap<Key,T>& m );
#ifndef QT_NO_STL
    QMap<Key,T>& operator= ( const Q_TYPENAME std::map<Key,T>& m )
    {
	clear();
#if defined(Q_OS_WIN32)
	std::map<Key,T>::const_iterator it = m.begin();
#else
	QMapConstIterator<Key,T> it = m.begin();
#endif
	for ( ; it != m.end(); ++it ) {
	    value_type p( (*it).first, (*it).second );
	    insert( p );
	}
	return *this;
    }
#  ifdef Q_CC_HPACC    // undo the HP-UX aCC hackery done above
#    undef Q_TYPENAME 
#    define Q_TYPENAME typename
#  endif
#endif

    iterator begin() { detach(); return sh->begin(); }
    iterator end() { detach(); return sh->end(); }
    const_iterator begin() const { return ((const Priv*)sh)->begin(); }
    const_iterator end() const { return ((const Priv*)sh)->end(); }
    iterator replace( const Key& k, const T& v )
    {
	remove( k );
	return insert( k, v );
    }

    size_type size() const
    {
	return sh->node_count;
    }
    bool empty() const
    {
	return sh->node_count == 0;
    }
    QPair<iterator,bool> insert( const value_type& x );

    void erase( iterator it )
    {
	detach();
	sh->remove( it );
    }
    void erase( const key_type& k );
    size_type count( const key_type& k ) const;
    T& operator[] ( const Key& k );
    void clear();

    iterator find ( const Key& k )
    {
	detach();
	return iterator( sh->find( k ).node );
    }
    const_iterator find ( const Key& k ) const {	return sh->find( k ); }

    const T& operator[] ( const Key& k ) const
	{ QT_CHECK_INVALID_MAP_ELEMENT; return sh->find( k ).data(); }
    bool contains ( const Key& k ) const
	{ return find( k ) != end(); }
	//{ return sh->find( k ) != ((const Priv*)sh)->end(); }

    size_type count() const { return sh->node_count; }

    QValueList<Key> keys() const {
	QValueList<Key> r;
	for (const_iterator i=begin(); i!=end(); ++i)
	    r.append(i.key());
	return r;
    }

    QValueList<T> values() const {
	QValueList<T> r;
	for (const_iterator i=begin(); i!=end(); ++i)
	    r.append(*i);
	return r;
    }

    bool isEmpty() const { return sh->node_count == 0; }

    iterator insert( const Key& key, const T& value, bool overwrite = TRUE );
    void remove( iterator it ) { detach(); sh->remove( it ); }
    void remove( const Key& k );

#if defined(Q_FULL_TEMPLATE_INSTANTIATION)
    bool operator==( const QMap<Key,T>& ) const { return FALSE; }
#ifndef QT_NO_STL
    bool operator==( const Q_TYPENAME std::map<Key,T>& ) const { return FALSE; }
#endif
#endif

protected:
    /**
     * Helpers
     */
    void detach() {  if ( sh->count > 1 ) detachInternal(); }

    Priv* sh;
private:
    void detachInternal();

    friend class QDeepCopy< QMap<Key,T> >;
};

template<class Key, class T>
Q_INLINE_TEMPLATES QMap<Key,T>& QMap<Key,T>::operator= ( const QMap<Key,T>& m )
{
    m.sh->ref();
    if ( sh->deref() )
	delete sh;
    sh = m.sh;
    return *this;
}

template<class Key, class T>
Q_INLINE_TEMPLATES Q_TYPENAME QMap<Key,T>::insert_pair QMap<Key,T>::insert( const Q_TYPENAME QMap<Key,T>::value_type& x )
{
    detach();
    size_type n = size();
    iterator it = sh->insertSingle( x.first );
    bool inserted = FALSE;
    if ( n < size() ) {
	inserted = TRUE;
	it.data() = x.second;
    }
    return QPair<iterator,bool>( it, inserted );
}

template<class Key, class T>
Q_INLINE_TEMPLATES void QMap<Key,T>::erase( const Key& k )
{
    detach();
    iterator it( sh->find( k ).node );
    if ( it != end() )
	sh->remove( it );
}

template<class Key, class T>
Q_INLINE_TEMPLATES Q_TYPENAME QMap<Key,T>::size_type QMap<Key,T>::count( const Key& k ) const
{
    const_iterator it( sh->find( k ).node );
    if ( it != end() ) {
	size_type c = 0;
	while ( it != end() ) {
	    ++it;
	    ++c;
	}
	return c;
    }
    return 0;
}

template<class Key, class T>
Q_INLINE_TEMPLATES T& QMap<Key,T>::operator[] ( const Key& k )
{
    detach();
    QMapNode<Key,T>* p = sh->find( k ).node;
    if ( p != sh->end().node )
	return p->data;
    return insert( k, T() ).data();
}

template<class Key, class T>
Q_INLINE_TEMPLATES void QMap<Key,T>::clear()
{
    if ( sh->count == 1 )
	sh->clear();
    else {
	sh->deref();
	sh = new QMapPrivate<Key,T>;
    }
}

template<class Key, class T>
Q_INLINE_TEMPLATES Q_TYPENAME QMap<Key,T>::iterator QMap<Key,T>::insert( const Key& key, const T& value, bool overwrite )
{
    detach();
    size_type n = size();
    iterator it = sh->insertSingle( key );
    if ( overwrite || n < size() )
	it.data() = value;
    return it;
}

template<class Key, class T>
Q_INLINE_TEMPLATES void QMap<Key,T>::remove( const Key& k )
{
    detach();
    iterator it( sh->find( k ).node );
    if ( it != end() )
	sh->remove( it );
}

template<class Key, class T>
Q_INLINE_TEMPLATES void QMap<Key,T>::detachInternal()
{
    sh->deref(); sh = new QMapPrivate<Key,T>( sh );
}


#ifndef QT_NO_DATASTREAM
template<class Key, class T>
Q_INLINE_TEMPLATES QDataStream& operator>>( QDataStream& s, QMap<Key,T>& m ) {
    m.clear();
    Q_UINT32 c;
    s >> c;
    for( Q_UINT32 i = 0; i < c; ++i ) {
	Key k; T t;
	s >> k >> t;
	m.insert( k, t );
	if ( s.atEnd() )
	    break;
    }
    return s;
}


template<class Key, class T>
Q_INLINE_TEMPLATES QDataStream& operator<<( QDataStream& s, const QMap<Key,T>& m ) {
    s << (Q_UINT32)m.size();
    QMapConstIterator<Key,T> it = m.begin();
    for( ; it != m.end(); ++it )
	s << it.key() << it.data();
    return s;
}
#endif

#endif // QMAP_H
