#include "qmutexpool_p.h"

#ifdef QT_THREAD_SUPPORT

#include <qthread.h>
#include <stdio.h>

QMutexPool *qt_global_mutexpool = 0;

// this is an internal class used only for inititalizing the global mutexpool
class QGlobalMutexPoolInitializer
{
public:
    inline QGlobalMutexPoolInitializer()
    {
	/*
	  Purify will report a leak here. However, this mutex pool must be alive
	  until *everything* in Qt has been destructed. Unfortunately there is
	  no way to guarantee this, so we never destroy this mutex pool.
	*/
	qt_global_mutexpool = new QMutexPool( TRUE );
    }
};
QGlobalMutexPoolInitializer qt_global_mutexpool_initializer;

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
    : mutex( FALSE ), mutexes( size ), recurs( recursive )
{
    mutexes.fill( 0 );
}

/*!
    Destructs a QMutexPool. All QMutexes that were created by the pool
    are deleted.
*/
QMutexPool::~QMutexPool()
{
    QMutexLocker locker( &mutex );
    QMutex **d = mutexes.data();
    for ( int index = 0; (uint) index < mutexes.size(); index++ ) {
	delete d[index];
	d[index] = 0;
    }
}

/*!
    Returns a QMutex from the pool. QMutexPool uses the value \a
    address to determine which mutex is retured from the pool.
*/
QMutex *QMutexPool::get( void *address )
{
    QMutex **d = mutexes.data();
    int index = (int)( (ulong) address % mutexes.size() );

    if ( ! d[index] ) {
	// mutex not created, create one

	QMutexLocker locker( &mutex );
	// we need to check once again that the mutex hasn't been created, since
	// 2 threads could be trying to create a mutex as the same index...
	if ( ! d[index] ) {
	    d[index] = new QMutex( recurs );
	}
    }

    return d[index];
}

#endif
