/****************************************************************************
** $Id: qgcache.cpp,v 1.1 2002-11-01 00:10:44 kergoth Exp $
**
** Implementation of QGCache and QGCacheIterator classes
**
** Created : 950208
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

#include "qgcache.h"
#include "qptrlist.h"
#include "qdict.h"
#include "qstring.h"

/*!
  \class QGCache qgcache.h
  \reentrant
  \ingroup shared
  \ingroup collection
  \brief The QGCache class is an internal class for implementing QCache
  template classes.

  \internal

  QGCache is a strictly internal class that acts as a base class for the
  \link collection.html collection classes\endlink QCache and QIntCache.
*/


/*****************************************************************************
  QGCacheItem class (internal cache item)
 *****************************************************************************/

struct QCacheItem
{
    QCacheItem( void *k, QPtrCollection::Item d, int c, short p )
	: priority(p), skipPriority(p), cost(c), key(k), data(d), node(0) {}
    short	priority;
    short	skipPriority;
    int		cost;
    void       *key;
    QPtrCollection::Item data;
    QLNode     *node;
};


/*****************************************************************************
  QCList class (internal list of cache items)
 *****************************************************************************/

class QCList : private QPtrList<QCacheItem>
{
friend class QGCacheIterator;
friend class QCListIt;
public:
    QCList()	{}
   ~QCList();

    void	insert( QCacheItem * );		// insert according to priority
    void	insert( int, QCacheItem * );
    void	take( QCacheItem * );
    void	reference( QCacheItem * );

    void	setAutoDelete( bool del ) { QPtrCollection::setAutoDelete(del); }

    bool	removeFirst()	{ return QPtrList<QCacheItem>::removeFirst(); }
    bool	removeLast()	{ return QPtrList<QCacheItem>::removeLast(); }

    QCacheItem *first()		{ return QPtrList<QCacheItem>::first(); }
    QCacheItem *last()		{ return QPtrList<QCacheItem>::last(); }
    QCacheItem *prev()		{ return QPtrList<QCacheItem>::prev(); }
    QCacheItem *next()		{ return QPtrList<QCacheItem>::next(); }

#if defined(QT_DEBUG)
    int		inserts;			// variables for statistics
    int		insertCosts;
    int		insertMisses;
    int		finds;
    int		hits;
    int		hitCosts;
    int		dumps;
    int		dumpCosts;
#endif
};


QCList::~QCList()
{
#if defined(QT_DEBUG)
    Q_ASSERT( count() == 0 );
#endif
}


void QCList::insert( QCacheItem *ci )
{
    QCacheItem *item = first();
    while( item && item->skipPriority > ci->priority ) {
	item->skipPriority--;
	item = next();
    }
    if ( item )
	QPtrList<QCacheItem>::insert( at(), ci );
    else
	append( ci );
#if defined(QT_DEBUG)
    Q_ASSERT( ci->node == 0 );
#endif
    ci->node = currentNode();
}

inline void QCList::insert( int i, QCacheItem *ci )
{
    QPtrList<QCacheItem>::insert( i, ci );
#if defined(QT_DEBUG)
    Q_ASSERT( ci->node == 0 );
#endif
    ci->node = currentNode();
}


void QCList::take( QCacheItem *ci )
{
    if ( ci ) {
#if defined(QT_DEBUG)
	Q_ASSERT( ci->node != 0 );
#endif
	takeNode( ci->node );
	ci->node = 0;
    }
}


inline void QCList::reference( QCacheItem *ci )
{
#if defined(QT_DEBUG)
    Q_ASSERT( ci != 0 && ci->node != 0 );
#endif
    ci->skipPriority = ci->priority;
    relinkNode( ci->node );			// relink as first item
}


class QCListIt: public QPtrListIterator<QCacheItem>
{
public:
    QCListIt( const QCList *p ): QPtrListIterator<QCacheItem>( *p ) {}
    QCListIt( const QCListIt *p ): QPtrListIterator<QCacheItem>( *p ) {}
};


/*****************************************************************************
  QCDict class (internal dictionary of cache items)
 *****************************************************************************/

//
// Since we need to decide if the dictionary should use an int or const
// char * key (the "bool trivial" argument in the constructor below)
// we cannot use the macro/template dict, but inherit directly from QGDict.
//

class QCDict : public QGDict
{
public:
    QCDict( uint size, uint kt, bool caseSensitive, bool copyKeys )
	: QGDict( size, (KeyType)kt, caseSensitive, copyKeys ) {}
    ~QCDict();

    void clear() { QGDict::clear(); }

    QCacheItem *find_string(const QString &key) const
	{ return (QCacheItem*)((QCDict*)this)->look_string(key, 0, 0); }
    QCacheItem *find_ascii(const char *key) const
	{ return (QCacheItem*)((QCDict*)this)->look_ascii(key, 0, 0); }
    QCacheItem *find_int(long key) const
	{ return (QCacheItem*)((QCDict*)this)->look_int(key, 0, 0); }

    QCacheItem *take_string(const QString &key)
	{ return (QCacheItem*)QGDict::take_string(key); }
    QCacheItem *take_ascii(const char *key)
	{ return (QCacheItem*)QGDict::take_ascii(key); }
    QCacheItem *take_int(long key)
	{ return (QCacheItem*)QGDict::take_int(key); }

    bool  insert_string( const QString &key, const QCacheItem *ci )
	{ return QGDict::look_string(key,(Item)ci,1)!=0;}
    bool  insert_ascii( const char *key, const QCacheItem *ci )
	{ return QGDict::look_ascii(key,(Item)ci,1)!=0;}
    bool  insert_int( long key, const QCacheItem *ci )
	{ return QGDict::look_int(key,(Item)ci,1)!=0;}

    bool  remove_string( QCacheItem *item )
	{ return QGDict::remove_string(*((QString*)(item->key)),item); }
    bool  remove_ascii( QCacheItem *item )
	{ return QGDict::remove_ascii((const char *)item->key,item); }
    bool  remove_int( QCacheItem *item )
	{ return QGDict::remove_int((long)item->key,item);}

    void  statistics()			{ QGDict::statistics(); }

private:
    void deleteItem( void *item )
	{ if ( del_item ) { QCacheItem *d = (QCacheItem*)item; delete d; } }
};

inline QCDict::~QCDict()
{
    clear();
}

/*****************************************************************************
  QGDict member functions
 *****************************************************************************/

/*!
  Constructs a cache.
  The maximum cost of the cache is given by \a maxCost and the size by \a
  size. The key type is \a kt which may be \c StringKey, \c AsciiKey,
  \c IntKey or \c PtrKey. The case-sensitivity of lookups is set with
  \a caseSensitive. Keys are copied if \a copyKeys is TRUE.
*/

QGCache::QGCache( int maxCost, uint size, KeyType kt, bool caseSensitive,
		  bool copyKeys )
{
    keytype = kt;
    lruList = new QCList;
    Q_CHECK_PTR( lruList );
    lruList->setAutoDelete( TRUE );
    copyk   = ((keytype == AsciiKey) && copyKeys);
    dict    = new QCDict( size, kt, caseSensitive, FALSE );
    Q_CHECK_PTR( dict );
    mCost   = maxCost;
    tCost   = 0;
#if defined(QT_DEBUG)
    lruList->inserts	  = 0;
    lruList->insertCosts  = 0;
    lruList->insertMisses = 0;
    lruList->finds	  = 0;
    lruList->hits	  = 0;
    lruList->hitCosts	  = 0;
    lruList->dumps	  = 0;
    lruList->dumpCosts	  = 0;
#endif
}

/*!
  Cannot copy a cache.
*/

QGCache::QGCache( const QGCache & )
    : QPtrCollection()
{
#if defined(QT_CHECK_NULL)
    qFatal( "QGCache::QGCache(QGCache &): Cannot copy a cache" );
#endif
}

/*!
  Removes all items from the cache and destroys it.
*/

QGCache::~QGCache()
{
    clear();
    delete dict;
    delete lruList;
}

/*!
  Cannot assign a cache.
*/

QGCache &QGCache::operator=( const QGCache & )
{
#if defined(QT_CHECK_NULL)
    qFatal( "QGCache::operator=: Cannot copy a cache" );
#endif
    return *this;
}


/*!
  Returns the number of items in the cache.
*/

uint QGCache::count() const
{
    return dict->count();
}

/*!
  Returns the size of the hash array.
*/

uint QGCache::size() const
{
    return dict->size();
}

/*!
  \fn int QGCache::maxCost() const

  Returns the maximum cache cost.
*/

/*!
  \fn int QGCache::totalCost() const

  Returns the total cache cost.
*/

/*!
  Sets the maximum cache cost to \a maxCost.
*/

void QGCache::setMaxCost( int maxCost )
{
    if ( maxCost < tCost ) {
	if ( !makeRoomFor(tCost - maxCost) )	// remove excess cost
	    return;
    }
    mCost = maxCost;
}


/*!
    Inserts an item with data \a data into the cache using key \a key.
    The item has cost \a cost and priority \a priority.

  \warning If this function returns FALSE, you must delete \a data
  yourself.  Additionally, be very careful about using \a data after
  calling this function, as any other insertions into the cache, from
  anywhere in the application, or within Qt itself, could cause the
  data to be discarded from the cache, and the pointer to become
  invalid.
*/

bool QGCache::insert_string( const QString &key, QPtrCollection::Item data,
			     int cost, int priority)
{
    if ( tCost + cost > mCost ) {
	if ( !makeRoomFor(tCost + cost - mCost, priority) ) {
#if defined(QT_DEBUG)
	    lruList->insertMisses++;
#endif
	    return FALSE;
	}
    }
#if defined(QT_DEBUG)
    Q_ASSERT( keytype == StringKey );
    lruList->inserts++;
    lruList->insertCosts += cost;
#endif
    if ( priority < -32768 )
	priority = -32768;
    else if ( priority > 32767 )
	priority = 32677;
    QCacheItem *ci = new QCacheItem( new QString(key), newItem(data),
				     cost, (short)priority );
    Q_CHECK_PTR( ci );
    lruList->insert( 0, ci );
    dict->insert_string( key, ci );
    tCost += cost;
    return TRUE;
}

bool QGCache::insert_other( const char *key, QPtrCollection::Item data,
			    int cost, int priority)
{
    if ( tCost + cost > mCost ) {
	if ( !makeRoomFor(tCost + cost - mCost, priority) ) {
#if defined(QT_DEBUG)
	    lruList->insertMisses++;
#endif
	    return FALSE;
	}
    }
#if defined(QT_DEBUG)
    Q_ASSERT( keytype != StringKey );
    lruList->inserts++;
    lruList->insertCosts += cost;
#endif
    if ( keytype == AsciiKey && copyk )
	key = qstrdup( key );
    if ( priority < -32768 )
	priority = -32768;
    else if ( priority > 32767 )
	priority = 32677;
    QCacheItem *ci = new QCacheItem( (void*)key, newItem(data), cost,
				     (short)priority );
    Q_CHECK_PTR( ci );
    lruList->insert( 0, ci );
    if ( keytype == AsciiKey )
	dict->insert_ascii( key, ci );
    else
	dict->insert_int( (long)key, ci );
    tCost += cost;
    return TRUE;
}


/*!
  Removes the item with key \a key from the cache. Returns TRUE if the
  item was removed; otherwise returns FALSE.
*/

bool QGCache::remove_string( const QString &key )
{
    Item d = take_string( key );
    if ( d )
	deleteItem( d );
    return d != 0;
}

bool QGCache::remove_other( const char *key )
{
    Item d = take_other( key );
    if ( d )
	deleteItem( d );
    return d != 0;
}


/*!
  Takes the item with key \a key out of the cache. The item is not
  deleted. If no item has this \a key 0 is returned.
*/

QPtrCollection::Item QGCache::take_string( const QString &key )
{
    QCacheItem *ci = dict->take_string( key );	// take from dict
    Item d;
    if ( ci ) {
	d = ci->data;
	tCost -= ci->cost;
	lruList->take( ci );			// take from list
	delete (QString*)ci->key;
	delete ci;
    } else {
	d = 0;
    }
    return d;
}

/*!
  Takes the item with key \a key out of the cache. The item is not
  deleted. If no item has this \a key 0 is returned.
*/

QPtrCollection::Item QGCache::take_other( const char *key )
{
    QCacheItem *ci;
    if ( keytype == AsciiKey )
	ci = dict->take_ascii( key );
    else
	ci = dict->take_int( (long)key );
    Item d;
    if ( ci ) {
	d = ci->data;
	tCost -= ci->cost;
	lruList->take( ci );			// take from list
	if ( copyk )
	    delete [] (char *)ci->key;
	delete ci;
    } else {
	d = 0;
    }
    return d;
}


/*!
  Clears the cache.
*/

void QGCache::clear()
{
    QCacheItem *ci;
    while ( (ci = lruList->first()) ) {
	switch ( keytype ) {
	    case StringKey:
		dict->remove_string( ci );
		delete (QString*)ci->key;
		break;
	    case AsciiKey:
		dict->remove_ascii( ci );
		if ( copyk )
		    delete [] (char*)ci->key;
		break;
	    case IntKey:
		dict->remove_int( ci );
		break;
	    case PtrKey:			// unused
		break;
	}
	deleteItem( ci->data );			// delete data
	lruList->removeFirst();			// remove from list
    }
    tCost = 0;
}


/*!
  Finds an item for \a key in the cache and adds a reference if \a ref is TRUE.
*/

QPtrCollection::Item QGCache::find_string( const QString &key, bool ref ) const
{
    QCacheItem *ci = dict->find_string( key );
#if defined(QT_DEBUG)
    lruList->finds++;
#endif
    if ( ci ) {
#if defined(QT_DEBUG)
	lruList->hits++;
	lruList->hitCosts += ci->cost;
#endif
	if ( ref )
	    lruList->reference( ci );
	return ci->data;
    }
    return 0;
}


/*!
  Finds an item for \a key in the cache and adds a reference if \a ref is TRUE.
*/

QPtrCollection::Item QGCache::find_other( const char *key, bool ref ) const
{
    QCacheItem *ci = keytype == AsciiKey ? dict->find_ascii(key)
					 : dict->find_int((long)key);
#if defined(QT_DEBUG)
    lruList->finds++;
#endif
    if ( ci ) {
#if defined(QT_DEBUG)
	lruList->hits++;
	lruList->hitCosts += ci->cost;
#endif
	if ( ref )
	    lruList->reference( ci );
	return ci->data;
    }
    return 0;
}


/*!
  Allocates cache space for one or more items.
*/

bool QGCache::makeRoomFor( int cost, int priority )
{
    if ( cost > mCost )				// cannot make room for more
	return FALSE;				//   than maximum cost
    if ( priority == -1 )
	priority = 32767;
    register QCacheItem *ci = lruList->last();
    int cntCost = 0;
    int dumps	= 0;				// number of items to dump
    while ( cntCost < cost && ci && ci->skipPriority <= priority ) {
	cntCost += ci->cost;
	ci	 = lruList->prev();
	dumps++;
    }
    if ( cntCost < cost )			// can enough cost be dumped?
	return FALSE;				// no
#if defined(QT_DEBUG)
    Q_ASSERT( dumps > 0 );
#endif
    while ( dumps-- ) {
	ci = lruList->last();
#if defined(QT_DEBUG)
	lruList->dumps++;
	lruList->dumpCosts += ci->cost;
#endif
	switch ( keytype ) {
	    case StringKey:
		dict->remove_string( ci );
		delete (QString*)ci->key;
		break;
	    case AsciiKey:
		dict->remove_ascii( ci );
		if ( copyk )
		    delete [] (char *)ci->key;
		break;
	    case IntKey:
		dict->remove_int( ci );
		break;
	    case PtrKey:			// unused
		break;
	}
	deleteItem( ci->data );			// delete data
	lruList->removeLast();			// remove from list
    }
    tCost -= cntCost;
    return TRUE;
}


/*!
  Outputs debug statistics.
*/

void QGCache::statistics() const
{
#if defined(QT_DEBUG)
    QString line;
    line.fill( '*', 80 );
    qDebug( line.ascii() );
    qDebug( "CACHE STATISTICS:" );
    qDebug( "cache contains %d item%s, with a total cost of %d",
	   count(), count() != 1 ? "s" : "", tCost );
    qDebug( "maximum cost is %d, cache is %d%% full.",
	   mCost, (200*tCost + mCost) / (mCost*2) );
    qDebug( "find() has been called %d time%s",
	   lruList->finds, lruList->finds != 1 ? "s" : "" );
    qDebug( "%d of these were hits, items found had a total cost of %d.",
	   lruList->hits,lruList->hitCosts );
    qDebug( "%d item%s %s been inserted with a total cost of %d.",
	   lruList->inserts,lruList->inserts != 1 ? "s" : "",
	   lruList->inserts != 1 ? "have" : "has", lruList->insertCosts );
    qDebug( "%d item%s %s too large or had too low priority to be inserted.",
	   lruList->insertMisses, lruList->insertMisses != 1 ? "s" : "",
	   lruList->insertMisses != 1 ? "were" : "was" );
    qDebug( "%d item%s %s been thrown away with a total cost of %d.",
	   lruList->dumps, lruList->dumps != 1 ? "s" : "",
	   lruList->dumps != 1 ? "have" : "has", lruList->dumpCosts );
    qDebug( "Statistics from internal dictionary class:" );
    dict->statistics();
    qDebug( line.ascii() );
#endif
}


/*****************************************************************************
  QGCacheIterator member functions
 *****************************************************************************/

/*!
  \class QGCacheIterator qgcache.h
  \reentrant
  \ingroup shared
  \ingroup collection
  \brief The QGCacheIterator class is an internal class for implementing QCacheIterator and
  QIntCacheIterator.

  \internal

  QGCacheIterator is a strictly internal class that does the heavy work for
  QCacheIterator and QIntCacheIterator.
*/

/*!
  Constructs an iterator that operates on the cache \a c.
*/

QGCacheIterator::QGCacheIterator( const QGCache &c )
{
    it = new QCListIt( c.lruList );
#if defined(QT_DEBUG)
    Q_ASSERT( it != 0 );
#endif
}

/*!
  Constructs an iterator that operates on the same cache as \a ci.
*/

QGCacheIterator::QGCacheIterator( const QGCacheIterator &ci )
{
    it = new QCListIt( ci.it );
#if defined(QT_DEBUG)
    Q_ASSERT( it != 0 );
#endif
}

/*!
  Destroys the iterator.
*/

QGCacheIterator::~QGCacheIterator()
{
    delete it;
}

/*!
  Assigns the iterator \a ci to this cache iterator.
*/

QGCacheIterator &QGCacheIterator::operator=( const QGCacheIterator &ci )
{
    *it = *ci.it;
    return *this;
}

/*!
  Returns the number of items in the cache.
*/

uint QGCacheIterator::count() const
{
    return it->count();
}

/*!
  Returns TRUE if the iterator points to the first item.
*/

bool  QGCacheIterator::atFirst() const
{
    return it->atFirst();
}

/*!
  Returns TRUE if the iterator points to the last item.
*/

bool QGCacheIterator::atLast() const
{
    return it->atLast();
}

/*!
  Sets the list iterator to point to the first item in the cache.
*/

QPtrCollection::Item QGCacheIterator::toFirst()
{
    QCacheItem *item = it->toFirst();
    return item ? item->data : 0;
}

/*!
  Sets the list iterator to point to the last item in the cache.
*/

QPtrCollection::Item QGCacheIterator::toLast()
{
    QCacheItem *item = it->toLast();
    return item ? item->data : 0;
}

/*!
  Returns the current item.
*/

QPtrCollection::Item QGCacheIterator::get() const
{
    QCacheItem *item = it->current();
    return item ? item->data : 0;
}

/*!
  Returns the key of the current item.
*/

QString QGCacheIterator::getKeyString() const
{
    QCacheItem *item = it->current();
    return item ? *((QString*)item->key) : QString::null;
}

/*!
  Returns the key of the current item, as a \0-terminated C string.
*/

const char *QGCacheIterator::getKeyAscii() const
{
    QCacheItem *item = it->current();
    return item ? (const char *)item->key : 0;
}

/*!
  Returns the key of the current item, as a long.
*/

long QGCacheIterator::getKeyInt() const
{
    QCacheItem *item = it->current();
    return item ? (long)item->key : 0;
}

/*!
  Moves to the next item (postfix).
*/

QPtrCollection::Item QGCacheIterator::operator()()
{
    QCacheItem *item = it->operator()();
    return item ? item->data : 0;
}

/*!
  Moves to the next item (prefix).
*/

QPtrCollection::Item QGCacheIterator::operator++()
{
    QCacheItem *item = it->operator++();
    return item ? item->data : 0;
}

/*!
  Moves \a jump positions forward.
*/

QPtrCollection::Item QGCacheIterator::operator+=( uint jump )
{
    QCacheItem *item = it->operator+=(jump);
    return item ? item->data : 0;
}

/*!
  Moves to the previous item (prefix).
*/

QPtrCollection::Item QGCacheIterator::operator--()
{
    QCacheItem *item = it->operator--();
    return item ? item->data : 0;
}

/*!
  Moves \a jump positions backward.
*/

QPtrCollection::Item QGCacheIterator::operator-=( uint jump )
{
    QCacheItem *item = it->operator-=(jump);
    return item ? item->data : 0;
}
