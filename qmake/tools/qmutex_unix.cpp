/****************************************************************************
** $Id: qmutex_unix.cpp,v 1.1 2002-11-01 00:10:44 kergoth Exp $
**
** QMutex class for Unix
**
** Created : 20010725
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

#if defined(QT_THREAD_SUPPORT)

#include "qplatformdefs.h"

typedef pthread_mutex_t     Q_MUTEX_T;

// POSIX threads mutex types
#if ((defined(PTHREAD_MUTEX_RECURSIVE) && defined(PTHREAD_MUTEX_DEFAULT)) || \
     defined(Q_OS_FREEBSD)) && !defined(Q_OS_UNIXWARE) && !defined(Q_OS_SOLARIS)
    // POSIX 1003.1c-1995 - We love this OS
#  define Q_MUTEX_SET_TYPE(a, b) pthread_mutexattr_settype((a), (b))
#  if defined(QT_CHECK_RANGE)
#    define Q_NORMAL_MUTEX_TYPE PTHREAD_MUTEX_ERRORCHECK
#  else
#    define Q_NORMAL_MUTEX_TYPE PTHREAD_MUTEX_DEFAULT
#  endif
#  define Q_RECURSIVE_MUTEX_TYPE PTHREAD_MUTEX_RECURSIVE
#elif defined(MUTEX_NONRECURSIVE_NP) && defined(MUTEX_RECURSIVE_NP)
// POSIX 1003.4a pthreads draft extensions
#  define Q_MUTEX_SET_TYPE(a, b) pthread_mutexattr_setkind_np((a), (b));
#  define Q_NORMAL_MUTEX_TYPE MUTEX_NONRECURSIVE_NP
#  define Q_RECURSIVE_MUTEX_TYPE MUTEX_RECURSIVE_NP
#else
// Unknown mutex types - skip them
#  define Q_MUTEX_SET_TYPE(a, b)
#  undef  Q_NORMAL_MUTEX_TYPE
#  undef  Q_RECURSIVE_MUTEX_TYPE
#endif

#include "qmutex.h"
#include "qmutex_p.h"

#include <errno.h>
#include <string.h>


// Private class declarations

class QRealMutexPrivate : public QMutexPrivate {
public:
    QRealMutexPrivate(bool = FALSE);

    void lock();
    void unlock();
    bool locked();
    bool trylock();
    int type() const;

    bool recursive;
};

#ifndef    Q_RECURSIVE_MUTEX_TYPE
class QRecursiveMutexPrivate : public QMutexPrivate
{
public:
    QRecursiveMutexPrivate();
    ~QRecursiveMutexPrivate();

    void lock();
    void unlock();
    bool locked();
    bool trylock();
    int type() const;

    int count;
    unsigned long owner;
    pthread_mutex_t handle2;
};
#endif // !Q_RECURSIVE_MUTEX_TYPE


// Private class implementation

// base destructor
QMutexPrivate::~QMutexPrivate()
{
    int ret = pthread_mutex_destroy(&handle);

#ifdef QT_CHECK_RANGE
    if ( ret )
	qWarning( "Mutex destroy failure: %s", strerror( ret ) );
#endif
}

// real mutex class
QRealMutexPrivate::QRealMutexPrivate(bool recurs)
    : recursive(recurs)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    Q_MUTEX_SET_TYPE(&attr, recursive ? Q_RECURSIVE_MUTEX_TYPE : Q_NORMAL_MUTEX_TYPE);
    Q_UNUSED(recursive);
    int ret = pthread_mutex_init(&handle, &attr);
    pthread_mutexattr_destroy(&attr);

#ifdef QT_CHECK_RANGE
    if( ret )
	qWarning( "Mutex init failure: %s", strerror( ret ) );
#endif // QT_CHECK_RANGE
}

void QRealMutexPrivate::lock()
{
    int ret = pthread_mutex_lock(&handle);

#ifdef QT_CHECK_RANGE
    if (ret)
	qWarning("Mutex lock failure: %s", strerror(ret));
#endif
}

void QRealMutexPrivate::unlock()
{
    int ret = pthread_mutex_unlock(&handle);

#ifdef QT_CHECK_RANGE
    if (ret)
	qWarning("Mutex unlock failure: %s", strerror(ret));
#endif
}

bool QRealMutexPrivate::locked()
{
    int ret = pthread_mutex_trylock(&handle);

    if (ret == EBUSY) {
	return TRUE;
    } else if (ret) {
#ifdef QT_CHECK_RANGE
	qWarning("Mutex locktest failure: %s", strerror(ret));
#endif
    } else
	pthread_mutex_unlock(&handle);

    return FALSE;
}

bool QRealMutexPrivate::trylock()
{
    int ret = pthread_mutex_trylock(&handle);

    if (ret == EBUSY) {
	return FALSE;
    } else if (ret) {
#ifdef QT_CHECK_RANGE
	qWarning("Mutex trylock failure: %s", strerror(ret));
#endif
	return FALSE;
    }

    return TRUE;
}

int QRealMutexPrivate::type() const
{
    return recursive ? Q_MUTEX_RECURSIVE : Q_MUTEX_NORMAL;
}


#ifndef    Q_RECURSIVE_MUTEX_TYPE
QRecursiveMutexPrivate::QRecursiveMutexPrivate()
    : count(0), owner(0)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    Q_MUTEX_SET_TYPE(&attr, Q_NORMAL_MUTEX_TYPE);
    int ret = pthread_mutex_init(&handle, &attr);
    pthread_mutexattr_destroy(&attr);

#  ifdef QT_CHECK_RANGE
    if (ret)
	qWarning( "Mutex init failure: %s", strerror(ret) );
#  endif

    pthread_mutexattr_init(&attr);
    ret = pthread_mutex_init( &handle2, &attr );
    pthread_mutexattr_destroy(&attr);

#  ifdef QT_CHECK_RANGE
    if (ret)
	qWarning( "Mutex init failure: %s", strerror(ret) );
#  endif
}

QRecursiveMutexPrivate::~QRecursiveMutexPrivate()
{
    int ret = pthread_mutex_destroy(&handle2);

#  ifdef QT_CHECK_RANGE
    if (ret)
	qWarning( "Mutex destroy failure: %s", strerror(ret) );
#  endif
}

void QRecursiveMutexPrivate::lock()
{
    pthread_mutex_lock(&handle2);

    if (count > 0 && owner == (unsigned long) pthread_self()) {
	count++;
    } else {
	pthread_mutex_unlock(&handle2);
	pthread_mutex_lock(&handle);
	pthread_mutex_lock(&handle2);
	count = 1;
	owner = (unsigned long) pthread_self();
    }

    pthread_mutex_unlock(&handle2);
}

void QRecursiveMutexPrivate::unlock()
{
    pthread_mutex_lock(&handle2);

    if (owner == (unsigned long) pthread_self()) {
	// do nothing if the count is already 0... to reflect the behaviour described
	// in the docs
	if (count && (--count) < 1) {
	    count = 0;
	    pthread_mutex_unlock(&handle);
	}
    } else {
#ifdef QT_CHECK_RANGE
	qWarning("QMutex::unlock: unlock from different thread than locker");
	qWarning("                was locked by %d, unlock attempt from %d",
		 (int)owner, (int)pthread_self());
#endif
    }

    pthread_mutex_unlock(&handle2);
}

bool QRecursiveMutexPrivate::locked()
{
    pthread_mutex_lock(&handle2);

    bool ret;
    int code = pthread_mutex_trylock(&handle);

    if (code == EBUSY) {
	ret = TRUE;
    } else {
#ifdef QT_CHECK_RANGE
	if (code)
	    qWarning("Mutex trylock failure: %s", strerror(code));
#endif

	pthread_mutex_unlock(&handle);
	ret = FALSE;
    }

    pthread_mutex_unlock(&handle2);

    return ret;
}

bool QRecursiveMutexPrivate::trylock()
{
    bool ret = TRUE;

    pthread_mutex_lock(&handle2);

    if ( count > 0 && owner == (unsigned long) pthread_self() ) {
	count++;
    } else {
        int code = pthread_mutex_trylock(&handle);

	if (code == EBUSY) {
	    ret = FALSE;
	} else if (code) {
#ifdef QT_CHECK_RANGE
	    qWarning("Mutex trylock failure: %s", strerror(code));
#endif
	    ret = FALSE;
	} else {
	    count = 1;
	    owner = (unsigned long) pthread_self();
	}
    }

    pthread_mutex_unlock(&handle2);

    return ret;
}

int QRecursiveMutexPrivate::type() const
{
    return Q_MUTEX_RECURSIVE;
}

#endif // !Q_RECURSIVE_MUTEX_TYPE


/*!
    \class QMutex qmutex.h
    \threadsafe
    \brief The QMutex class provides access serialization between threads.

    \ingroup thread
    \ingroup environment

    The purpose of a QMutex is to protect an object, data structure or
    section of code so that only one thread can access it at a time
    (This is similar to the Java \c synchronized keyword). For
    example, say there is a method which prints a message to the user
    on two lines:

    \code
    int number = 6;

    void method1()
    {
        number *= 5;
	number /= 4;
    }

    void method1()
    {
        number *= 3;
	number /= 2;
    }
    \endcode

    If these two methods are called in succession, the following happens:

    \code
    // method1()
    number *= 5;	// number is now 30
    number /= 4;	// number is now 7

    // method2()
    number *= 3;	// nubmer is now 21
    number /= 2;	// number is now 10
    \endcode

    If these two methods are called simultaneously from two threads then the
    following sequence could result:

    \code
    // Thread 1 calls method1()
    number *= 5;	// number is now 30

    // Thread 2 calls method2().
    //
    // Most likely Thread 1 has been put to sleep by the operating
    // system to allow Thread 2 to run.
    number *= 3;	// number is now 90
    number /= 2;	// number is now 45

    // Thread 1 finishes executing.
    number /= 4;	// number is now 11, instead of 10
    \endcode

    If we add a mutex, we should get the result we want:

    \code
    QMutex mutex;
    int number = 6;

    void method1()
    {
	mutex.lock();
        number *= 5;
	number /= 4;
	mutex.unlock();
    }

    void method2()
    {
	mutex.lock();
        number *= 3;
	number /= 2;
	mutex.unlock();
    }
    \endcode

    Then only one thread can modify \c number at any given time and
    the result is correct. This is a trivial example, of course, but
    applies to any other case where things need to happen in a
    particular sequence.

    When you call lock() in a thread, other threads that try to call
    lock() in the same place will block until the thread that got the
    lock calls unlock(). A non-blocking alternative to lock() is
    tryLock().
*/

/*!
    Constructs a new mutex. The mutex is created in an unlocked state.
    A recursive mutex is created if \a recursive is TRUE; a normal
    mutex is created if \a recursive is FALSE (the default). With a
    recursive mutex, a thread can lock the same mutex multiple times
    and it will not be unlocked until a corresponding number of
    unlock() calls have been made.
*/
QMutex::QMutex(bool recursive)
{
#ifndef    Q_RECURSIVE_MUTEX_TYPE
    if ( recursive )
	d = new QRecursiveMutexPrivate();
    else
#endif // !Q_RECURSIVE_MUTEX_TYPE
	d = new QRealMutexPrivate(recursive);
}

/*!
    Destroys the mutex.

    \warning If you destroy a mutex that still holds a lock the
    resultant behavior is undefined.
*/
QMutex::~QMutex()
{
    delete d;
}

/*!
    Attempt to lock the mutex. If another thread has locked the mutex
    then this call will \e block until that thread has unlocked it.

    \sa unlock(), locked()
*/
void QMutex::lock()
{
    d->lock();
}

/*!
    Unlocks the mutex. Attempting to unlock a mutex in a different
    thread to the one that locked it results in an error. Unlocking a
    mutex that is not locked results in undefined behaviour (varies
    between different Operating Systems' thread implementations).

    \sa lock(), locked()
*/
void QMutex::unlock()
{
    d->unlock();
}

/*!
    Returns TRUE if the mutex is locked by another thread; otherwise
    returns FALSE.

    \warning Due to differing implementations of recursive mutexes on
    various platforms, calling this function from the same thread that
    previously locked the mutex will return undefined results.

    \sa lock(), unlock()
*/
bool QMutex::locked()
{
    return d->locked();
}

/*!
    Attempt to lock the mutex. If the lock was obtained, this function
    returns TRUE. If another thread has locked the mutex, this
    function returns FALSE, instead of waiting for the mutex to become
    available, i.e. it does not block.

    If the lock was obtained, the mutex must be unlocked with unlock()
    before another thread can successfully lock it.

    \sa lock(), unlock(), locked()
*/
bool QMutex::tryLock()
{
    return d->trylock();
}

/*!
    \class QMutexLocker qmutex.h
    \brief The QMutexLocker class simplifies locking and unlocking QMutexes.

    \threadsafe

    \ingroup thread
    \ingroup environment

    The purpose of QMutexLocker is to simplify QMutex locking and
    unlocking. Locking and unlocking a QMutex in complex functions and
    statements or in exception handling code is error prone and
    difficult to debug. QMutexLocker should be used in such situations
    to ensure that the state of the mutex is well defined and always
    locked and unlocked properly.

    QMutexLocker should be created within a function where a QMutex
    needs to be locked. The mutex is locked when QMutexLocker is
    created, and unlocked when QMutexLocker is destroyed.

    For example, this complex function locks a QMutex upon entering
    the function and unlocks the mutex at all the exit points:

    \code
    int complexFunction( int flag )
    {
	mutex.lock();

	int return_value = 0;

	switch ( flag ) {
	case 0:
	case 1:
	    {
		mutex.unlock();
		return moreComplexFunction( flag );
	    }

	case 2:
	    {
		int status = anotherFunction();
		if ( status < 0 ) {
		    mutex.unlock();
		    return -2;
		}
		return_value = status + flag;
		break;
	    }

	default:
	    {
		if ( flag > 10 ) {
		    mutex.unlock();
		    return -1;
		}
		break;
	    }
	}

	mutex.unlock();
	return return_value;
    }
    \endcode

    This example function will get more complicated as it is
    developed, which increases the likelihood that errors will occur.

    Using QMutexLocker greatly simplifies the code, and makes it more
    readable:

    \code
    int complexFunction( int flag )
    {
	QMutexLocker locker( &mutex );

	int return_value = 0;

	switch ( flag ) {
	case 0:
	case 1:
	    {
		return moreComplexFunction( flag );
	    }

	case 2:
	    {
		int status = anotherFunction();
		if ( status < 0 )
		    return -2;
		return_value = status + flag;
		break;
	    }

	default:
	    {
		if ( flag > 10 )
		    return -1;
		break;
	    }
	}

	return return_value;
    }
    \endcode

    Now, the mutex will always be unlocked when the QMutexLocker
    object is destroyed (when the function returns since \c locker is
    an auto variable).

    The same principle applies to code that throws and catches
    exceptions. An exception that is not caught in the function that
    has locked the mutex has no way of unlocking the mutex before the
    exception is passed up the stack to the calling function.

    QMutexLocker also provides a mutex() member function that returns
    the mutex on which the QMutexLocker is operating. This is useful
    for code that needs access to the mutex, such as
    QWaitCondition::wait(). For example:

    \code
    class SignalWaiter
    {
    private:
	QMutexLocker locker;

    public:
	SignalWaiter( QMutex *mutex )
	    : locker( mutex )
	{
	}

	void waitForSignal()
	{
	    ...
	    ...
	    ...

	    while ( ! signalled )
		waitcondition.wait( locker.mutex() );

	    ...
	    ...
	    ...
	}
    };
    \endcode

    \sa QMutex, QWaitCondition
*/

/*!
    \fn QMutexLocker::QMutexLocker( QMutex *mutex )

    Constructs a QMutexLocker and locks \a mutex. The mutex will be
    unlocked when the QMutexLocker is destroyed.

    \sa QMutex::lock()
*/

/*!
    \fn QMutexLocker::~QMutexLocker()

    Destroys the QMutexLocker and unlocks the mutex which was locked
    in the constructor.

    \sa QMutexLocker::QMutexLocker(), QMutex::unlock()
*/

/*!
    \fn QMutex *QMutexLocker::mutex() const

    Returns a pointer to the mutex which was locked in the
    constructor.

    \sa QMutexLocker::QMutexLocker()
*/

#endif // QT_THREAD_SUPPORT
