/****************************************************************************
** $Id: qstrlist.h,v 1.1 2002-11-01 00:10:43 kergoth Exp $
**
** Definition of QStrList, QStrIList and QStrListIterator classes
**
** Created : 920730
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

#ifndef QSTRLIST_H
#define QSTRLIST_H

#ifndef QT_H
#include "qstring.h"
#include "qptrlist.h"
#include "qdatastream.h"
#endif // QT_H


#if defined(Q_TEMPLATEDLL)
Q_TEMPLATE_EXTERN template class Q_EXPORT QPtrList<char>;
Q_TEMPLATE_EXTERN template class Q_EXPORT QPtrListIterator<char>;
#endif

#if defined(Q_QDOC)
class QStrListIterator : public QPtrListIterator<char>
{
};
#else
typedef QPtrListIterator<char> QStrListIterator;
#endif

class Q_EXPORT QStrList : public QPtrList<char>
{
public:
    QStrList( bool deepCopies=TRUE ) { dc = deepCopies; del_item = deepCopies; }
    QStrList( const QStrList & );
    ~QStrList()			{ clear(); }
    QStrList& operator=( const QStrList & );

private:
    QPtrCollection::Item newItem( QPtrCollection::Item d ) { return dc ? qstrdup( (const char*)d ) : d; }
    void deleteItem( QPtrCollection::Item d ) { if ( del_item ) delete[] (char*)d; }
    int compareItems( QPtrCollection::Item s1, QPtrCollection::Item s2 ) { return qstrcmp((const char*)s1,
							 (const char*)s2); }
#ifndef QT_NO_DATASTREAM
    QDataStream &read( QDataStream &s, QPtrCollection::Item &d )
				{ s >> (char *&)d; return s; }
    QDataStream &write( QDataStream &s, QPtrCollection::Item d ) const
				{ return s << (const char *)d; }
#endif
    bool  dc;
};


class Q_EXPORT QStrIList : public QStrList	// case insensitive string list
{
public:
    QStrIList( bool deepCopies=TRUE ) : QStrList( deepCopies ) {}
    ~QStrIList()			{ clear(); }
private:
    int	  compareItems( QPtrCollection::Item s1, QPtrCollection::Item s2 )
				{ return qstricmp((const char*)s1,
						    (const char*)s2); }
};


inline QStrList & QStrList::operator=( const QStrList &strList )
{
    clear();
    dc = strList.dc;
    del_item = dc;
    QPtrList<char>::operator=( strList );
    return *this;
}

inline QStrList::QStrList( const QStrList &strList )
    : QPtrList<char>( strList )
{
    dc = FALSE;
    operator=( strList );
}

#endif // QSTRLIST_H
