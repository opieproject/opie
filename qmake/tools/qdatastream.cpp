/****************************************************************************
** $Id: qdatastream.cpp,v 1.1 2002-11-01 00:10:44 kergoth Exp $
**
** Implementation of QDataStream class
**
** Created : 930831
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

#include "qdatastream.h"

#ifndef QT_NO_DATASTREAM
#include "qbuffer.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

/*!
    \class QDataStream qdatastream.h
    \reentrant
    \brief The QDataStream class provides serialization of binary data
    to a QIODevice.

    \ingroup io

    A data stream is a binary stream of encoded information which is
    100% independent of the host computer's operating system, CPU or
    byte order. For example, a data stream that is written by a PC
    under Windows can be read by a Sun SPARC running Solaris.

    You can also use a data stream to read/write \link #raw raw
    unencoded binary data\endlink. If you want a "parsing" input
    stream, see QTextStream.

    The QDataStream class implements serialization of primitive types,
    like \c char, \c short, \c int, \c char* etc. Serialization of
    more complex data is accomplished by breaking up the data into
    primitive units.

    A data stream cooperates closely with a QIODevice. A QIODevice
    represents an input/output medium one can read data from and write
    data to. The QFile class is an example of an IO device.

    Example (write binary data to a stream):
    \code
    QFile file( "file.dat" );
    file.open( IO_WriteOnly );
    QDataStream stream( &file ); // we will serialize the data into the file
    stream << "the answer is";   // serialize a string
    stream << (Q_INT32)42;       // serialize an integer
    \endcode

    Example (read binary data from a stream):
    \code
    QFile file( "file.dat" );
    file.open( IO_ReadOnly );
    QDataStream stream( &file );  // read the data serialized from the file
    QString str;
    Q_INT32 a;
    stream >> str >> a;           // extract "the answer is" and 42
    \endcode

    Each item written to the stream is written in a predefined binary
    format that varies depending on the item's type. Supported Qt
    types include QBrush, QColor, QDateTime, QFont, QPixmap, QString,
    QVariant and many others. For the complete list of all Qt types
    supporting data streaming see the \link datastreamformat.html
    Format of the QDataStream operators \endlink.

    To take one example, a \c char* string is written as a 32-bit
    integer equal to the length of the string including the NUL byte
    ('\0'), followed by all the characters of the string including the
    NUL byte. When reading a \c char* string, 4 bytes are read to
    create the 32-bit length value, then that many characters for the
    \c char* string including the NUL are read.

    The initial IODevice is usually set in the constructor, but can be
    changed with setDevice(). If you've reached the end of the data
    (or if there is no IODevice set) atEnd() will return TRUE.

    If you want the data to be compatible with an earlier version of
    Qt use setVersion().

    If you want the data to be human-readable, e.g. for debugging, you
    can set the data stream into printable data mode with
    setPrintableData(). The data is then written slower, in a bloated
    but human readable format.

    If you are producing a new binary data format, such as a file
    format for documents created by your application, you could use a
    QDataStream to write the data in a portable format. Typically, you
    would write a brief header containing a magic string and a version
    number to give yourself room for future expansion. For example:

    \code
    QFile file( "file.xxx" );
    file.open( IO_WriteOnly );
    QDataStream stream( &file );

    // Write a header with a "magic number" and a version
    stream << (Q_UINT32)0xA0B0C0D0;
    stream << (Q_INT32)123;

    // Write the data
    stream << [lots of interesting data]
    \endcode

    Then read it in with:

    \code
    QFile file( "file.xxx" );
    file.open( IO_ReadOnly );
    QDataStream stream( &file );

    // Read and check the header
    Q_UINT32 magic;
    stream >> magic;
    if ( magic != 0xA0B0C0D0 )
	return XXX_BAD_FILE_FORMAT;

    // Read the version
    Q_INT32 version;
    stream >> version;
    if ( version < 100 )
	return XXX_BAD_FILE_TOO_OLD;
    if ( version > 123 )
	return XXX_BAD_FILE_TOO_NEW;
    if ( version <= 110 )
	stream.setVersion(1);

    // Read the data
    stream >> [lots of interesting data];
    if ( version > 120 )
	stream >> [data new in XXX version 1.2];
    stream >> [other interesting data];
    \endcode

    You can select which byte order to use when serializing data. The
    default setting is big endian (MSB first). Changing it to little
    endian breaks the portability (unless the reader also changes to
    little endian). We recommend keeping this setting unless you have
    special requirements.

    \target raw
    \section1 Reading and writing raw binary data

    You may wish to read/write your own raw binary data to/from the
    data stream directly. Data may be read from the stream into a
    preallocated char* using readRawBytes(). Similarly data can be
    written to the stream using writeRawBytes(). Notice that any
    encoding/decoding of the data must be done by you.

    A similar pair of functions is readBytes() and writeBytes(). These
    differ from their \e raw counterparts as follows: readBytes()
    reads a Q_UINT32 which is taken to be the length of the data to be
    read, then that number of bytes is read into the preallocated
    char*; writeBytes() writes a Q_UINT32 containing the length of the
    data, followed by the data. Notice that any encoding/decoding of
    the data (apart from the length Q_UINT32) must be done by you.

    \sa QTextStream QVariant
*/

/*!
    \enum QDataStream::ByteOrder

    The byte order used for reading/writing the data.

    \value BigEndian the default
    \value LittleEndian
*/


/*****************************************************************************
  QDataStream member functions
 *****************************************************************************/

#if defined(QT_CHECK_STATE)
#undef  CHECK_STREAM_PRECOND
#define CHECK_STREAM_PRECOND  if ( !dev ) {				\
				qWarning( "QDataStream: No device" );	\
				return *this; }
#else
#define CHECK_STREAM_PRECOND
#endif

static int  systemWordSize = 0;
static bool systemBigEndian;

static const int DefaultStreamVersion = 5;
// 5 is default in Qt 3.1
// 4 is default in Qt 3.0
// 3 is default in Qt 2.1
// 2 is the Qt 2.0.x format
// 1 is the Qt 1.x format

/*!
    Constructs a data stream that has no IO device.

    \sa setDevice()
*/

QDataStream::QDataStream()
{
    if ( systemWordSize == 0 )			// get system features
	qSysInfo( &systemWordSize, &systemBigEndian );
    dev	      = 0;				// no device set
    owndev    = FALSE;
    byteorder = BigEndian;			// default byte order
    printable = FALSE;
    ver	      = DefaultStreamVersion;
    noswap    = systemBigEndian;
}

/*!
    Constructs a data stream that uses the IO device \a d.

    \warning If you use QSocket or QSocketDevice as the IO device \a d
    for reading data, you must make sure that enough data is available
    on the socket for the operation to successfully proceed;
    QDataStream does not have any means to handle or recover from
    short-reads.

    \sa setDevice(), device()
*/

QDataStream::QDataStream( QIODevice *d )
{
    if ( systemWordSize == 0 )			// get system features
	qSysInfo( &systemWordSize, &systemBigEndian );
    dev	      = d;				// set device
    owndev    = FALSE;
    byteorder = BigEndian;			// default byte order
    printable = FALSE;
    ver	      = DefaultStreamVersion;
    noswap    = systemBigEndian;
}

/*!
    Constructs a data stream that operates on a byte array, \a a,
    through an internal QBuffer device. The \a mode is a
    QIODevice::mode(), usually either \c IO_ReadOnly or \c
    IO_WriteOnly.

    Example:
    \code
    static char bindata[] = { 231, 1, 44, ... };
    QByteArray a;
    a.setRawData( bindata, sizeof(bindata) );	// a points to bindata
    QDataStream stream( a, IO_ReadOnly );	// open on a's data
    stream >> [something];			// read raw bindata
    a.resetRawData( bindata, sizeof(bindata) ); // finished
    \endcode

    The QByteArray::setRawData() function is not for the inexperienced.
*/

QDataStream::QDataStream( QByteArray a, int mode )
{
    if ( systemWordSize == 0 )			// get system features
	qSysInfo( &systemWordSize, &systemBigEndian );
    dev	      = new QBuffer( a );		// create device
    ((QBuffer *)dev)->open( mode );		// open device
    owndev    = TRUE;
    byteorder = BigEndian;			// default byte order
    printable = FALSE;
    ver	      = DefaultStreamVersion;
    noswap    = systemBigEndian;
}

/*!
    Destroys the data stream.

    The destructor will not affect the current IO device, unless it is
    an internal IO device processing a QByteArray passed in the \e
    constructor, in which case the internal IO device is destroyed.
*/

QDataStream::~QDataStream()
{
    if ( owndev )
	delete dev;
}


/*!
    \fn QIODevice *QDataStream::device() const

    Returns the IO device currently set.

    \sa setDevice(), unsetDevice()
*/

/*!
    void QDataStream::setDevice(QIODevice *d )

    Sets the IO device to \a d.

    \sa device(), unsetDevice()
*/

void QDataStream::setDevice(QIODevice *d )
{
    if ( owndev ) {
	delete dev;
	owndev = FALSE;
    }
    dev = d;
}

/*!
    Unsets the IO device. This is the same as calling setDevice( 0 ).

    \sa device(), setDevice()
*/

void QDataStream::unsetDevice()
{
    setDevice( 0 );
}


/*!
    \fn bool QDataStream::atEnd() const

    Returns TRUE if the IO device has reached the end position (end of
    the stream or file) or if there is no IO device set; otherwise
    returns FALSE, i.e. if the current position of the IO device is
    before the end position.

    \sa QIODevice::atEnd()
*/

/*!\fn bool QDataStream::eof() const

  \obsolete

  Returns TRUE if the IO device has reached the end position (end of
  stream or file) or if there is no IO device set.

  Returns FALSE if the current position of the read/write head of the IO
  device is somewhere before the end position.

  \sa QIODevice::atEnd()
*/

/*!
    \fn int QDataStream::byteOrder() const

    Returns the current byte order setting -- either \c BigEndian or
    \c LittleEndian.

    \sa setByteOrder()
*/

/*!
    Sets the serialization byte order to \a bo.

    The \a bo parameter can be \c QDataStream::BigEndian or \c
    QDataStream::LittleEndian.

    The default setting is big endian. We recommend leaving this
    setting unless you have special requirements.

    \sa byteOrder()
*/

void QDataStream::setByteOrder( int bo )
{
    byteorder = bo;
    if ( systemBigEndian )
	noswap = byteorder == BigEndian;
    else
	noswap = byteorder == LittleEndian;
}


/*!
    \fn bool QDataStream::isPrintableData() const

    Returns TRUE if the printable data flag has been set; otherwise
    returns FALSE.

    \sa setPrintableData()
*/

/*!
    \fn void QDataStream::setPrintableData( bool enable )

    If \a enable is TRUE, data will be output in a human readable
    format. If \a enable is FALSE, data will be output in a binary
    format.

    If \a enable is TRUE, the write functions will generate output
    that consists of printable characters (7 bit ASCII). This output
    will typically be a lot larger than the default binary output, and
    consequently slower to write.

    We recommend only enabling printable data for debugging purposes.
*/


/*!
    \fn int QDataStream::version() const

    Returns the version number of the data serialization format. In Qt
    3.1, this number is 5.

    \sa setVersion()
*/

/*!
    \fn void QDataStream::setVersion( int v )

    Sets the version number of the data serialization format to \a v.

    You don't need to set a version if you are using the current
    version of Qt.

    In order to accommodate new functionality, the datastream
    serialization format of some Qt classes has changed in some
    versions of Qt. If you want to read data that was created by an
    earlier version of Qt, or write data that can be read by a program
    that was compiled with an earlier version of Qt, use this function
    to modify the serialization format of QDataStream.

    \table
    \header \i Qt Version	    \i QDataStream Version
    \row \i Qt 3.1		    \i11 5
    \row \i Qt 3.0		    \i11 4
    \row \i Qt 2.1.x and Qt 2.2.x   \i11 3
    \row \i Qt 2.0.x		    \i11 2
    \row \i Qt 1.x		    \i11 1
    \endtable

    \sa version()
*/

/*****************************************************************************
  QDataStream read functions
 *****************************************************************************/


static Q_INT32 read_int_ascii( QDataStream *s )
{
    register int n = 0;
    char buf[40];
    for ( ;; ) {
	buf[n] = s->device()->getch();
	if ( buf[n] == '\n' || n > 38 )		// $-terminator
	    break;
	n++;
    }
    buf[n] = '\0';
    return atol( buf );
}


/*!
    \overload QDataStream &QDataStream::operator>>( Q_UINT8 &i )

    Reads an unsigned byte from the stream into \a i, and returns a
    reference to the stream.
*/

/*!
    Reads a signed byte from the stream into \a i, and returns a
    reference to the stream.
*/

QDataStream &QDataStream::operator>>( Q_INT8 &i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	i = (Q_INT8)dev->getch();
	if ( i == '\\' ) {			// read octal code
	    char buf[4];
	    dev->readBlock( buf, 3 );
	    i = (buf[2] & 0x07)+((buf[1] & 0x07) << 3)+((buf[0] & 0x07) << 6);
	}
    } else {					// data or text
	i = (Q_INT8)dev->getch();
    }
    return *this;
}


/*!
    \overload QDataStream &QDataStream::operator>>( Q_UINT16 &i )

    Reads an unsigned 16-bit integer from the stream into \a i, and
    returns a reference to the stream.
*/

/*!
    \overload

    Reads a signed 16-bit integer from the stream into \a i, and
    returns a reference to the stream.
*/

QDataStream &QDataStream::operator>>( Q_INT16 &i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	i = (Q_INT16)read_int_ascii( this );
    } else if ( noswap ) {			// no conversion needed
	dev->readBlock( (char *)&i, sizeof(Q_INT16) );
    } else {					// swap bytes
	register uchar *p = (uchar *)(&i);
	char b[2];
	dev->readBlock( b, 2 );
	*p++ = b[1];
	*p   = b[0];
    }
    return *this;
}


/*!
    \overload QDataStream &QDataStream::operator>>( Q_UINT32 &i )

    Reads an unsigned 32-bit integer from the stream into \a i, and
    returns a reference to the stream.
*/

/*!
    \overload

    Reads a signed 32-bit integer from the stream into \a i, and
    returns a reference to the stream.
*/

QDataStream &QDataStream::operator>>( Q_INT32 &i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	i = read_int_ascii( this );
    } else if ( noswap ) {			// no conversion needed
	dev->readBlock( (char *)&i, sizeof(Q_INT32) );
    } else {					// swap bytes
	uchar *p = (uchar *)(&i);
	char b[4];
	dev->readBlock( b, 4 );
	*p++ = b[3];
	*p++ = b[2];
	*p++ = b[1];
	*p   = b[0];
    }
    return *this;
}

/*!
    \overload QDataStream &QDataStream::operator>>( Q_ULONG &i )

    Reads an unsigned integer of the system's word length from the
    stream, into \a i, and returns a reference to the stream.
*/

/*!
    \overload

    Reads a signed integer of the system's word length from the stream
    into \a i, and returns a reference to the stream.
*/

QDataStream &QDataStream::operator>>( Q_LONG &i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	i = read_int_ascii( this );
    } else if ( noswap ) {			// no conversion needed
	dev->readBlock( (char *)&i, sizeof(Q_LONG) );
    } else {					// swap bytes
	register uchar *p = (uchar *)(&i);
	char b[sizeof(Q_LONG)];
	dev->readBlock( b, sizeof(Q_LONG) );
	for ( int j = sizeof(Q_LONG); j;  )
	    *p++ = b[--j];
    }
    return *this;
}

static double read_double_ascii( QDataStream *s )
{
    register int n = 0;
    char buf[80];
    for ( ;; ) {
	buf[n] = s->device()->getch();
	if ( buf[n] == '\n' || n > 78 )		// $-terminator
	    break;
	n++;
    }
    buf[n] = '\0';
    return atof( buf );
}


/*!
    \overload

    Reads a 32-bit floating point number from the stream into \a f,
    using the standard IEEE754 format. Returns a reference to the
    stream.
*/

QDataStream &QDataStream::operator>>( float &f )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	f = (float)read_double_ascii( this );
    } else if ( noswap ) {			// no conversion needed
	dev->readBlock( (char *)&f, sizeof(float) );
    } else {					// swap bytes
	uchar *p = (uchar *)(&f);
	char b[4];
	dev->readBlock( b, 4 );
	*p++ = b[3];
	*p++ = b[2];
	*p++ = b[1];
	*p   = b[0];
    }
    return *this;
}


/*!
    \overload

    Reads a 64-bit floating point number from the stream into \a f,
    using the standard IEEE754 format. Returns a reference to the
    stream.
*/

QDataStream &QDataStream::operator>>( double &f )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	f = read_double_ascii( this );
    } else if ( noswap ) {			// no conversion needed
	dev->readBlock( (char *)&f, sizeof(double) );
    } else {					// swap bytes
	register uchar *p = (uchar *)(&f);
	char b[8];
	dev->readBlock( b, 8 );
	*p++ = b[7];
	*p++ = b[6];
	*p++ = b[5];
	*p++ = b[4];
	*p++ = b[3];
	*p++ = b[2];
	*p++ = b[1];
	*p   = b[0];
    }
    return *this;
}


/*!
    \overload

    Reads the '\0'-terminated string \a s from the stream and returns
    a reference to the stream.

    Space for the string is allocated using \c new -- the caller must
    destroy it with delete[].
*/

QDataStream &QDataStream::operator>>( char *&s )
{
    uint len = 0;
    return readBytes( s, len );
}


/*!
    Reads the buffer \a s from the stream and returns a reference to
    the stream.

    The buffer \a s is allocated using \c new. Destroy it with the \c
    delete[] operator. If the length is zero or \a s cannot be
    allocated, \a s is set to 0.

    The \a l parameter will be set to the length of the buffer.

    The serialization format is a Q_UINT32 length specifier first,
    then \a l bytes of data. Note that the data is \e not encoded.

    \sa readRawBytes(), writeBytes()
*/

QDataStream &QDataStream::readBytes( char *&s, uint &l )
{
    CHECK_STREAM_PRECOND
    Q_UINT32 len;
    *this >> len;				// first read length spec
    l = (uint)len;
    if ( len == 0 || eof() ) {
	s = 0;
	return *this;
    } else {
	s = new char[len];			// create char array
	Q_CHECK_PTR( s );
	if ( !s )				// no memory
	    return *this;
	return readRawBytes( s, (uint)len );
    }
}


/*!
    Reads \a len bytes from the stream into \a s and returns a
    reference to the stream.

    The buffer \a s must be preallocated. The data is \e not encoded.

    \sa readBytes(), QIODevice::readBlock(), writeRawBytes()
*/

QDataStream &QDataStream::readRawBytes( char *s, uint len )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	register Q_INT8 *p = (Q_INT8*)s;
	while ( len-- )
	    *this >> *p++;
    } else {					// read data char array
	dev->readBlock( s, len );
    }
    return *this;
}


/*****************************************************************************
  QDataStream write functions
 *****************************************************************************/


/*!
    \overload QDataStream &QDataStream::operator<<( Q_UINT8 i )

    Writes an unsigned byte, \a i, to the stream and returns a
    reference to the stream.
*/

/*!
    Writes a signed byte, \a i, to the stream and returns a reference
    to the stream.
*/

QDataStream &QDataStream::operator<<( Q_INT8 i )
{
    CHECK_STREAM_PRECOND
    if ( printable && (i == '\\' || !isprint((uchar) i)) ) {
	char buf[6];				// write octal code
	buf[0] = '\\';
	buf[1] = '0' + ((i >> 6) & 0x07);
	buf[2] = '0' + ((i >> 3) & 0x07);
	buf[3] = '0' + (i & 0x07);
	buf[4] = '\0';
	dev->writeBlock( buf, 4 );
    } else {
	dev->putch( i );
    }
    return *this;
}


/*!
    \overload QDataStream &QDataStream::operator<<( Q_UINT16 i )

    Writes an unsigned 16-bit integer, \a i, to the stream and returns
    a reference to the stream.
*/

/*!
    \overload

    Writes a signed 16-bit integer, \a i, to the stream and returns a
    reference to the stream.
*/

QDataStream &QDataStream::operator<<( Q_INT16 i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	char buf[16];
	sprintf( buf, "%d\n", i );
	dev->writeBlock( buf, strlen(buf) );
    } else if ( noswap ) {			// no conversion needed
	dev->writeBlock( (char *)&i, sizeof(Q_INT16) );
    } else {					// swap bytes
	register uchar *p = (uchar *)(&i);
	char b[2];
	b[1] = *p++;
	b[0] = *p;
	dev->writeBlock( b, 2 );
    }
    return *this;
}

/*!
    \overload

    Writes a signed 32-bit integer, \a i, to the stream and returns a
    reference to the stream.
*/

QDataStream &QDataStream::operator<<( Q_INT32 i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	char buf[16];
	sprintf( buf, "%d\n", i );
	dev->writeBlock( buf, strlen(buf) );
    } else if ( noswap ) {			// no conversion needed
	dev->writeBlock( (char *)&i, sizeof(Q_INT32) );
    } else {					// swap bytes
	register uchar *p = (uchar *)(&i);
	char b[4];
	b[3] = *p++;
	b[2] = *p++;
	b[1] = *p++;
	b[0] = *p;
	dev->writeBlock( b, 4 );
    }
    return *this;
}

/*!
    \overload QDataStream &QDataStream::operator<<( Q_ULONG i )

    Writes an unsigned integer \a i, of the system's word length, to
    the stream and returns a reference to the stream.
*/

/*!
    \overload

    Writes a signed integer \a i, of the system's word length, to the
    stream and returns a reference to the stream.
*/

QDataStream &QDataStream::operator<<( Q_LONG i )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	char buf[20];
	sprintf( buf, "%ld\n", i );
	dev->writeBlock( buf, strlen(buf) );
    } else if ( noswap ) {			// no conversion needed
	dev->writeBlock( (char *)&i, sizeof(Q_LONG) );
    } else {					// swap bytes
	register uchar *p = (uchar *)(&i);
	char b[sizeof(Q_LONG)];
	for ( int j = sizeof(Q_LONG); j;  )
	    b[--j] = *p++;
	dev->writeBlock( b, sizeof(Q_LONG) );
    }
    return *this;
}

/*!
    \overload QDataStream &QDataStream::operator<<( Q_UINT32 i )

    Writes an unsigned integer, \a i, to the stream as a 32-bit
    unsigned integer (Q_UINT32). Returns a reference to the stream.
*/

/*!
    \overload

    Writes a 32-bit floating point number, \a f, to the stream using
    the standard IEEE754 format. Returns a reference to the stream.
*/

QDataStream &QDataStream::operator<<( float f )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	char buf[32];
	sprintf( buf, "%g\n", (double)f );
	dev->writeBlock( buf, strlen(buf) );
    } else {
	float g = f;				// fixes float-on-stack problem
	if ( noswap ) {				// no conversion needed
	    dev->writeBlock( (char *)&g, sizeof(float) );
	} else {				// swap bytes
	    register uchar *p = (uchar *)(&g);
	    char b[4];
	    b[3] = *p++;
	    b[2] = *p++;
	    b[1] = *p++;
	    b[0] = *p;
	    dev->writeBlock( b, 4 );
	}
    }
    return *this;
}


/*!
    \overload

    Writes a 64-bit floating point number, \a f, to the stream using
    the standard IEEE754 format. Returns a reference to the stream.
*/

QDataStream &QDataStream::operator<<( double f )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// printable data
	char buf[32];
	sprintf( buf, "%g\n", f );
	dev->writeBlock( buf, strlen(buf) );
    } else if ( noswap ) {			// no conversion needed
	dev->writeBlock( (char *)&f, sizeof(double) );
    } else {					// swap bytes
	register uchar *p = (uchar *)(&f);
	char b[8];
	b[7] = *p++;
	b[6] = *p++;
	b[5] = *p++;
	b[4] = *p++;
	b[3] = *p++;
	b[2] = *p++;
	b[1] = *p++;
	b[0] = *p;
	dev->writeBlock( b, 8 );
    }
    return *this;
}


/*!
    \overload

    Writes the '\0'-terminated string \a s to the stream and returns a
    reference to the stream.

    The string is serialized using writeBytes().
*/

QDataStream &QDataStream::operator<<( const char *s )
{
    if ( !s ) {
	*this << (Q_UINT32)0;
	return *this;
    }
    uint len = qstrlen( s ) + 1;			// also write null terminator
    *this << (Q_UINT32)len;			// write length specifier
    return writeRawBytes( s, len );
}


/*!
    Writes the length specifier \a len and the buffer \a s to the
    stream and returns a reference to the stream.

    The \a len is serialized as a Q_UINT32, followed by \a len bytes
    from \a s. Note that the data is \e not encoded.

    \sa writeRawBytes(), readBytes()
*/

QDataStream &QDataStream::writeBytes(const char *s, uint len)
{
    CHECK_STREAM_PRECOND
    *this << (Q_UINT32)len;			// write length specifier
    if ( len )
	writeRawBytes( s, len );
    return *this;
}


/*!
    Writes \a len bytes from \a s to the stream and returns a
    reference to the stream. The data is \e not encoded.

    \sa writeBytes(), QIODevice::writeBlock(), readRawBytes()
*/

QDataStream &QDataStream::writeRawBytes( const char *s, uint len )
{
    CHECK_STREAM_PRECOND
    if ( printable ) {				// write printable
	register Q_INT8 *p = (Q_INT8*)s;
	while ( len-- )
	    *this << *p++;
    } else {					// write data char array
	dev->writeBlock( s, len );
    }
    return *this;
}

#endif // QT_NO_DATASTREAM
