/****************************************************************************
** $Id: qptrlist.h,v 1.1 2002-11-01 00:10:43 kergoth Exp $
**
** Definition of QPtrList template/macro class
**
** Created :
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

#ifndef QPTRLIST_H
#define QPTRLIST_H

#ifndef QT_H
#include "qglist.h"
#endif // QT_H


template<class type>
class QPtrList
#ifdef Q_QDOC
	: public QPtrCollection
#else
	: public QGList
#endif
{
public:
    QPtrList()				{}
    QPtrList( const QPtrList<type> &l ) : QGList(l) {}
    ~QPtrList()				{ clear(); }
    QPtrList<type> &operator=(const QPtrList<type> &l)
			{ return (QPtrList<type>&)QGList::operator=(l); }
    bool operator==( const QPtrList<type> &list ) const
    { return QGList::operator==( list ); }
    bool operator!=( const QPtrList<type> &list ) const
    { return !QGList::operator==( list ); }
    uint  count()   const		{ return QGList::count(); }
    bool  isEmpty() const		{ return QGList::count() == 0; }
    bool  insert( uint i, const type *d){ return QGList::insertAt(i,(QPtrCollection::Item)d); }
    void  inSort( const type *d )	{ QGList::inSort((QPtrCollection::Item)d); }
    void  prepend( const type *d )	{ QGList::insertAt(0,(QPtrCollection::Item)d); }
    void  append( const type *d )	{ QGList::append((QPtrCollection::Item)d); }
    bool  remove( uint i )		{ return QGList::removeAt(i); }
    bool  remove()			{ return QGList::remove((QPtrCollection::Item)0); }
    bool  remove( const type *d )	{ return QGList::remove((QPtrCollection::Item)d); }
    bool  removeRef( const type *d )	{ return QGList::removeRef((QPtrCollection::Item)d); }
    void  removeNode( QLNode *n )	{ QGList::removeNode(n); }
    bool  removeFirst()			{ return QGList::removeFirst(); }
    bool  removeLast()			{ return QGList::removeLast(); }
    type *take( uint i )		{ return (type *)QGList::takeAt(i); }
    type *take()			{ return (type *)QGList::take(); }
    type *takeNode( QLNode *n )		{ return (type *)QGList::takeNode(n); }
    void  clear()			{ QGList::clear(); }
    void  sort()			{ QGList::sort(); }
    int	  find( const type *d )		{ return QGList::find((QPtrCollection::Item)d); }
    int	  findNext( const type *d )	{ return QGList::find((QPtrCollection::Item)d,FALSE); }
    int	  findRef( const type *d )	{ return QGList::findRef((QPtrCollection::Item)d); }
    int	  findNextRef( const type *d ){ return QGList::findRef((QPtrCollection::Item)d,FALSE);}
    uint  contains( const type *d ) const { return QGList::contains((QPtrCollection::Item)d); }
    uint  containsRef( const type *d ) const
					{ return QGList::containsRef((QPtrCollection::Item)d); }
    bool replace( uint i, const type *d ) { return QGList::replaceAt( i, (QPtrCollection::Item)d ); }
    type *at( uint i )			{ return (type *)QGList::at(i); }
    int	  at() const			{ return QGList::at(); }
    type *current()  const		{ return (type *)QGList::get(); }
    QLNode *currentNode()  const	{ return QGList::currentNode(); }
    type *getFirst() const		{ return (type *)QGList::cfirst(); }
    type *getLast()  const		{ return (type *)QGList::clast(); }
    type *first()			{ return (type *)QGList::first(); }
    type *last()			{ return (type *)QGList::last(); }
    type *next()			{ return (type *)QGList::next(); }
    type *prev()			{ return (type *)QGList::prev(); }
    void  toVector( QGVector *vec )const{ QGList::toVector(vec); }

#ifdef Q_QDOC
protected:
    virtual int compareItems( QPtrCollection::Item, QPtrCollection::Item );
    virtual QDataStream& read( QDataStream&, QPtrCollection::Item& );
    virtual QDataStream& write( QDataStream&, QPtrCollection::Item ) const;
#endif

private:
    void  deleteItem( Item d );
};

#if !defined(Q_BROKEN_TEMPLATE_SPECIALIZATION)
template<> inline void QPtrList<void>::deleteItem( QPtrCollection::Item )
{
}
#endif

template<class type> inline void QPtrList<type>::deleteItem( QPtrCollection::Item d )
{
    if ( del_item ) delete (type *)d;
}

template<class type>
class QPtrListIterator : public QGListIterator
{
public:
    QPtrListIterator(const QPtrList<type> &l) :QGListIterator((QGList &)l) {}
   ~QPtrListIterator()	      {}
    uint  count()   const     { return list->count(); }
    bool  isEmpty() const     { return list->count() == 0; }
    bool  atFirst() const     { return QGListIterator::atFirst(); }
    bool  atLast()  const     { return QGListIterator::atLast(); }
    type *toFirst()	      { return (type *)QGListIterator::toFirst(); }
    type *toLast()	      { return (type *)QGListIterator::toLast(); }
    operator type *() const   { return (type *)QGListIterator::get(); }
    type *operator*()         { return (type *)QGListIterator::get(); }

    // No good, since QPtrList<char> (ie. QStrList fails...
    //
    // MSVC++ gives warning
    // Sunpro C++ 4.1 gives error
    //    type *operator->()        { return (type *)QGListIterator::get(); }

    type *current()   const   { return (type *)QGListIterator::get(); }
    type *operator()()	      { return (type *)QGListIterator::operator()();}
    type *operator++()	      { return (type *)QGListIterator::operator++(); }
    type *operator+=(uint j)  { return (type *)QGListIterator::operator+=(j);}
    type *operator--()	      { return (type *)QGListIterator::operator--(); }
    type *operator-=(uint j)  { return (type *)QGListIterator::operator-=(j);}
    QPtrListIterator<type>& operator=(const QPtrListIterator<type>&it)
			      { QGListIterator::operator=(it); return *this; }
};

#ifndef QT_NO_COMPAT
#define QList QPtrList
#define QListIterator QPtrListIterator
#endif

#endif // QPTRLIST_H
