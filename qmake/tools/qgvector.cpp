/****************************************************************************
** $Id: qgvector.cpp,v 1.1 2002-11-01 00:10:44 kergoth Exp $
**
** Implementation of QGVector class
**
** Created : 930907
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

#define	 QGVECTOR_CPP
#include "qgvector.h"
#include "qglist.h"
#include "qstring.h"
#include "qdatastream.h"
#include <stdlib.h>

#ifdef QT_THREAD_SUPPORT
#  include <private/qmutexpool_p.h>
#endif // QT_THREAD_SUPPORT

#define USE_MALLOC				// comment to use new/delete

#undef NEW
#undef DELETE

#if defined(USE_MALLOC)
#define NEW(type,size)	((type*)malloc(size*sizeof(type)))
#define DELETE(array)	(free((char*)array))
#else
#define NEW(type,size)	(new type[size])
#define DELETE(array)	(delete[] array)
#define DONT_USE_REALLOC			// comment to use realloc()
#endif

/*!
  \class QGVector
  \reentrant
  \ingroup collection

  \brief The QGVector class is an internal class for implementing Qt
  collection classes.

  \internal

  QGVector is an internal class that acts as a base class for the
  QPtrVector collection class.

  QGVector has some virtual functions that may be reimplemented in
  subclasses to customize behavior.

  \list
  \i compareItems() compares two collection/vector items.
  \i read() reads a collection/vector item from a QDataStream.
  \i write() writes a collection/vector item to a QDataStream.
  \endlist
*/

/*****************************************************************************
  Default implementation of virtual functions
 *****************************************************************************/

/*!
  This virtual function compares two list items.

  Returns:
  <ul>
  <li> 0 if \a d1 == \a d2
  <li> non-zero if \a d1 != \a d2
  </ul>

  This function returns \e int rather than \e bool so that
  reimplementations can return one of three values and use it to sort
  by:
  <ul>
  <li> 0 if \a d1 == \a d2
  <li> \> 0 (positive integer) if \a d1 \> \a d2
  <li> \< 0 (negative integer) if \a d1 \< \a d2
  </ul>

  The QPtrVector::sort() and QPtrVector::bsearch() functions require that
  compareItems() is implemented as described here.

  This function should not modify the vector because some const
  functions call compareItems().
*/

int QGVector::compareItems( Item d1, Item d2 )
{
    return d1 != d2;				// compare pointers
}

#ifndef QT_NO_DATASTREAM
/*!
  Reads a collection/vector item from the stream \a s and returns a reference
  to the stream.

  The default implementation sets \a d to 0.

  \sa write()
*/

QDataStream &QGVector::read( QDataStream &s, Item &d )
{						// read item from stream
    d = 0;
    return s;
}

/*!
  Writes a collection/vector item to the stream \a s and returns a reference
  to the stream.

  The default implementation does nothing.

  \sa read()
*/

QDataStream &QGVector::write( QDataStream &s, Item ) const
{						// write item to stream
    return s;
}
#endif // QT_NO_DATASTREAM

/*****************************************************************************
  QGVector member functions
 *****************************************************************************/

QGVector::QGVector()				// create empty vector
{
    vec = 0;
    len = numItems = 0;
}

QGVector::QGVector( uint size )			// create vectors with nullptrs
{
    len = size;
    numItems = 0;
    if ( len == 0 ) {				// zero length
	vec = 0;
	return;
    }
    vec = NEW(Item,len);
    Q_CHECK_PTR( vec );
    memset( (void*)vec, 0, len*sizeof(Item) );	// fill with nulls
}

QGVector::QGVector( const QGVector &a )		// make copy of other vector
    : QPtrCollection( a )
{
    len = a.len;
    numItems = a.numItems;
    if ( len == 0 ) {
	vec = 0;
	return;
    }
    vec = NEW( Item, len );
    Q_CHECK_PTR( vec );
    for ( uint i = 0; i < len; i++ ) {
	if ( a.vec[i] ) {
	    vec[i] = newItem( a.vec[i] );
	    Q_CHECK_PTR( vec[i] );
	} else {
	    vec[i] = 0;
	}
    }
}

QGVector::~QGVector()
{
    clear();
}

QGVector& QGVector::operator=( const QGVector &v )
{
    if ( &v == this )
	return *this;

    clear();
    len = v.len;
    numItems = v.numItems;
    if ( len == 0 ) {
	vec = 0;
	return *this;
    }
    vec = NEW( Item, len );
    Q_CHECK_PTR( vec );
    for ( uint i = 0; i < len; i++ ) {
	if ( v.vec[i] ) {
	    vec[i] = newItem( v.vec[i] );
	    Q_CHECK_PTR( vec[i] );
	} else {
	    vec[i] = 0;
	}
    }
    return *this;
}


bool QGVector::insert( uint index, Item d )	// insert item at index
{
#if defined(QT_CHECK_RANGE)
    if ( index >= len ) {			// range error
	qWarning( "QGVector::insert: Index %d out of range", index );
	return FALSE;
    }
#endif
    if ( vec[index] ) {				// remove old item
	deleteItem( vec[index] );
	numItems--;
    }
    if ( d ) {
	vec[index] = newItem( d );
	Q_CHECK_PTR( vec[index] );
	numItems++;
	return vec[index] != 0;
    } else {
	vec[index] = 0;				// reset item
    }
    return TRUE;
}

bool QGVector::remove( uint index )		// remove item at index
{
#if defined(QT_CHECK_RANGE)
    if ( index >= len ) {			// range error
	qWarning( "QGVector::remove: Index %d out of range", index );
	return FALSE;
    }
#endif
    if ( vec[index] ) {				// valid item
	deleteItem( vec[index] );		// delete it
	vec[index] = 0;				// reset pointer
	numItems--;
    }
    return TRUE;
}

QPtrCollection::Item QGVector::take( uint index )		// take out item
{
#if defined(QT_CHECK_RANGE)
    if ( index >= len ) {			// range error
	qWarning( "QGVector::take: Index %d out of range", index );
	return 0;
    }
#endif
    Item d = vec[index];				// don't delete item
    if ( d )
	numItems--;
    vec[index] = 0;
    return d;
}

void QGVector::clear()				// clear vector
{
    if ( vec ) {
	for ( uint i=0; i<len; i++ ) {		// delete each item
	    if ( vec[i] )
		deleteItem( vec[i] );
	}
	DELETE(vec);
	vec = 0;
	len = numItems = 0;
    }
}

bool QGVector::resize( uint newsize )		// resize array
{
    if ( newsize == len )			// nothing to do
	return TRUE;
    if ( vec ) {				// existing data
	if ( newsize < len ) {			// shrink vector
	    uint i = newsize;
	    while ( i < len ) {			// delete lost items
		if ( vec[i] ) {
		    deleteItem( vec[i] );
		    numItems--;
		}
		i++;
	    }
	}
	if ( newsize == 0 ) {			// vector becomes empty
	    DELETE(vec);
	    vec = 0;
	    len = numItems = 0;
	    return TRUE;
	}
#if defined(DONT_USE_REALLOC)
	if ( newsize == 0 ) {
	    DELETE(vec);
	    vec = 0;
	    return FALSE;
	}
	Item *newvec = NEW(Item,newsize);		// manual realloc
	memcpy( newvec, vec, (len < newsize ? len : newsize)*sizeof(Item) );
	DELETE(vec);
	vec = newvec;
#else
	vec = (Item*)realloc( (char *)vec, newsize*sizeof(Item) );
#endif
    } else {					// create new vector
	vec = NEW(Item,newsize);
	len = numItems = 0;
    }
    Q_CHECK_PTR( vec );
    if ( !vec )					// no memory
	return FALSE;
    if ( newsize > len )			// init extra space added
	memset( (void*)&vec[len], 0, (newsize-len)*sizeof(Item) );
    len = newsize;
    return TRUE;
}


bool QGVector::fill( Item d, int flen )		// resize and fill vector
{
    if ( flen < 0 )
	flen = len;				// default: use vector length
    else if ( !resize( flen ) )
	return FALSE;
    for ( uint i=0; i<(uint)flen; i++ )		// insert d at every index
	insert( i, d );
    return TRUE;
}


static QGVector *sort_vec=0;			// current sort vector


#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

#ifdef Q_OS_TEMP
static int _cdecl cmp_vec( const void *n1, const void *n2 )
#else
static int cmp_vec( const void *n1, const void *n2 )
#endif
{
    return sort_vec->compareItems( *((QPtrCollection::Item*)n1), *((QPtrCollection::Item*)n2) );
}

#if defined(Q_C_CALLBACKS)
}
#endif


void QGVector::sort()				// sort vector
{
    if ( count() == 0 )				// no elements
	return;
    register Item *start = &vec[0];
    register Item *end	= &vec[len-1];
    Item tmp;
    for (;;) {				// put all zero elements behind
	while ( start < end && *start != 0 )
	    start++;
	while ( end > start && *end == 0 )
	    end--;
	if ( start < end ) {
	    tmp = *start;
	    *start = *end;
	    *end = tmp;
	} else {
	    break;
	}
    }

#ifdef QT_THREAD_SUPPORT
    QMutexLocker locker( qt_global_mutexpool->get( &sort_vec ) );
#endif // QT_THREAD_SUPPORT

    sort_vec = (QGVector*)this;
    qsort( vec, count(), sizeof(Item), cmp_vec );
    sort_vec = 0;
}

int QGVector::bsearch( Item d ) const		// binary search; when sorted
{
    if ( !len )
	return -1;
    if ( !d ) {
#if defined(QT_CHECK_NULL)
	qWarning( "QGVector::bsearch: Cannot search for null object" );
#endif
	return -1;
    }
    int n1 = 0;
    int n2 = len - 1;
    int mid = 0;
    bool found = FALSE;
    while ( n1 <= n2 ) {
	int  res;
	mid = (n1 + n2)/2;
	if ( vec[mid] == 0 )			// null item greater
	    res = -1;
	else
	    res = ((QGVector*)this)->compareItems( d, vec[mid] );
	if ( res < 0 )
	    n2 = mid - 1;
	else if ( res > 0 )
	    n1 = mid + 1;
	else {					// found it
	    found = TRUE;
	    break;
	}
    }
    if ( !found )
	return -1;
    // search to first of equal items
    while ( (mid - 1 >= 0) && !((QGVector*)this)->compareItems(d, vec[mid-1]) )
	mid--;
    return mid;
}

int QGVector::findRef( Item d, uint index) const // find exact item in vector
{
#if defined(QT_CHECK_RANGE)
    if ( index > len ) {			// range error
	qWarning( "QGVector::findRef: Index %d out of range", index );
	return -1;
    }
#endif
    for ( uint i=index; i<len; i++ ) {
	if ( vec[i] == d )
	    return i;
    }
    return -1;
}

int QGVector::find( Item d, uint index ) const	// find equal item in vector
{
#if defined(QT_CHECK_RANGE)
    if ( index >= len ) {			// range error
	qWarning( "QGVector::find: Index %d out of range", index );
	return -1;
    }
#endif
    for ( uint i=index; i<len; i++ ) {
	if ( vec[i] == 0 && d == 0 )		// found null item
	    return i;
	if ( vec[i] && ((QGVector*)this)->compareItems( vec[i], d ) == 0 )
	    return i;
    }
    return -1;
}

uint QGVector::containsRef( Item d ) const	// get number of exact matches
{
    uint count = 0;
    for ( uint i=0; i<len; i++ ) {
	if ( vec[i] == d )
	    count++;
    }
    return count;
}

uint QGVector::contains( Item d ) const		// get number of equal matches
{
    uint count = 0;
    for ( uint i=0; i<len; i++ ) {
	if ( vec[i] == 0 && d == 0 )		// count null items
	    count++;
	if ( vec[i] && ((QGVector*)this)->compareItems( vec[i], d ) == 0 )
	    count++;
    }
    return count;
}

bool QGVector::insertExpand( uint index, Item d )// insert and grow if necessary
{
    if ( index >= len ) {
	if ( !resize( index+1 ) )		// no memory
	    return FALSE;
    }
    insert( index, d );
    return TRUE;
}

void QGVector::toList( QGList *list ) const	// store items in list
{
    list->clear();
    for ( uint i=0; i<len; i++ ) {
	if ( vec[i] )
	    list->append( vec[i] );
    }
}


void QGVector::warningIndexRange( uint i )
{
#if defined(QT_CHECK_RANGE)
    qWarning( "QGVector::operator[]: Index %d out of range", i );
#else
    Q_UNUSED( i )
#endif
}


/*****************************************************************************
  QGVector stream functions
 *****************************************************************************/
#ifndef QT_NO_DATASTREAM
QDataStream &operator>>( QDataStream &s, QGVector &vec )
{						// read vector
    return vec.read( s );
}

QDataStream &operator<<( QDataStream &s, const QGVector &vec )
{						// write vector
    return vec.write( s );
}

QDataStream &QGVector::read( QDataStream &s )	// read vector from stream
{
    uint num;
    s >> num;					// read number of items
    clear();					// clear vector
    resize( num );
    for (uint i=0; i<num; i++) {		// read all items
	Item d;
	read( s, d );
	Q_CHECK_PTR( d );
	if ( !d )				// no memory
	    break;
	vec[i] = d;
    }
    return s;
}

QDataStream &QGVector::write( QDataStream &s ) const
{						// write vector to stream
    uint num = count();
    s << num;					// number of items to write
    num = size();
    for (uint i=0; i<num; i++) {		// write non-null items
	if ( vec[i] )
	    write( s, vec[i] );
    }
    return s;
}

/* Returns whether v equals this vector or not */

bool QGVector::operator==( const QGVector &v ) const
{
    if ( size() != v.size() )
	return FALSE;
    if ( count() != v.count() )
	return FALSE;
    for ( int i = 0; i < (int)size(); ++i ) {
	if ( ( (QGVector*)this )->compareItems( at( i ), v.at( i ) ) != 0 )
	    return FALSE;
    }
    return TRUE;
}

#endif // QT_NO_DATASTREAM
