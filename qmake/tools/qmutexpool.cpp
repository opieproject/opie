/****************************************************************************
** $Id: qmutexpool.cpp,v 1.2 2003-07-10 02:40:12 llornkcor Exp $
**
** ...
**
** Copyright (C) 2002 Trolltech AS.  All rights reserved.
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

#include "qmutexpool_p.h"

#ifdef QT_THREAD_SUPPORT

#include <qthread.h>

QMutexPool *qt_global_mutexpool = 0;


/*!
    \class QMutexPool qmutexpool_p.h
    \brief The QMutexPool class provides a pool of QMutex objects.

    \internal

    \ingroup thread

    QMutexPool is a convenience class that provides access to a fixed
    number of QMutex objects.

    Typical use of a QMutexPool is in situations where it is not
    possible or feasible to use one QMutex for every protected object.
    The mutex pool will return a mutex based on the address of the
    object that needs protection.

    For example, consider this simple class:

    \code
    class Number {
    public:
	Number( double n ) : num ( n ) { }

	void setNumber( double n ) { num = n; }
	double number() const { return num; }

    private:
	double num;
    };
    \endcode

    Adding a QMutex member to the Number class does not make sense,
    because it is so small. However, in order to ensure that access to
    each Number is protected, you need to use a mutex. In this case, a
    QMutexPool would be ideal.

    Code to calculate the square of a number would then look something
    like this:

    \code
    void calcSquare( Number *num )
    {
	QMutexLocker locker( mutexpool.get( num ) );
	num.setNumber( num.number() * num.number() );
    }
    \endcode

    This function will safely calculate the square of a number, since
    it uses a mutex from a QMutexPool. The mutex is locked and
    unlocked automatically by the QMutexLocker class. See the
    QMutexLocker documentation for more details.
*/

/*!
    Constructs  a QMutexPool, reserving space for \a size QMutexes. If
    \a recursive is TRUE, all QMutexes in the pool will be recursive
    mutexes; otherwise they will all be non-recursive (the default).

    The QMutexes are created when needed, and deleted when the
    QMutexPool is destructed.
*/
QMutexPool::QMutexPool( bool recursive, int size )
    : mutex( FALSE ), count( size ), recurs( recursive )
{
    mutexes = new QMutex*[count];
    for ( int index = 0; index < count; ++index ) {
	mutexes[index] = 0;
    }
}

/*!
    Destructs a QMutexPool. All QMutexes that were created by the pool
    are deleted.
*/
QMutexPool::~QMutexPool()
{
    QMutexLocker locker( &mutex );
    for ( int index = 0; index < count; ++index ) {
	delete mutexes[index];
	mutexes[index] = 0;
    }
    delete [] mutexes;
    mutexes = 0;
}

/*!
    Returns a QMutex from the pool. QMutexPool uses the value \a
    address to determine which mutex is retured from the pool.
*/
QMutex *QMutexPool::get( void *address )
{
    int index = (int) ( (unsigned long) address % count );

    if ( ! mutexes[index] ) {
	// mutex not created, create one

	QMutexLocker locker( &mutex );
	// we need to check once again that the mutex hasn't been created, since
	// 2 threads could be trying to create a mutex as the same index...
	if ( ! mutexes[index] ) {
	    mutexes[index] = new QMutex( recurs );
	}
    }

    return mutexes[index];
}

#endif
