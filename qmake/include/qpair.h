/****************************************************************************
**
** Definition of QPair class
**
**
** Copyright (C) 1992-2001 Trolltech AS.  All rights reserved.
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

#ifndef QPAIR_H
#define QPAIR_H

#ifndef QT_H
#include "qglobal.h"
#include "qdatastream.h"
#endif // QT_H

template <class T1, class T2>
struct QPair
{
    typedef T1 first_type;
    typedef T2 second_type;

    QPair()
	: first( T1() ), second( T2() )
    {}
    QPair( const T1& t1, const T2& t2 )
	: first( t1 ), second( t2 )
    {}

    T1 first;
    T2 second;
};

template <class T1, class T2>
Q_INLINE_TEMPLATES bool operator==( const QPair<T1, T2>& x, const QPair<T1, T2>& y )
{
    return x.first == y.first && x.second == y.second;
}

template <class T1, class T2>
Q_INLINE_TEMPLATES bool operator<( const QPair<T1, T2>& x, const QPair<T1, T2>& y )
{
    return x.first < y.first ||
	   ( !( y.first < x.first ) && x.second < y.second );
}

template <class T1, class T2>
Q_INLINE_TEMPLATES QPair<T1, T2> qMakePair( const T1& x, const T2& y )
{
    return QPair<T1, T2>( x, y );
}

#ifndef QT_NO_DATASTREAM
template <class T1, class T2>
inline QDataStream& operator>>( QDataStream& s, QPair<T1, T2>& p )
{
    s >> p.first >> p.second;
    return s;
}

template <class T1, class T2>
inline QDataStream& operator<<( QDataStream& s, const QPair<T1, T2>& p )
{
    s << p.first << p.second;
    return s;
}
#endif

#endif
