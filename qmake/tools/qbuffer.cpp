/****************************************************************************
** $Id: qbuffer.cpp,v 1.1 2002-11-01 00:10:44 kergoth Exp $
**
** Implementation of QBuffer class
**
** Created : 930812
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

#include "qbuffer.h"
#include <stdlib.h>

/*!
    \class QBuffer qbuffer.h
    \reentrant
    \brief The QBuffer class is an I/O device that operates on a QByteArray.

    \ingroup io
    \ingroup collection

    QBuffer is used to read and write to a memory buffer. It is
    normally used with a QTextStream or a QDataStream. QBuffer has an
    associated QByteArray which holds the buffer data. The size() of
    the buffer is automatically adjusted as data is written.

    The constructor \c QBuffer(QByteArray) creates a QBuffer using an
    existing byte array. The byte array can also be set with
    setBuffer(). Writing to the QBuffer will modify the original byte
    array because QByteArray is \link shclass.html explicitly
    shared.\endlink

    Use open() to open the buffer before use and to set the mode
    (read-only, write-only, etc.). close() closes the buffer. The
    buffer must be closed before reopening or calling setBuffer().

    A common way to use QBuffer is through \l QDataStream or \l
    QTextStream, which have constructors that take a QBuffer
    parameter. For convenience, there are also QDataStream and
    QTextStream constructors that take a QByteArray parameter. These
    constructors create and open an internal QBuffer.

    Note that QTextStream can also operate on a QString (a Unicode
    string); a QBuffer cannot.

    You can also use QBuffer directly through the standard QIODevice
    functions readBlock(), writeBlock() readLine(), at(), getch(),
    putch() and ungetch().

    \sa QFile, QDataStream, QTextStream, QByteArray, \link shclass.html Shared Classes\endlink
*/


/*!
    Constructs an empty buffer.
*/

QBuffer::QBuffer()
{
    setFlags( IO_Direct );
    a_inc = 16;                                 // initial increment
    a_len = 0;
    ioIndex = 0;
}


/*!
    Constructs a buffer that operates on \a buf.

    If you open the buffer in write mode (\c IO_WriteOnly or
    \c IO_ReadWrite) and write something into the buffer, \a buf
    will be modified.

    Example:
    \code
    QCString str = "abc";
    QBuffer b( str );
    b.open( IO_WriteOnly );
    b.at( 3 ); // position at the 4th character (the terminating \0)
    b.writeBlock( "def", 4 ); // write "def" including the terminating \0
    b.close();
    // Now, str == "abcdef" with a terminating \0
    \endcode

    \sa setBuffer()
*/

QBuffer::QBuffer( QByteArray buf ) : a(buf)
{
    setFlags( IO_Direct );
    a_len = a.size();
    a_inc = (a_len > 512) ? 512 : a_len;        // initial increment
    if ( a_inc < 16 )
        a_inc = 16;
    ioIndex = 0;
}

/*!
    Destroys the buffer.
*/

QBuffer::~QBuffer()
{
}


/*!
    Replaces the buffer's contents with \a buf and returns TRUE.

    Does nothing (and returns FALSE) if isOpen() is TRUE.

    Note that if you open the buffer in write mode (\c IO_WriteOnly or
    IO_ReadWrite) and write something into the buffer, \a buf is also
    modified because QByteArray is an explicitly shared class.

    \sa buffer(), open(), close()
*/

bool QBuffer::setBuffer( QByteArray buf )
{
    if ( isOpen() ) {
#if defined(QT_CHECK_STATE)
        qWarning( "QBuffer::setBuffer: Buffer is open" );
#endif
        return FALSE;
    }
    a = buf;
    a_len = a.size();
    a_inc = (a_len > 512) ? 512 : a_len;        // initial increment
    if ( a_inc < 16 )
        a_inc = 16;
    ioIndex = 0;
    return TRUE;
}

/*!
    \fn QByteArray QBuffer::buffer() const

    Returns this buffer's byte array.

    \sa setBuffer()
*/

/*!
    \reimp

    Opens the buffer in mode \a m. Returns TRUE if successful;
    otherwise returns FALSE. The buffer must be opened before use.

    The mode parameter \a m must be a combination of the following flags.
    \list
    \i \c IO_ReadOnly opens the buffer in read-only mode.
    \i \c IO_WriteOnly opens the buffer in write-only mode.
    \i \c IO_ReadWrite opens the buffer in read/write mode.
    \i \c IO_Append sets the buffer index to the end of the buffer.
    \i \c IO_Truncate truncates the buffer.
    \endlist

    \sa close(), isOpen()
*/

bool QBuffer::open( int m  )
{
    if ( isOpen() ) {                           // buffer already open
#if defined(QT_CHECK_STATE)
        qWarning( "QBuffer::open: Buffer already open" );
#endif
        return FALSE;
    }
    setMode( m );
    if ( m & IO_Truncate ) {                    // truncate buffer
        a.resize( 0 );
        a_len = 0;
    }
    if ( m & IO_Append ) {                      // append to end of buffer
        ioIndex = a.size();
    } else {
        ioIndex = 0;
    }
    a_inc = 16;
    setState( IO_Open );
    setStatus( 0 );
    return TRUE;
}

/*!
    \reimp

    Closes an open buffer.

    \sa open()
*/

void QBuffer::close()
{
    if ( isOpen() ) {
        setFlags( IO_Direct );
        ioIndex = 0;
        a_inc = 16;
    }
}

/*!
    \reimp

    The flush function does nothing for a QBuffer.
*/

void QBuffer::flush()
{
    return;
}


/*!
    \fn QIODevice::Offset QBuffer::at() const

    \reimp
*/

/*!
    \fn QIODevice::Offset QBuffer::size() const

    \reimp
*/

/*!
  \reimp
*/

bool QBuffer::at( Offset pos )
{
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {
        qWarning( "QBuffer::at: Buffer is not open" );
        return FALSE;
    }
#endif
    if ( pos > a_len ) {
#if defined(QT_CHECK_RANGE)
#if defined(QT_LARGEFILE_SUPPORT) && defined(QT_ABI_64BITOFFSET)
        qWarning( "QBuffer::at: Index %llu out of range", pos );
#else
        qWarning( "QBuffer::at: Index %lu out of range", pos );
#endif
#endif
        return FALSE;
    }
    ioIndex = pos;
    return TRUE;
}


/*!
  \reimp
*/

Q_LONG QBuffer::readBlock( char *p, Q_ULONG len )
{
#if defined(QT_CHECK_STATE)
    if ( !p ) {
	qWarning( "QBuffer::readBlock: Null pointer error" );
	return -1;
    }
    if ( !isOpen() ) {                          // buffer not open
        qWarning( "QBuffer::readBlock: Buffer not open" );
        return -1;
    }
    if ( !isReadable() ) {                      // reading not permitted
        qWarning( "QBuffer::readBlock: Read operation not permitted" );
        return -1;
    }
#endif
    if ( ioIndex + len > a.size() ) {   // overflow
        if ( ioIndex >= a.size() ) {
            return 0;
        } else {
            len = a.size() - ioIndex;
        }
    }
    memcpy( p, a.data()+ioIndex, len );
    ioIndex += len;
    return len;
}

/*!
    \overload Q_LONG QBuffer::writeBlock( const QByteArray& data )

    This convenience function is the same as calling
    \c{writeBlock( data.data(), data.size() )} with \a data.
*/

/*!
    Writes \a len bytes from \a p into the buffer at the current
    index position, overwriting any characters there and extending the
    buffer if necessary. Returns the number of bytes actually written.

    Returns -1 if an error occurred.

    \sa readBlock()
*/

Q_LONG QBuffer::writeBlock( const char *p, Q_ULONG len )
{
    if ( len == 0 )
        return 0;

#if defined(QT_CHECK_NULL)
    if ( p == 0 ) {
        qWarning( "QBuffer::writeBlock: Null pointer error" );
        return -1;
    }
#endif
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {                          // buffer not open
        qWarning( "QBuffer::writeBlock: Buffer not open" );
        return -1;
    }
    if ( !isWritable() ) {                      // writing not permitted
        qWarning( "QBuffer::writeBlock: Write operation not permitted" );
        return -1;
    }
#endif
    if ( ioIndex + len >= a_len ) {             // overflow
        Q_ULONG new_len = a_len + a_inc*((ioIndex+len-a_len)/a_inc+1);
        if ( !a.resize( new_len ) ) {           // could not resize
#if defined(QT_CHECK_NULL)
            qWarning( "QBuffer::writeBlock: Memory allocation error" );
#endif
            setStatus( IO_ResourceError );
            return -1;
        }
        a_inc *= 2;                             // double increment
        a_len = new_len;
        a.shd->len = ioIndex + len;
    }
    memcpy( a.data()+ioIndex, p, len );
    ioIndex += len;
    if ( a.shd->len < ioIndex )
        a.shd->len = ioIndex;                   // fake (not alloc'd) length
    return len;
}


/*!
  \reimp
*/

Q_LONG QBuffer::readLine( char *p, Q_ULONG maxlen )
{
#if defined(QT_CHECK_NULL)
    if ( p == 0 ) {
        qWarning( "QBuffer::readLine: Null pointer error" );
        return -1;
    }
#endif
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {                          // buffer not open
        qWarning( "QBuffer::readLine: Buffer not open" );
        return -1;
    }
    if ( !isReadable() ) {                      // reading not permitted
        qWarning( "QBuffer::readLine: Read operation not permitted" );
        return -1;
    }
#endif
    if ( maxlen == 0 )
        return 0;
    Q_ULONG start = ioIndex;
    char *d = a.data() + ioIndex;
    maxlen--;                                   // make room for 0-terminator
    if ( a.size() - ioIndex < maxlen )
        maxlen = a.size() - ioIndex;
    while ( maxlen-- ) {
        if ( (*p++ = *d++) == '\n' )
            break;
    }
    *p = '\0';
    ioIndex = d - a.data();
    return ioIndex - start;
}


/*!
  \reimp
*/

int QBuffer::getch()
{
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {                          // buffer not open
        qWarning( "QBuffer::getch: Buffer not open" );
        return -1;
    }
    if ( !isReadable() ) {                      // reading not permitted
        qWarning( "QBuffer::getch: Read operation not permitted" );
        return -1;
    }
#endif
    if ( ioIndex+1 > a.size() ) {               // overflow
        setStatus( IO_ReadError );
        return -1;
    }
    return uchar(*(a.data()+ioIndex++));
}

/*!
    \reimp

    Writes the character \a ch into the buffer at the current index
    position, overwriting any existing character and extending the
    buffer if necessary.

    Returns \a ch, or -1 if an error occurred.

    \sa getch(), ungetch()
*/

int QBuffer::putch( int ch )
{
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {                          // buffer not open
        qWarning( "QBuffer::putch: Buffer not open" );
        return -1;
    }
    if ( !isWritable() ) {                      // writing not permitted
        qWarning( "QBuffer::putch: Write operation not permitted" );
        return -1;
    }
#endif
    if ( ioIndex + 1 >= a_len ) {               // overflow
        char buf[1];
        buf[0] = (char)ch;
        if ( writeBlock(buf,1) != 1 )
            return -1;                          // write error
    } else {
        *(a.data() + ioIndex++) = (char)ch;
        if ( a.shd->len < ioIndex )
            a.shd->len = ioIndex;
    }
    return ch;
}

/*!
  \reimp
*/

int QBuffer::ungetch( int ch )
{
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {                          // buffer not open
        qWarning( "QBuffer::ungetch: Buffer not open" );
        return -1;
    }
    if ( !isReadable() ) {                      // reading not permitted
        qWarning( "QBuffer::ungetch: Read operation not permitted" );
        return -1;
    }
#endif
    if ( ch != -1 ) {
        if ( ioIndex )
            ioIndex--;
        else
            ch = -1;
    }
    return ch;
}

