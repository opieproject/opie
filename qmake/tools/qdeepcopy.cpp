/****************************************************************************
** $Id: qdeepcopy.cpp,v 1.1 2002-11-01 00:10:44 kergoth Exp $
**
** Implementation of QDeepCopy class
**
** Created : 20020613
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
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

#include "qdeepcopy.h"

/*!
    \class QDeepCopy qdeepcopy.h
    \brief The QDeepCopy class is a template class which ensures that
    implicitly shared and explicitly shared classes reference unique
    data.

    \reentrant

    \ingroup tools
    \ingroup shared

    Normally, shared copies reference the same data to optimize memory
    use and for maximum speed. In the example below, \c s1, \c s2, \c
    s3, \c s4 and \c s5 share data.

    \code
    // all 5 strings share the same data
    QString s1 = "abcd";
    QString s2 = s1;
    QString s3 = s2;
    QString s4 = s3;
    QString s5 = s2;
    \endcode

    QDeepCopy can be used several ways to ensure that an object
    references unique, unshared data. In the example below, \c s1, \c
    s2 and \c s5 share data, while neither \c s3 nor \c s4 share data.
    \code
    // s1, s2 and s5 share the same data, neither s3 nor s4 are shared
    QString s1 = "abcd";
    QString s2 = s1;
    QDeepCopy<QString> s3 = s2;  // s3 is a deep copy of s2
    QString s4 = s3;             // s4 is a deep copy of s3
    QString s5 = s2;
    \endcode

    In the example below, \c s1, \c s2 and \c s5 share data, and \c s3
    and \c s4 share data.
    \code
    // s1, s2 and s5 share the same data, s3 and s4 share the same data
    QString s1 = "abcd";
    QString s2 = s1;
    QString s3 = QDeepCopy<QString>( s2 );  // s3 is a deep copy of s2
    QString s4 = s3;                        // s4 is a shallow copy of s3
    QString s5 = s2;
    \endcode

    QDeepCopy can also provide safety in multithreaded applications
    that use shared classes. In the example below, the variable \c
    global_string is used safely since the data contained in \c
    global_string is always a deep copy. This ensures that all threads
    get a unique copy of the data, and that any assignments to \c
    global_string will result in a deep copy.

    \code
    QDeepCopy<QString> global_string;  // global string data
    QMutex global_mutex;               // mutex to protext global_string

    ...

    void setGlobalString( const QString &str )
    {
	global_mutex.lock();
	global_string = str;           // global_string is a deep copy of str
	global_mutex.unlock();
    }

    ...

    void MyThread::run()
    {
	global_mutex.lock();
	QString str = global_string;          // str is a deep copy of global_string
	global_mutex.unlock();

	// process the string data
	...

	// update global_string
	setGlobalString( str );
    }
    \endcode

    \warning It is the application developer's responsibility to
    protect the object shared across multiple threads.

    The examples above use QString, which is an implicitly shared
    class. The behavior of QDeepCopy is the same when using explicitly
    shared classes like QByteArray.

    Currently, QDeepCopy works with the following classes:
    \list
    \i QMemArray (including subclasses like QByteArray and QCString)
    \i QMap
    \i QString
    \i QValueList (including subclasses like QStringList and QValueStack)
    \i QValueVector
    \endlist

    \sa \link threads.html Thread Support in Qt \endlink
*/

/*!
    \fn QDeepCopy::QDeepCopy()

    Constructs an empty instance of type \e T.
*/

/*!
    \fn QDeepCopy::QDeepCopy( const T &t )

    Constructs a deep copy of \a t.
*/

/*!
    \fn QDeepCopy<T>& QDeepCopy::operator=( const T &t )

    Assigns a deep copy of \a t.
*/

/*!
    \fn QDeepCopy::operator T ()

    Returns a deep copy of the encapsulated data.
*/

