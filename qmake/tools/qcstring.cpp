/****************************************************************************
** $Id: qcstring.cpp,v 1.1 2002-11-01 00:10:44 kergoth Exp $
**
** Implementation of extended char array operations, and QByteArray and
** QCString classes
**
** Created : 920722
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

#include "qstring.h"
#include "qregexp.h"
#include "qdatastream.h"

#ifdef QT_THREAD_SUPPORT
#  include <private/qmutexpool_p.h>
#endif // QT_THREAD_SUPPORT

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#ifndef QT_NO_COMPRESS
#include "../3rdparty/zlib/zlib.h"
#endif

/*****************************************************************************
  Safe and portable C string functions; extensions to standard string.h
 *****************************************************************************/

/*!
    \relates QCString

    This function is normally part of the C library. Qt implements
    memmove() for platforms that do not provide it.

    memmove() copies \a len bytes from \a src into \a dst. The data
    is copied correctly even if \a src and \a dst overlap.
*/

void *qmemmove( void *dst, const void *src, uint len )
{
    register char *d;
    register char *s;
    if ( dst > src ) {
	d = (char *)dst + len - 1;
	s = (char *)src + len - 1;
	while ( len-- )
	    *d-- = *s--;
    } else if ( dst < src ) {
	d = (char *)dst;
	s = (char *)src;
	while ( len-- )
	    *d++ = *s++;
    }
    return dst;
}


/*!
    \relates QCString

    Returns a duplicate string.

    Allocates space for a copy of \a src, copies it, and returns a
    pointer to the copy. If \a src is 0, it immediately returns 0.

    The returned string must be deleted using \c delete[].
*/

char *qstrdup( const char *src )
{
    if ( !src )
	return 0;
    char *dst = new char[strlen(src)+1];
    Q_CHECK_PTR( dst );
    return strcpy( dst, src );
}

/*!
    \fn char *qstrcpy( char *dst, const char *src )

    \relates QCString

    A safe strcpy() function.

    Copies all characters up to and including the '\0' from \a src
    into \a dst and returns a pointer to \a dst.
*/

/*!
    \relates QCString

    A safe strncpy() function.

    Copies at most \a len bytes from \a src (stopping at \a len or the
    terminating '\0' whichever comes first) into \a dst and returns a
    pointer to \a dst. Guarantees that \a dst is '\0'-terminated. If
    \a src or \a dst is 0, returns 0 immediately.

    \sa qstrcpy()
*/

char *qstrncpy( char *dst, const char *src, uint len )
{
    if ( !src || !dst )
	return 0;
    strncpy( dst, src, len );
    if ( len > 0 )
	dst[len-1] = '\0';
    return dst;
}

/*!
    \fn int qstrcmp( const char *str1, const char *str2 );

    \relates QCString

    A safe strcmp() function.

    Compares \a str1 and \a str2. Returns a negative value if \a str1
    is less than \a str2, 0 if \a str1 is equal to \a str2 or a
    positive value if \a str1 is greater than \a str2.

    Special case I: Returns 0 if \a str1 and \a str2 are both 0.

    Special case II: Returns a random nonzero value if \a str1 is 0
    or \a str2 is 0 (but not both).

    \sa qstrncmp() qstricmp() qstrnicmp()
	\link #asciinotion Note on character comparisons \endlink
*/

/*!
    \fn int qstrncmp( const char *str1, const char *str2, uint len );

    \relates QCString

    A safe strncmp() function.

    Compares at most \a len bytes of \a str1 and \a str2.

    Returns a negative value if \a str1 is less than \a str2, 0 if \a
    str1 is equal to \a str2 or a positive value if \a str1 is greater
    than \a str2.

    Special case I: Returns 0 if \a str1 and \a str2 are both 0.

    Special case II: Returns a random nonzero value if \a str1 is 0
    or \a str2 is 0 (but not both).

    \sa qstrcmp(), qstricmp(), qstrnicmp()
	\link #asciinotion Note on character comparisons \endlink
*/

/*!
    \relates QCString

    A safe stricmp() function.

    Compares \a str1 and \a str2 ignoring the case.

    Returns a negative value if \a str1 is less than \a str2, 0 if \a
    str1 is equal to \a str2 or a positive value if \a str1 is greater
    than \a str2.

    Special case I: Returns 0 if \a str1 and \a str2 are both 0.

    Special case II: Returns a random nonzero value if \a str1 is 0
    or \a str2 is 0 (but not both).

    \sa qstrcmp(), qstrncmp(), qstrnicmp()
	\link #asciinotion Note on character comparisons \endlink
*/

int qstricmp( const char *str1, const char *str2 )
{
    register const uchar *s1 = (const uchar *)str1;
    register const uchar *s2 = (const uchar *)str2;
    int res;
    uchar c;
    if ( !s1 || !s2 )
	return s1 ? 1 : ( s2 ? -1 : 0 );
    for ( ; !(res = (c=tolower(*s1)) - tolower(*s2)); s1++, s2++ )
	if ( !c )				// strings are equal
	    break;
    return res;
}

/*!
    \relates QCString

    A safe strnicmp() function.

    Compares at most \a len bytes of \a str1 and \a str2 ignoring the case.

    Returns a negative value if \a str1 is less than \a str2, 0 if \a str1
    is equal to \a str2 or a positive value if \a str1 is greater than \a
    str2.

    Special case I: Returns 0 if \a str1 and \a str2 are both 0.

    Special case II: Returns a random nonzero value if \a str1 is 0
    or \a str2 is 0 (but not both).

    \sa qstrcmp(), qstrncmp() qstricmp()
	\link #asciinotion Note on character comparisons \endlink
*/

int qstrnicmp( const char *str1, const char *str2, uint len )
{
    register const uchar *s1 = (const uchar *)str1;
    register const uchar *s2 = (const uchar *)str2;
    int res;
    uchar c;
    if ( !s1 || !s2 )
	return s1 ? 1 : ( s2 ? -1 : 0 );
    for ( ; len--; s1++, s2++ ) {
	if ( (res = (c=tolower(*s1)) - tolower(*s2)) )
	    return res;
	if ( !c )				// strings are equal
	    break;
    }
    return 0;
}


static Q_UINT16 crc_tbl[16];
static bool   crc_tbl_init = FALSE;

static void createCRC16Table()			// build CRC16 lookup table
{
    register uint i;
    register uint j;
    uint v0, v1, v2, v3;
    for ( i = 0; i < 16; i++ ) {
	v0 = i & 1;
	v1 = ( i >> 1 ) & 1;
	v2 = ( i >> 2 ) & 1;
	v3 = ( i >> 3 ) & 1;
	j = 0;
#undef SET_BIT
#define SET_BIT(x, b, v) (x) |= (v) << (b)
	SET_BIT( j,  0, v0 );
	SET_BIT( j,  7, v0 );
	SET_BIT( j, 12, v0 );
	SET_BIT( j,  1, v1 );
	SET_BIT( j,  8, v1 );
	SET_BIT( j, 13, v1 );
	SET_BIT( j,  2, v2 );
	SET_BIT( j,  9, v2 );
	SET_BIT( j, 14, v2 );
	SET_BIT( j,  3, v3 );
	SET_BIT( j, 10, v3 );
	SET_BIT( j, 15, v3 );
	crc_tbl[i] = j;
    }
}

/*!
    \relates QMemArray

    Returns the CRC-16 checksum of \a len bytes starting at \a data.

    The checksum is independent of the byte order (endianness).
*/

Q_UINT16 qChecksum( const char *data, uint len )
{
    if ( !crc_tbl_init ) {			// create lookup table

#ifdef QT_THREAD_SUPPORT
	QMutexLocker locker( qt_global_mutexpool->get( &crc_tbl_init ) );
#endif // QT_THREAD_SUPPORT

	if ( !crc_tbl_init ) {
	    createCRC16Table();
	    crc_tbl_init = TRUE;
	}
    }
    register Q_UINT16 crc = 0xffff;
    uchar c;
    uchar *p = (uchar *)data;
    while ( len-- ) {
	c = *p++;
	crc = ( (crc >> 4) & 0x0fff ) ^ crc_tbl[((crc ^ c) & 15)];
	c >>= 4;
	crc = ( (crc >> 4) & 0x0fff ) ^ crc_tbl[((crc ^ c) & 15)];
    }
    return ~crc & 0xffff;
}

/*! \fn QByteArray qCompress( const QByteArray& data)
  \relates QByteArray
  \overload
*/

/*!
  \relates QByteArray

  Compresses the array \a data which is \a nbytes long and returns the
  compressed byte array.

  \sa qUncompress()
*/

#ifndef QT_NO_COMPRESS
QByteArray qCompress( const uchar* data, int nbytes )
{
    if ( nbytes == 0 ) {
	QByteArray tmp( 4 );
	tmp.fill( 0 );
	return tmp;
    }
    if ( !data ) {
#if defined(QT_CHECK_RANGE)
	qWarning( "qCompress: data is NULL." );
#endif
	return QByteArray();
    }

    ulong len = nbytes * 2;
    QByteArray bazip;
    int res;
    do {
	bazip.resize( len + 4 );
	res = ::compress(  (uchar*)bazip.data()+4, &len, (uchar*)data, nbytes );

	switch ( res ) {
	case Z_OK:
	    bazip.resize( len + 4 );
	    bazip[0] = ( nbytes & 0xff000000 ) >> 24;
	    bazip[1] = ( nbytes & 0x00ff0000 ) >> 16;
	    bazip[2] = ( nbytes & 0x0000ff00 ) >> 8;
	    bazip[3] = ( nbytes & 0x000000ff );
	    break;
	case Z_MEM_ERROR:
#if defined(QT_CHECK_RANGE)
	    qWarning( "qCompress: Z_MEM_ERROR: Not enough memory." );
#endif
	    bazip.resize( 0 );
	    break;
	case Z_BUF_ERROR:
	    len *= 2;
	    break;
	}
    } while ( res == Z_BUF_ERROR );

    return bazip;
}
#endif

/*! \fn QByteArray qUncompress( const QByteArray& data )
  \relates QByteArray
  \overload
*/

/*!
  \relates QByteArray

  Uncompresses the array \a data which is \a nbytes long and returns
  the uncompressed byte array.

  Returns an empty QByteArray if the input data was corrupt.

  \sa qCompress()
*/

#ifndef QT_NO_COMPRESS
QByteArray qUncompress( const uchar* data, int nbytes )
{
    if ( !data ) {
#if defined(QT_CHECK_RANGE)
	qWarning( "qUncompress: data is NULL." );
#endif
	return QByteArray();
    }
    if ( nbytes <= 4 ) {
#if defined(QT_CHECK_RANGE)
	if ( nbytes < 4 || ( data[0]!=0 || data[1]!=0 || data[2]!=0 || data[3]!=0 ) )
	    qWarning( "qUncompress: Input data is corrupted." );
#endif
	return QByteArray();
    }
    ulong expectedSize = ( data[0] << 24 ) | ( data[1] << 16 ) | ( data[2] << 8 ) | data[3];
    ulong len = QMAX( expectedSize,  1 );
    QByteArray baunzip;
    int res;
    do {
	baunzip.resize( len );
	res = ::uncompress( (uchar*)baunzip.data(), &len,
				(uchar*)data+4, nbytes-4 );

	switch ( res ) {
	case Z_OK:
	    if ( len != baunzip.size() )
		baunzip.resize( len );
	    break;
	case Z_MEM_ERROR:
#if defined(QT_CHECK_RANGE)
	    qWarning( "qUncompress: Z_MEM_ERROR: Not enough memory." );
#endif
	    break;
	case Z_BUF_ERROR:
	    len *= 2;
	    break;
	case Z_DATA_ERROR:
#if defined(QT_CHECK_RANGE)
	    qWarning( "qUncompress: Z_DATA_ERROR: Input data is corrupted." );
#endif
	    break;
	}
    } while ( res == Z_BUF_ERROR );

    if ( res != Z_OK )
	baunzip = QByteArray();

    return baunzip;
}
#endif

/*****************************************************************************
  QByteArray documentation
 *****************************************************************************/

/*!
    \class QByteArray
    \reentrant
    \brief The QByteArray class provides an array of bytes.

    \ingroup collection
    \ingroup tools

    The QByteArray class provides an explicitly shared array of bytes.
    It is useful for manipulating memory areas with custom data.
    QByteArray is implemented as a QMemArray\<char\>. See the \l
    QMemArray documentation for further information.
*/

/*!
    \fn QByteArray::QByteArray()

    Constructs an empty QByteArray.
*/

/*!
    \fn QByteArray::QByteArray( int size )

    Constructs a QByteArray of size \a size.
*/

/*****************************************************************************
  QByteArray stream functions
 *****************************************************************************/

/*!
    \relates QMemArray

    Writes byte array \a a to the stream \a s and returns a reference
    to the stream.

    \sa \link datastreamformat.html Format of the QDataStream operators \endlink
*/
#ifndef QT_NO_DATASTREAM

QDataStream &operator<<( QDataStream &s, const QByteArray &a )
{
    return s.writeBytes( a.data(), a.size() );
}

/*!
    \relates QMemArray

    Reads a byte array into \a a from the stream \a s and returns a
    reference to the stream.

    \sa \link datastreamformat.html Format of the QDataStream operators \endlink
*/

QDataStream &operator>>( QDataStream &s, QByteArray &a )
{
    Q_UINT32 len;
    s >> len;					// read size of array
    if ( len == 0 || s.eof() ) {		// end of file reached
	a.resize( 0 );
	return s;
    }
    if ( !a.resize( (uint)len ) ) {		// resize array
#if defined(QT_CHECK_NULL)
	qWarning( "QDataStream: Not enough memory to read QByteArray" );
#endif
	len = 0;
    }
    if ( len > 0 )				// not null array
	s.readRawBytes( a.data(), (uint)len );
    return s;
}

#endif //QT_NO_DATASTREAM

/*****************************************************************************
  QCString member functions
 *****************************************************************************/

/*!
    \class QCString qcstring.h
    \reentrant
    \brief The QCString class provides an abstraction of the classic C
    zero-terminated char array (char *).

    \ingroup text
    \ingroup collection
    \ingroup tools
    \ingroup shared

    QCString inherits QByteArray, which is defined as
    QMemArray\<char\>. Since QCString is a QMemArray, it uses \link
    shclass.html explicit sharing\endlink with a reference count.

    QCString tries to behave like a more convenient \c{const char *}.
    The price of doing this is that some algorithms will perform
    badly. For example, append() is O(length()) since it scans for a
    null terminator. Although you might use QCString for text that is
    never exposed to the user, for most purposes, and especially for
    user-visible text, you should use QString. QString provides
    implicit sharing, Unicode and other internationalization support,
    and is well optimized.

    Note that for the QCString methods that take a \c{const char *}
    parameter the \c{const char *} must either be 0 (null) or not-null
    and '\0' (NUL byte) terminated; otherwise the results are
    undefined.

    A QCString that has not been assigned to anything is \e null, i.e.
    both the length and the data pointer is 0. A QCString that
    references the empty string ("", a single '\0' char) is \e empty.
    Both null and empty QCStrings are legal parameters to the methods.
    Assigning \c{const char *} 0 to QCString produces a null QCString.

    The length() function returns the length of the string; resize()
    resizes the string and truncate() truncates the string. A string
    can be filled with a character using fill(). Strings can be left
    or right padded with characters using leftJustify() and
    rightJustify(). Characters, strings and regular expressions can be
    searched for using find() and findRev(), and counted using
    contains().

    Strings and characters can be inserted with insert() and appended
    with append(). A string can be prepended with prepend().
    Characters can be removed from the string with remove() and
    replaced with replace().

    Portions of a string can be extracted using left(), right() and
    mid(). Whitespace can be removed using stripWhiteSpace() and
    simplifyWhiteSpace(). Strings can be converted to uppercase or
    lowercase with upper() and lower() respectively.

    Strings that contain numbers can be converted to numbers with
    toShort(), toInt(), toLong(), toULong(), toFloat() and toDouble().
    Numbers can be converted to strings with setNum().

    Many operators are overloaded to work with QCStrings. QCString
    also supports some more obscure functions, e.g. sprintf(),
    setStr() and setExpand().

    \target asciinotion
    \sidebar Note on Character Comparisons

    In QCString the notion of uppercase and lowercase and of which
    character is greater than or less than another character is locale
    dependent. This affects functions which support a case insensitive
    option or which compare or lowercase or uppercase their arguments.
    Case insensitive operations and comparisons will be accurate if
    both strings contain only ASCII characters. (If \c $LC_CTYPE is
    set, most Unix systems do "the right thing".) Functions that this
    affects include contains(), find(), findRev(), \l operator<(), \l
    operator<=(), \l operator>(), \l operator>=(), lower() and
    upper().

    This issue does not apply to \l{QString}s since they represent
    characters using Unicode.
    \endsidebar

    Performance note: The QCString methods for QRegExp searching are
    implemented by converting the QCString to a QString and performing
    the search on that. This implies a deep copy of the QCString data.
    If you are going to perform many QRegExp searches on a large
    QCString, you will get better performance by converting the
    QCString to a QString yourself, and then searching in the QString.
*/

/*!
    \fn QCString::QCString()

    Constructs a null string.

    \sa isNull()
*/

/*!
    \fn QCString::QCString( const QCString &s )

    Constructs a shallow copy \a s.

    \sa assign()
*/

/*!
    Constructs a string with room for \a size characters, including
    the '\0'-terminator. Makes a null string if \a size == 0.

    If \a size \> 0, then the first and last characters in the string
    are initialized to '\0'. All other characters are uninitialized.

    \sa resize(), isNull()
*/

QCString::QCString( int size )
    : QByteArray( size )
{
    if ( size > 0 ) {
	*data() = '\0';				// set terminator
	*(data()+(size-1)) = '\0';
    }
}

/*!
    Constructs a string that is a deep copy of \a str.

    If \a str is 0 a null string is created.

    \sa isNull()
*/

QCString::QCString( const char *str )
{
    duplicate( str, qstrlen(str) + 1 );
}


/*!
    Constructs a string that is a deep copy of \a str. The copy will
    be at most \a maxsize bytes long including the '\0'-terminator.

    Example:
    \code
    QCString str( "helloworld", 6 ); // assigns "hello" to str
    \endcode

    If \a str contains a 0 byte within the first \a maxsize bytes, the
    resulting QCString will be terminated by this 0. If \a str is 0 a
    null string is created.

    \sa isNull()
*/

QCString::QCString( const char *str, uint maxsize )
{
    if ( str == 0 )
	return;
    uint len; // index of first '\0'
    for ( len = 0; len < maxsize - 1; len++ ) {
	if ( str[len] == '\0' )
	    break;
    }
    QByteArray::resize( len + 1 );
    memcpy( data(), str, len );
    data()[len] = 0;
}

/*!
  \reimp
*/

QCString::~QCString()
{
}

/*!
    \fn QCString &QCString::operator=( const QCString &s )

    Assigns a shallow copy of \a s to this string and returns a
    reference to this string.
*/

/*!
    \overload QCString &QCString::operator=( const char *str )

    Assigns a deep copy of \a str to this string and returns a
    reference to this string.

    If \a str is 0 a null string is created.

    \sa isNull()
*/

/*!
    \fn bool QCString::isNull() const

    Returns TRUE if the string is null, i.e. if data() == 0; otherwise
    returns FALSE. A null string is also an empty string.

    Example:
    \code
    QCString a;		// a.data() == 0,  a.size() == 0, a.length() == 0
    QCString b == "";	// b.data() == "", b.size() == 1, b.length() == 0
    a.isNull();		// TRUE  because a.data() == 0
    a.isEmpty();	// TRUE  because a.length() == 0
    b.isNull();		// FALSE because b.data() == ""
    b.isEmpty();	// TRUE  because b.length() == 0
    \endcode

    \sa isEmpty(), length(), size()
*/

/*!
    \fn bool QCString::isEmpty() const

    Returns TRUE if the string is empty, i.e. if length() == 0;
    otherwise returns FALSE. An empty string is not always a null
    string.

    See example in isNull().

    \sa isNull(), length(), size()
*/

/*!
    \fn uint QCString::length() const

    Returns the length of the string, excluding the '\0'-terminator.
    Equivalent to calling \c strlen(data()).

    Null strings and empty strings have zero length.

    \sa size(), isNull(), isEmpty()
*/

/*!
    \fn bool QCString::truncate( uint pos )

    Truncates the string at position \a pos.

    Equivalent to calling \c resize(pos+1).

    Example:
    \code
    QCString s = "truncate this string";
    s.truncate( 5 );                      // s == "trunc"
    \endcode

    \sa resize()
*/

/*!
    Extends or shrinks the string to \a len bytes, including the
    '\0'-terminator.

    A '\0'-terminator is set at position \c{len - 1} unless
    \c{len == 0}.

    Example:
    \code
    QCString s = "resize this string";
    s.resize( 7 );                      // s == "resize"
    \endcode

    \sa truncate()
*/

bool QCString::resize( uint len )
{
    detach();
    uint wasNull = isNull();
    if ( !QByteArray::resize(len) )
	return FALSE;
    if ( len )
	data()[len - 1] = '\0';
    if ( len > 0 && wasNull )
	data()[0] = '\0';
    return TRUE;
}


/*!
    Implemented as a call to the native vsprintf() (see the manual for
    your C library).

    If the string is shorter than 256 characters, this sprintf() calls
    resize(256) to decrease the chance of memory corruption. The
    string is resized back to its actual length before sprintf()
    returns.

    Example:
    \code
    QCString s;
    s.sprintf( "%d - %s", 1, "first" );		// result < 256 chars

    QCString big( 25000 );			// very long string
    big.sprintf( "%d - %s", 2, longString );	// result < 25000 chars
    \endcode

    \warning All vsprintf() implementations will write past the end of
    the target string (*this) if the \a format specification and
    arguments happen to be longer than the target string, and some
    will also fail if the target string is longer than some arbitrary
    implementation limit.

    Giving user-supplied arguments to sprintf() is risky: Sooner or
    later someone will paste a huge line into your application.
*/

QCString &QCString::sprintf( const char *format, ... )
{
    detach();
    va_list ap;
    va_start( ap, format );
    if ( size() < 256 )
	QByteArray::resize( 256 );		// make string big enough
    vsprintf( data(), format, ap );
    resize( qstrlen(data()) + 1 );		// truncate
    va_end( ap );
    return *this;
}


/*!
    Fills the string with \a len bytes of character \a c, followed by
    a '\0'-terminator.

    If \a len is negative, then the current string length is used.

    Returns FALSE is \a len is nonnegative and there is not enough
    memory to resize the string; otherwise returns TRUE.
*/

bool QCString::fill( char c, int len )
{
    detach();
    if ( len < 0 )
	len = length();
    if ( !QByteArray::fill(c,len+1) )
	return FALSE;
    *(data()+len) = '\0';
    return TRUE;
}


/*!
    \fn QCString QCString::copy() const

    Returns a deep copy of this string.

    \sa detach()
*/


/*!
    Finds the first occurrence of the character \a c, starting at
    position \a index.

    The search is case sensitive if \a cs is TRUE, or case insensitive
    if \a cs is FALSE.

    Returns the position of \a c, or -1 if \a c could not be found.

    \sa \link #asciinotion Note on character comparisons \endlink
*/

int QCString::find( char c, int index, bool cs ) const
{
    if ( (uint)index >= size() )		// index outside string
	return -1;
    register const char *d;
    if ( cs ) {					// case sensitive
	d = strchr( data()+index, c );
    } else {
	d = data()+index;
	c = tolower( (uchar) c );
	while ( *d && tolower((uchar) *d) != c )
	    d++;
	if ( !*d && c )				// not found
	    d = 0;
    }
    return d ? (int)(d - data()) : -1;
}

#define REHASH( a ) \
    if ( sl_minus_1 < sizeof(uint) * CHAR_BIT ) \
	hashHaystack -= (a) << sl_minus_1; \
    hashHaystack <<= 1

/*!
    \overload

    Finds the first occurrence of the string \a str, starting at
    position \a index.

    The search is case sensitive if \a cs is TRUE, or case insensitive
    if \a cs is FALSE.

    Returns the position of \a str, or -1 if \a str could not be
    found.

    \sa \link #asciinotion Note on character comparisons \endlink
*/

int QCString::find( const char *str, int index, bool cs ) const
{
    if ( (uint)index >= size() )
	return -1;
    if ( !str )
	return -1;
    if ( !*str )
	return index;
    const uint l = length();
    const uint sl = qstrlen( str );
    if ( sl + index > l )
	return -1;

    if ( sl == 1 )
	return find( *str, index, cs );

    /*
      See QString::find() for details.
    */
    const char* needle = str;
    const char* haystack = data() + index;
    const char* end = data() + (l-sl);
    const uint sl_minus_1 = sl-1;
    uint hashNeedle = 0, hashHaystack = 0,i;

    if ( cs ) {
	for ( i = 0; i < sl; ++i ) {
	    hashNeedle = ((hashNeedle<<1) + needle[i] );
	    hashHaystack = ((hashHaystack<<1) + haystack[i] );
	}
	hashHaystack -= *(haystack+sl_minus_1);

	while ( haystack <= end ) {
	    hashHaystack += *(haystack+sl_minus_1);
 	    if ( hashHaystack == hashNeedle  && *needle == *haystack
		 && qstrncmp( needle, haystack, sl ) == 0 )
		return haystack - data();

	    REHASH( *haystack );
	    ++haystack;
	}
    } else {
	for ( i = 0; i < sl; ++i ) {
	    hashNeedle = ((hashNeedle<<1) +
			  tolower( needle[i] ) );
	    hashHaystack = ((hashHaystack<<1) +
			    tolower( haystack[i] ) );
	}
	hashHaystack -= tolower(*(haystack+sl_minus_1));

	while ( haystack <= end ) {
	    hashHaystack += tolower(*(haystack+sl_minus_1));
	    if ( hashHaystack == hashNeedle
		 && qstrnicmp( needle, haystack, sl ) == 0 )
		return haystack - data();

	    REHASH( tolower(*haystack) );
	    ++haystack;
	}
    }
    return -1;
}


/*!
    Finds the first occurrence of the character \a c, starting at
    position \a index and searching backwards.

    The search is case sensitive if \a cs is TRUE, or case insensitive
    if \a cs is FALSE.

    Returns the position of \a c, or -1 if \a c could not be found.

    \sa \link #asciinotion Note on character comparisons \endlink
*/

int QCString::findRev( char c, int index, bool cs ) const
{
    register const char *b = data();
    register const char *d;
    if ( index < 0 )
	index = length();
    if ( (uint)index >= size() )
	return -1;
    d = b + index;
    if ( cs ) {
	while ( d >= b && *d != c )
	    d--;
    } else {
	c = tolower( (uchar) c );
	while ( d >= b && tolower((uchar) *d) != c )
	    d--;
    }
    return d >= b ? (int)(d - b) : -1;
}

/*!
    \overload

    Finds the first occurrence of the string \a str, starting at
    position \a index and searching backwards.

    The search is case sensitive if \a cs is TRUE, or case insensitive
    if \a cs is FALSE.

    Returns the position of \a str, or -1 if \a str could not be
    found.

    \sa \link #asciinotion Note on character comparisons \endlink
*/

int QCString::findRev( const char *str, int index, bool cs ) const
{
    /*
      See QString::find() for explanations.
    */
    const uint sl = qstrlen( str );
    const uint l = length();
    int delta = l-sl;
    if ( index < 0 )
	index = delta;
    if ( index < 0 || index > (int)l )
	return -1;
    if ( index > delta )
	index = delta;

    if ( sl == 1 )
	return findRev( *str, index, cs );

    const char* needle = str;
    const char* haystack = data() + index;
    const char* end = data();
    const uint sl_minus_1 = sl-1;
    const char* n = needle+sl_minus_1;
    const char* h = haystack+sl_minus_1;
    uint hashNeedle = 0, hashHaystack = 0, i;

    if ( cs ) {
	for ( i = 0; i < sl; ++i ) {
	    hashNeedle = ((hashNeedle<<1) + *(n-i) );
	    hashHaystack = ((hashHaystack<<1) + *(h-i) );
	}
	hashHaystack -= *haystack;
	while ( haystack >= end ) {
	    hashHaystack += *haystack;
 	    if ( hashHaystack == hashNeedle  && qstrncmp( needle, haystack, sl ) == 0 )
		return haystack-data();
	    --haystack;
	    REHASH( *(haystack+sl) );
	}
    } else {
	for ( i = 0; i < sl; ++i ) {
	    hashNeedle = ((hashNeedle<<1) + tolower( *(n-i) ) );
	    hashHaystack = ((hashHaystack<<1) + tolower( *(h-i) ) );
	}
	hashHaystack -= tolower(*haystack);
	while ( haystack >= end ) {
	    hashHaystack += tolower(*haystack);
	    if ( hashHaystack == hashNeedle && qstrnicmp( needle, haystack, sl ) == 0 )
		return haystack-data();
	    --haystack;
	    REHASH( tolower(*(haystack+sl)) );
	}
    }
    return -1;
}


/*!
    Returns the number of times the character \a c occurs in the
    string.

    The match is case sensitive if \a cs is TRUE, or case insensitive
    if \a cs if FALSE.

    \sa \link #asciinotion Note on character comparisons \endlink
*/

int QCString::contains( char c, bool cs ) const
{
    int count = 0;
    char *d = data();
    if ( !d )
	return 0;
    if ( cs ) {					// case sensitive
	while ( *d )
	    if ( *d++ == c )
		count++;
    } else {					// case insensitive
	c = tolower( (uchar) c );
	while ( *d ) {
	    if ( tolower((uchar) *d) == c )
		count++;
	    d++;
	}
    }
    return count;
}

/*!
    \overload

    Returns the number of times \a str occurs in the string.

    The match is case sensitive if \a cs is TRUE, or case insensitive
    if \a cs if FALSE.

    This function counts overlapping substrings, for example, "banana"
    contains two occurrences of "ana".

    \sa findRev()
	\link #asciinotion Note on character comparisons \endlink
*/

int QCString::contains( const char *str, bool cs ) const
{
    int count = 0;
    int i = -1;
    // use find for the faster hashing algorithm
    while ( ( i = find ( str, i+1, cs ) ) != -1 )
	count++;
    return count;
}

/*!
    Returns a substring that contains the \a len leftmost characters
    of the string.

    The whole string is returned if \a len exceeds the length of the
    string.

    Example:
    \code
    QCString s = "Pineapple";
    QCString t = s.left( 4 );  // t == "Pine"
    \endcode

    \sa right(), mid()
*/

QCString QCString::left( uint len ) const
{
    if ( isEmpty() ) {
	QCString empty;
	return empty;
    } else if ( len >= size() ) {
	QCString same( data() );
	return same;
    } else {
	QCString s( len+1 );
	strncpy( s.data(), data(), len );
	*(s.data()+len) = '\0';
	return s;
    }
}

/*!
    Returns a substring that contains the \a len rightmost characters
    of the string.

    The whole string is returned if \a len exceeds the length of the
    string.

    Example:
    \code
    QCString s = "Pineapple";
    QCString t = s.right( 5 );  // t == "apple"
    \endcode

    \sa left(), mid()
*/

QCString QCString::right( uint len ) const
{
    if ( isEmpty() ) {
	QCString empty;
	return empty;
    } else {
	uint l = length();
	if ( len > l )
	    len = l;
	char *p = data() + (l - len);
	return QCString( p );
    }
}

/*!
    Returns a substring that contains at most \a len characters from
    this string, starting at position \a index.

    Returns a null string if the string is empty or if \a index is out
    of range. Returns the whole string from \a index if \a index+len
    exceeds the length of the string.

    Example:
    \code
    QCString s = "Two pineapples";
    QCString t = s.mid( 4, 3 );     // t == "pin"
    \endcode

    \sa left(), right()
*/

QCString QCString::mid( uint index, uint len ) const
{
    uint slen = qstrlen( data() );
    if ( isEmpty() || index >= slen ) {
	QCString empty;
	return empty;
    } else {
	if ( len > slen-index )
	    len = slen - index;
	register char *p = data()+index;
	QCString s( len+1 );
	strncpy( s.data(), p, len );
	*(s.data()+len) = '\0';
	return s;
    }
}

/*!
    Returns a string of length \a width (plus one for the terminating
    '\0') that contains this string padded with the \a fill character.

    If the length of the string exceeds \a width and \a truncate is
    FALSE (the default), then the returned string is a copy of the
    string. If the length of the string exceeds \a width and \a
    truncate is TRUE, then the returned string is a left(\a width).

    Example:
    \code
    QCString s("apple");
    QCString t = s.leftJustify(8, '.');  // t == "apple..."
    \endcode

    \sa rightJustify()
*/

QCString QCString::leftJustify( uint width, char fill, bool truncate ) const
{
    QCString result;
    int len = qstrlen(data());
    int padlen = width - len;
    if ( padlen > 0 ) {
	result.QByteArray::resize( len+padlen+1 );
	memcpy( result.data(), data(), len );
	memset( result.data()+len, fill, padlen );
	result[len+padlen] = '\0';
    } else {
	if ( truncate )
	    result = left( width );
	else
	    result = copy();
    }
    return result;
}

/*!
    Returns a string of length \a width (plus one for the terminating
    '\0') that contains zero or more of the \a fill character followed
    by this string.

    If the length of the string exceeds \a width and \a truncate is
    FALSE (the default), then the returned string is a copy of the
    string. If the length of the string exceeds \a width and \a
    truncate is TRUE, then the returned string is a left(\a width).

    Example:
    \code
    QCString s("pie");
    QCString t = s.rightJustify(8, '.');  // t == ".....pie"
    \endcode

    \sa leftJustify()
*/

QCString QCString::rightJustify( uint width, char fill, bool truncate ) const
{
    QCString result;
    int len = qstrlen(data());
    int padlen = width - len;
    if ( padlen > 0 ) {
	result.QByteArray::resize( len+padlen+1 );
	memset( result.data(), fill, padlen );
	memcpy( result.data()+padlen, data(), len );
	result[len+padlen] = '\0';
    } else {
	if ( truncate )
	    result = left( width );
	else
	    result = copy();
    }
    return result;
}

/*!
    Returns a new string that is a copy of this string converted to lower
    case.

    Example:
    \code
    QCString s("Credit");
    QCString t = s.lower();  // t == "credit"
    \endcode

    \sa upper()
      \link #asciinotion Note on character comparisons \endlink
*/

QCString QCString::lower() const
{
    QCString s( data() );
    register char *p = s.data();
    if ( p ) {
	while ( *p ) {
	    *p = tolower( (uchar) *p );
	    p++;
	}
    }
    return s;
}

/*!
    Returns a new string that is a copy of this string converted to upper case.

    Example:
    \code
    QCString s( "Debit" );
    QCString t = s.upper();  // t == "DEBIT"
    \endcode

    \sa lower()
      \link #asciinotion Note on character comparisons \endlink
*/

QCString QCString::upper() const
{
    QCString s( data() );
    register char *p = s.data();
    if ( p ) {
	while ( *p ) {
	    *p = toupper(*p);
	    p++;
	}
    }
    return s;
}


/*!
    Returns a new string that has white space removed from the start
    and the end.

    White space means the decimal ASCII codes 9, 10, 11, 12, 13 and
    32.

    Example:
    \code
    QCString s = " space ";
    QCString t = s.stripWhiteSpace();		// t == "space"
    \endcode

    \sa simplifyWhiteSpace()
*/

QCString QCString::stripWhiteSpace() const
{
    if ( isEmpty() )				// nothing to do
	return copy();

    register char *s = data();
    QCString result = s;
    int reslen = result.length();
    if ( !isspace((uchar) s[0]) && !isspace((uchar) s[reslen-1]) )
	return result;				// returns a copy

    s = result.data();
    int start = 0;
    int end = reslen - 1;
    while ( isspace((uchar) s[start]) )		// skip white space from start
	start++;
    if ( s[start] == '\0' ) {			// only white space
	result.resize( 1 );
	return result;
    }
    while ( end && isspace((uchar) s[end]) )	// skip white space from end
	end--;
    end -= start - 1;
    memmove( result.data(), &s[start], end );
    result.resize( end + 1 );
    return result;
}


/*!
    Returns a new string that has white space removed from the start
    and the end, plus any sequence of internal white space replaced
    with a single space (ASCII 32).

    White space means the decimal ASCII codes 9, 10, 11, 12, 13 and
    32.

    \code
    QCString s = "  lots\t of\nwhite    space ";
    QCString t = s.simplifyWhiteSpace(); // t == "lots of white space"
    \endcode

    \sa stripWhiteSpace()
*/

QCString QCString::simplifyWhiteSpace() const
{
    if ( isEmpty() )				// nothing to do
	return copy();
    QCString result( size() );
    char *from	= data();
    char *to	= result.data();
    char *first = to;
    for ( ;; ) {
	while ( isspace((uchar) *from) )
	    from++;
	while ( *from && !isspace((uchar) *from) )
	    *to++ = *from++;
	if ( *from )
	    *to++ = 0x20;			// ' '
	else
	    break;
    }
    if ( to > first && *(to-1) == 0x20 )
	to--;
    *to = '\0';
    result.resize( (int)(to - result.data()) + 1 );
    return result;
}


/*!
    \overload

    Inserts string \a s into the string at position \a index.

    If \a index is beyond the end of the string, the string is
    padded with spaces (ASCII 32) to length \a index and then \a s
    is appended.

    \code
    QCString s = "I like fish";
    s.insert( 2, "don't ");     // s == "I don't like fish"

    s = "x";                    // index 01234
    s.insert( 3, "yz" );        // s == "x  yz"
    \endcode
*/

QCString &QCString::insert( uint index, const char *s )
{
    int len = qstrlen(s);
    if ( len == 0 )
	return *this;
    uint olen = length();
    int nlen = olen + len;
    if ( index >= olen ) {			// insert after end of string
	detach();
	if ( QByteArray::resize(nlen+index-olen+1) ) {
	    memset( data()+olen, ' ', index-olen );
	    memcpy( data()+index, s, len+1 );
	}
    } else if ( QByteArray::resize(nlen+1) ) {	// normal insert
	detach();
	memmove( data()+index+len, data()+index, olen-index+1 );
	memcpy( data()+index, s, len );
    }
    return *this;
}

/*!
    Inserts character \a c into the string at position \a index and
    returns a reference to the string.

    If \a index is beyond the end of the string, the string is
    padded with spaces (ASCII 32) to length \a index and then \a c
    is appended.

    Example:
    \code
    QCString s = "Yes";
    s.insert( 3, '!');   // s == "Yes!"
    \endcode

    \sa remove(), replace()
*/

QCString &QCString::insert( uint index, char c )	// insert char
{
    char buf[2];
    buf[0] = c;
    buf[1] = '\0';
    return insert( index, buf );
}

/*!
    \fn QCString &QCString::prepend( const char *s )

    Prepend \a s to the string. Equivalent to insert(0, s).

    \sa insert()
*/

/*!
    Removes \a len characters from the string, starting at position \a
    index, and returns a reference to the string.

    If \a index is out of range, nothing happens. If \a index is
    valid, but \a index + \a len is larger than the length of the
    string, the string is truncated at position \a index.

    \code
    QCString s = "Montreal";
    s.remove( 1, 4 );         // s == "Meal"
    \endcode

    \sa insert(), replace()
*/

QCString &QCString::remove( uint index, uint len )
{
    uint olen = length();
    if ( index + len >= olen ) {		// range problems
	if ( index < olen ) {			// index ok
	    detach();
	    resize( index+1 );
	}
    } else if ( len != 0 ) {
	detach();
	memmove( data()+index, data()+index+len, olen-index-len+1 );
	QByteArray::resize(olen-len+1);
    }
    return *this;
}

/*!
    Replaces \a len characters from the string, starting at position
    \a index, with \a str, and returns a reference to the string.

    If \a index is out of range, nothing is removed and \a str is
    appended at the end of the string. If \a index is valid, but \a
    index + \a len is larger than the length of the string, \a str
    replaces the rest of the string from position \a index.

    \code
    QCString s = "Say yes!";
    s.replace( 4, 3, "NO" );  // s == "Say NO!"
    \endcode

    \sa insert(), remove()
*/

QCString &QCString::replace( uint index, uint len, const char *str )
{
    remove( index, len );
    insert( index, str );
    return *this;
}


/*! \overload

    Replaces every occurrence of the character \a c in the string
    with \a after. Returns a reference to the string.

    Example:
    \code
    QCString s = "a,b,c";
    s.replace( ',', " or " );
    // s == "a or b or c"
    \endcode
*/
QCString &QCString::replace( char c, const char *after )
{
    char str[2];
    str[0] = c;
    str[1] = '\0';
    return replace( str, after );
}

/*! \overload

    Replaces every occurrence of the string \a before in the string
    with the string \a after. Returns a reference to the string.

    Example:
    \code
    QCString s = "Greek is Greek";
    s.replace( "Greek", "English" );
    // s == "English is English"
    \endcode
*/
QCString &QCString::replace( const char *before, const char *after )
{
    if ( before == after || isNull() )
	return *this;

    detach();

    int index = 0;
    const int bl = before ? strlen( before ) : 0;
    const int al = after ? strlen( after ) : 0;
    char *d = data();
    uint len = length();

    if ( bl == al ) {
	if ( bl ) {
	    while( (index = find( before, index ) ) != -1 ) {
		memcpy( d+index, after, al );
		index += bl;
	    }
	}
    } else if ( al < bl ) {
	uint to = 0;
	uint movestart = 0;
	uint num = 0;
	while( (index = find( before, index ) ) != -1 ) {
	    if ( num ) {
		int msize = index - movestart;
		if ( msize > 0 ) {
		    memmove( d + to, d + movestart, msize );
		    to += msize;
		}
	    } else {
		to = index;
	    }
	    if ( al ) {
		memcpy( d + to, after, al );
		to += al;
	    }
	    index += bl;
	    movestart = index;
	    num++;
	}
	if ( num ) {
	    int msize = len - movestart;
	    if ( msize > 0 )
		memmove( d + to, d + movestart, msize );
	    resize( len - num*(bl-al) + 1 );
	}
    } else {
	// the most complex case. We don't want to loose performance by doing repeated
	// copies and reallocs of the string.
	while( index != -1 ) {
	    uint indices[4096];
	    uint pos = 0;
	    while( pos < 4095 ) {
		index = find(before, index);
		if ( index == -1 )
		    break;
		indices[pos++] = index;
		index += bl;
		// avoid infinite loop
		if ( !bl )
		    index++;
	    }
	    if ( !pos )
		break;

	    // we have a table of replacement positions, use them for fast replacing
	    int adjust = pos*(al-bl);
	    // index has to be adjusted in case we get back into the loop above.
	    if ( index != -1 )
		index += adjust;
	    uint newlen = len + adjust;
	    int moveend = len;
	    if ( newlen > len ) {
		resize( newlen + 1 );
		len = newlen;
	    }
	    d = data();

	    while( pos ) {
		pos--;
		int movestart = indices[pos] + bl;
		int insertstart = indices[pos] + pos*(al-bl);
		int moveto = insertstart + al;
		memmove( d + moveto, d + movestart, (moveend - movestart) );
		if ( after )
		    memcpy( d + insertstart, after, al );
		moveend = movestart - bl;
	    }
	}
    }
    return *this;
}

/*! \overload

  Replaces every occurrence of \a c1 with the char \a c2.
  Returns a reference to the string.
*/
QCString &QCString::replace( char c1, char c2 )
{
    detach();
    uint i = 0;
    char *d = data();
    uint len = length();
    while ( i < len ) {
	if ( d[i] == c1 )
	    d[i] = c2;
	i++;
    }
    return *this;
}


#ifndef QT_NO_REGEXP_CAPTURE
/*!
    \overload

    Finds the first occurrence of the regular expression \a rx,
    starting at position \a index.

    Returns the position of the next match, or -1 if \a rx was not
    found.

    \warning If you want to apply this function repeatedly to the same
    string it is more efficient to convert the string to a QString and
    apply the function to that.
*/

int QCString::find( const QRegExp& rx, int index ) const
{
    QString d = QString::fromLatin1( data() );
    return d.find( rx, index );
}

/*!
    \overload

    Finds the first occurrence of the regular expression \a rx,
    starting at position \a index and searching backwards.

    Returns the position of the next match (backwards), or -1 if \a rx
    was not found.

    \warning If you want to apply this function repeatedly to the same
    string it is more efficient to convert the string to a QString and
    apply the function to that.
*/

int QCString::findRev( const QRegExp& rx, int index ) const
{
    QString d = QString::fromLatin1( data() );
    return d.findRev( rx, index );
}

/*!
    \overload

    Counts the number of overlapping occurrences of \a rx in the string.

    Example:
    \code
    QString s = "banana and panama";
    QRegExp r = QRegExp( "a[nm]a", TRUE, FALSE );
    s.contains( r ); // 4 matches
    \endcode

    \sa find(), findRev()

    \warning If you want to apply this function repeatedly to the same
    string it is more efficient to convert the string to a QString and
    apply the function to that.
*/

int QCString::contains( const QRegExp &rx ) const
{
    QString d = QString::fromLatin1( data() );
    return d.contains( rx );
}


/*!
    \overload

    Replaces every occurrence of \a rx in the string with \a str.
    Returns a reference to the string.

    Example:
    \code
    QString s = "banana";
    s.replace( QRegExp("a.*a"), "" );     // becomes "b"

    s = "banana";
    s.replace( QRegExp("^[bn]a"), "X" );  // becomes "Xnana"

    s = "banana";
    s.replace( QRegExp("^[bn]a"), "" );   // becomes "nana"
    \endcode

    \warning If you want to apply this function repeatedly to the same
    string it is more efficient to convert the string to a QString and
    apply the function to that.
*/

QCString &QCString::replace( const QRegExp &rx, const char *str )
{
    QString d = QString::fromLatin1( data() );
    QString r = QString::fromLatin1( str );
    d.replace( rx, r );
    setStr( d.ascii() );
    return *this;
}
#endif //QT_NO_REGEXP

/*!
    Returns the string converted to a \c long value.

    If \a ok is not 0: \a *ok is set to FALSE if the string is not a
    number, or if it has trailing garbage; otherwise \a *ok is set to
    TRUE.
*/

long QCString::toLong( bool *ok ) const
{
    char *p = data();
    long val=0;
    const long max_mult = 214748364;
    bool is_ok = FALSE;
    int neg = 0;
    if ( !p )
	goto bye;
    while ( isspace((uchar) *p) )		// skip leading space
	p++;
    if ( *p == '-' ) {
	p++;
	neg = 1;
    } else if ( *p == '+' ) {
	p++;
    }
    if ( !isdigit((uchar) *p) )
	goto bye;
    while ( isdigit((uchar) *p) ) {
	if ( val > max_mult || (val == max_mult && (*p-'0') > 7+neg) )
	    goto bye;
	val = 10*val + (*p++ - '0');
    }
    if ( neg )
	val = -val;
    while ( isspace((uchar) *p) )		// skip trailing space
	p++;
    if ( *p == '\0' )
	is_ok = TRUE;
bye:
    if ( ok )
	*ok = is_ok;
    return is_ok ? val : 0;
}

/*!
    Returns the string converted to an \c{unsigned long} value.

    If \a ok is not 0: \a *ok is set to FALSE if the string is not a
    number, or if it has trailing garbage; otherwise \a *ok is set to
    TRUE.
*/

ulong QCString::toULong( bool *ok ) const
{
    char *p = data();
    ulong val=0;
    const ulong max_mult = 429496729;
    bool is_ok = FALSE;
    if ( !p )
	goto bye;
    while ( isspace((uchar) *p) )		// skip leading space
	p++;
    if ( *p == '+' )
	p++;
    if ( !isdigit((uchar) *p) )
	goto bye;
    while ( isdigit((uchar) *p) ) {
	if ( val > max_mult || (val == max_mult && (*p-'0') > 5) )
	    goto bye;
	val = 10*val + (*p++ - '0');
    }
    while ( isspace((uchar) *p) )		// skip trailing space
	p++;
    if ( *p == '\0' )
	is_ok = TRUE;
bye:
    if ( ok )
	*ok = is_ok;
    return is_ok ? val : 0;
}

/*!
    Returns the string converted to a \c{short} value.

    If \a ok is not 0: \a *ok is set to FALSE if the string is not a
    number, is out of range, or if it has trailing garbage; otherwise
    \a *ok is set to TRUE.
*/

short QCString::toShort( bool *ok ) const
{
    long v = toLong( ok );
    if ( ok && *ok && (v < -32768 || v > 32767) )
	*ok = FALSE;
    return (short)v;
}

/*!
    Returns the string converted to an \c{unsigned short} value.

    If \a ok is not 0: \a *ok is set to FALSE if the string is not a
    number, is out of range, or if it has trailing garbage; otherwise
    \a *ok is set to TRUE.
*/

ushort QCString::toUShort( bool *ok ) const
{
    ulong v = toULong( ok );
    if ( ok && *ok && (v > 65535) )
	*ok = FALSE;
    return (ushort)v;
}


/*!
    Returns the string converted to a \c{int} value.

    If \a ok is not 0: \a *ok is set to FALSE if the string is not a
    number, or if it has trailing garbage; otherwise \a *ok is set to
    TRUE.
*/

int QCString::toInt( bool *ok ) const
{
    return (int)toLong( ok );
}

/*!
    Returns the string converted to an \c{unsigned int} value.

    If \a ok is not 0: \a *ok is set to FALSE if the string is not a
    number, or if it has trailing garbage; otherwise \a *ok is set to
    TRUE.
*/

uint QCString::toUInt( bool *ok ) const
{
    return (uint)toULong( ok );
}

/*!
    Returns the string converted to a \c{double} value.

    If \a ok is not 0: \a *ok is set to FALSE if the string is not a
    number, or if it has trailing garbage; otherwise \a *ok is set to
    TRUE.
*/

double QCString::toDouble( bool *ok ) const
{
    char *end;
    double val = strtod( data() ? data() : "", &end );
    if ( ok )
	*ok = ( data() && *data() && ( end == 0 || *end == '\0' ) );
    return val;
}

/*!
    Returns the string converted to a \c{float} value.

    If \a ok is not 0: \a *ok is set to FALSE if the string is not a
    number, or if it has trailing garbage; otherwise \a *ok is set to
    TRUE.
*/

float QCString::toFloat( bool *ok ) const
{
    return (float)toDouble( ok );
}


/*!
    Makes a deep copy of \a str. Returns a reference to the string.
*/

QCString &QCString::setStr( const char *str )
{
    detach();
    if ( str )					// valid string
	store( str, qstrlen(str)+1 );
    else					// empty
	resize( 0 );
    return *this;
}

/*!
    \overload

    Sets the string to the string representation of the number \a n
    and returns a reference to the string.
*/

QCString &QCString::setNum( long n )
{
    detach();
    char buf[20];
    register char *p = &buf[19];
    bool neg;
    if ( n < 0 ) {
	neg = TRUE;
	n = -n;
    } else {
	neg = FALSE;
    }
    *p = '\0';
    do {
	*--p = ((int)(n%10)) + '0';
	n /= 10;
    } while ( n );
    if ( neg )
	*--p = '-';
    store( p, qstrlen(p)+1 );
    return *this;
}

/*!
    \overload

    Sets the string to the string representation of the number \a n
    and returns a reference to the string.
*/

QCString &QCString::setNum( ulong n )
{
    detach();
    char buf[20];
    register char *p = &buf[19];
    *p = '\0';
    do {
	*--p = ((int)(n%10)) + '0';
	n /= 10;
    } while ( n );
    store( p, qstrlen(p)+1 );
    return *this;
}

/*!
    \overload QCString &QCString::setNum( int n )

    Sets the string to the string representation of the number \a n
    and returns a reference to the string.
*/

/*!
    \overload QCString &QCString::setNum( uint n )

    Sets the string to the string representation of the number \a n
    and returns a reference to the string.
*/

/*!
    \overload QCString &QCString::setNum( short n )

    Sets the string to the string representation of the number \a n
    and returns a reference to the string.
*/

/*!
    \overload QCString &QCString::setNum( ushort n )

    Sets the string to the string representation of the number \a n
    and returns a reference to the string.
*/

/*!
    Sets the string to the string representation of the number \a n
    and returns a reference to the string.

    The format of the string representation is specified by the format
    character \a f, and the precision (number of digits after the
    decimal point) is specified with \a prec.

    The valid formats for \a f are 'e', 'E', 'f', 'g' and 'G'. The
    formats are the same as for sprintf(); they are explained in \l
    QString::arg().
*/

QCString &QCString::setNum( double n, char f, int prec )
{
#if defined(QT_CHECK_RANGE)
    if ( !(f=='f' || f=='F' || f=='e' || f=='E' || f=='g' || f=='G') )
	qWarning( "QCString::setNum: Invalid format char '%c'", f );
#endif
    char format[20];
    register char *fs = format;			// generate format string
    *fs++ = '%';				//   "%.<prec>l<f>"
    if ( prec > 99 )
	prec = 99;
    *fs++ = '.';
    if ( prec >= 10 ) {
	*fs++ = prec / 10 + '0';
	*fs++ = prec % 10 + '0';
    } else {
	*fs++ = prec + '0';
    }
    *fs++ = 'l';
    *fs++ = f;
    *fs = '\0';
    return sprintf( format, n );
}

/*! \overload QCString &QCString::setNum( float n, char f, int prec ) */


/*!
    Sets the character at position \a index to \a c and expands the
    string if necessary, padding with spaces.

    Returns FALSE if \a index was out of range and the string could
    not be expanded; otherwise returns TRUE.
*/

bool QCString::setExpand( uint index, char c )
{
    detach();
    uint oldlen = length();
    if ( index >= oldlen ) {
	if ( !QByteArray::resize( index+2 ) )	// no memory
	    return FALSE;
	if ( index > oldlen )
	    memset( data() + oldlen, ' ', index - oldlen );
	*(data() + index+1) = '\0';		// terminate padded string
    }
    *(data() + index) = c;
    return TRUE;
}


/*!
    \fn QCString::operator const char *() const

    Returns the string data.
*/


/*!
    \fn QCString& QCString::append( const char *str )

    Appends string \a str to the string and returns a reference to the
    string. Equivalent to operator+=().
*/

/*!
    Appends string \a str to the string and returns a reference to the string.
*/

QCString& QCString::operator+=( const char *str )
{
    if ( !str )
	return *this;				// nothing to append
    detach();
    uint len1 = length();
    uint len2 = qstrlen(str);
    if ( !QByteArray::resize( len1 + len2 + 1 ) )
	return *this;				// no memory
    memcpy( data() + len1, str, len2 + 1 );
    return *this;
}

/*!
    \overload

    Appends character \a c to the string and returns a reference to the string.
*/

QCString &QCString::operator+=( char c )
{
    detach();
    uint len = length();
    if ( !QByteArray::resize( len + 2 ) )
	return *this;				// no memory
    *(data() + len) = c;
    *(data() + len+1) = '\0';
    return *this;
}


/*****************************************************************************
  QCString stream functions
 *****************************************************************************/
#ifndef QT_NO_DATASTREAM
/*!
    \relates QCString

    Writes string \a str to the stream \a s.

    \sa \link datastreamformat.html Format of the QDataStream operators \endlink
*/
QDataStream &operator<<( QDataStream &s, const QCString &str )
{
    return s.writeBytes( str.data(), str.size() );
}

/*!
    \relates QCString

    Reads a string into \a str from the stream \a s.

    \sa \link datastreamformat.html Format of the QDataStream operators \endlink
*/

QDataStream &operator>>( QDataStream &s, QCString &str )
{
    str.detach();
    Q_UINT32 len;
    s >> len;					// read size of string
    if ( len == 0 || s.eof() ) {		// end of file reached
	str.resize( 0 );
	return s;
    }
    if ( !str.QByteArray::resize( (uint)len )) {// resize string
#if defined(QT_CHECK_NULL)
	qWarning( "QDataStream: Not enough memory to read QCString" );
#endif
	len = 0;
    }
    if ( len > 0 )				// not null array
	s.readRawBytes( str.data(), (uint)len );
    return s;
}
#endif //QT_NO_DATASTREAM

/*****************************************************************************
  Documentation for related functions
 *****************************************************************************/

/*!
    \fn bool operator==( const QCString &s1, const QCString &s2 )

    \relates QCString

    Returns TRUE if \a s1 and \a s2 are equal; otherwise returns FALSE.

    Equivalent to qstrcmp(\a s1, \a s2) == 0.
*/

/*!
    \overload bool operator==( const QCString &s1, const char *s2 )

    \relates QCString

    Returns TRUE if \a s1 and \a s2 are equal; otherwise returns FALSE.

    Equivalent to qstrcmp(\a s1, \a s2) == 0.
*/

/*!
    \overload bool operator==( const char *s1, const QCString &s2 )

    \relates QCString

    Returns TRUE if \a s1 and \a s2 are equal; otherwise returns FALSE.

    Equivalent to qstrcmp(\a s1, \a s2) == 0.
*/

/*!
    \fn bool operator!=( const QCString &s1, const QCString &s2 )

    \relates QCString

    Returns TRUE if \a s1 and \a s2 are different; otherwise returns FALSE.

    Equivalent to qstrcmp(\a s1, \a s2) != 0.
*/

/*!
    \overload bool operator!=( const QCString &s1, const char *s2 )

    \relates QCString

    Returns TRUE if \a s1 and \a s2 are different; otherwise returns FALSE.

    Equivalent to qstrcmp(\a s1, \a s2) != 0.
*/

/*!
    \overload bool operator!=( const char *s1, const QCString &s2 )

    \relates QCString

    Returns TRUE if \a s1 and \a s2 are different; otherwise returns FALSE.

    Equivalent to qstrcmp(\a s1, \a s2) != 0.
*/

/*!
    \fn bool operator<( const QCString &s1, const char *s2 )

    \relates QCString

    Returns TRUE if \a s1 is less than \a s2; otherwise returns FALSE.

    Equivalent to qstrcmp(\a s1, \a s2) \< 0.

    \sa \link #asciinotion Note on character comparisons \endlink
*/

/*!
    \overload bool operator<( const char *s1, const QCString &s2 )

    \relates QCString

    Returns TRUE if \a s1 is less than \a s2; otherwise returns FALSE.

    Equivalent to qstrcmp(\a s1, \a s2) \< 0.

    \sa \link #asciinotion Note on character comparisons \endlink
*/

/*!
    \fn bool operator<=( const QCString &s1, const char *s2 )

    \relates QCString

    Returns TRUE if \a s1 is less than or equal to \a s2; otherwise
    returns FALSE.

    Equivalent to qstrcmp(\a s1, \a s2) \<= 0.

    \sa \link #asciinotion Note on character comparisons \endlink
*/

/*!
    \overload bool operator<=( const char *s1, const QCString &s2 )

    \relates QCString

    Returns TRUE if \a s1 is less than or equal to \a s2; otherwise
    returns FALSE.

    Equivalent to qstrcmp(\a s1, \a s2) \<= 0.

    \sa \link #asciinotion Note on character comparisons \endlink
*/

/*!
    \fn bool operator>( const QCString &s1, const char *s2 )

    \relates QCString

    Returns TRUE if \a s1 is greater than \a s2; otherwise returns FALSE.

    Equivalent to qstrcmp(\a s1, \a s2) \> 0.

    \sa \link #asciinotion Note on character comparisons \endlink
*/

/*!
    \overload bool operator>( const char *s1, const QCString &s2 )

    \relates QCString

    Returns TRUE if \a s1 is greater than \a s2; otherwise returns FALSE.

    Equivalent to qstrcmp(\a s1, \a s2) \> 0.

    \sa \link #asciinotion Note on character comparisons \endlink
*/

/*!
    \fn bool operator>=( const QCString &s1, const char *s2 )

    \relates QCString

    Returns TRUE if \a s1 is greater than or equal to \a s2; otherwise
    returns FALSE.

    Equivalent to qstrcmp(\a s1, \a s2) \>= 0.

    \sa \link #asciinotion Note on character comparisons \endlink
*/

/*!
    \overload bool operator>=( const char *s1, const QCString &s2 )

    \relates QCString

    Returns TRUE if \a s1 is greater than or equal to \a s2; otherwise
    returns FALSE.

    Equivalent to qstrcmp(\a s1, \a s2) \>= 0.

    \sa \link #asciinotion Note on character comparisons \endlink
*/

/*!
    \fn const QCString operator+( const QCString &s1, const QCString &s2 )

    \relates QCString

    Returns a string which consists of the concatenation of \a s1 and
    \a s2.
*/

/*!
    \overload const QCString operator+( const QCString &s1, const char *s2 )

    \relates QCString

    Returns a string which consists of the concatenation of \a s1 and \a s2.
*/

/*!
    \overload const QCString operator+( const char *s1, const QCString &s2 )

    \relates QCString

    Returns a string which consists of the concatenation of \a s1 and \a s2.
*/

/*!
    \overload const QCString operator+( const QCString &s, char c )

    \relates QCString

    Returns a string which consists of the concatenation of \a s and \a c.
*/

/*!
    \overload const QCString operator+( char c, const QCString &s )

    \relates QCString

    Returns a string which consists of the concatenation of \a c and \a s.
*/
