/****************************************************************************
** $Id: qfile_unix.cpp,v 1.1 2002-11-01 00:10:44 kergoth Exp $
**
** Implementation of QFile class
**
** Created : 950628
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
** licenses for Unix/X11 or for Qt/Embedded may use this file in accordance
** with the Qt Commercial License Agreement provided with the Software.
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

#include "qplatformdefs.h"

// POSIX Large File Support redefines open -> open64
static inline int qt_open(const char *pathname, int flags, mode_t mode)
{ return ::open(pathname, flags, mode); }
#if defined(open)
# undef open
#endif

// POSIX Large File Support redefines truncate -> truncate64
#if defined(truncate)
# undef truncate
#endif

#include "qfile.h"
#include <errno.h>
#include <limits.h>


bool qt_file_access( const QString& fn, int t )
{
    if ( fn.isEmpty() )
	return FALSE;
    return ::access( QFile::encodeName(fn), t ) == 0;
}

/*!
    \overload
    Removes the file \a fileName.
  Returns TRUE if successful, otherwise FALSE.
*/

bool QFile::remove( const QString &fileName )
{
    if ( fileName.isEmpty() ) {
#if defined(QT_CHECK_NULL)
	qWarning( "QFile::remove: Empty or null file name" );
#endif
	return FALSE;
    }
    return unlink( QFile::encodeName(fileName) ) == 0;
}

#if defined(O_NONBLOCK)
# define HAS_ASYNC_FILEMODE
# define OPEN_ASYNC O_NONBLOCK
#elif defined(O_NDELAY)
# define HAS_ASYNC_FILEMODE
# define OPEN_ASYNC O_NDELAY
#endif

/*!
    Opens the file specified by the file name currently set, using the
    mode \a m. Returns TRUE if successful, otherwise FALSE.

    \keyword IO_Raw
    \keyword IO_ReadOnly
    \keyword IO_WriteOnly
    \keyword IO_ReadWrite
    \keyword IO_Append
    \keyword IO_Truncate
    \keyword IO_Translate

    The mode parameter \a m must be a combination of the following flags:
    \table
    \header \i Flag \i Meaning
    \row \i IO_Raw
	 \i Raw (non-buffered) file access.
    \row \i IO_ReadOnly
	 \i Opens the file in read-only mode.
    \row \i IO_WriteOnly
	 \i Opens the file in write-only mode. If this flag is used
	    with another flag, e.g. \c IO_ReadOnly or \c IO_Raw or \c
	    IO_Append, the file is \e not truncated; but if used on
	    its own (or with \c IO_Truncate), the file is truncated.
    \row \i IO_ReadWrite
	 \i Opens the file in read/write mode, equivalent to \c
	    (IO_ReadOnly | IO_WriteOnly).
    \row \i IO_Append
	 \i Opens the file in append mode. (You must actually use \c
	    (IO_WriteOnly | IO_Append) to make the file writable and
	    to go into append mode.) This mode is very useful when you
	    want to write something to a log file. The file index is
	    set to the end of the file. Note that the result is
	    undefined if you position the file index manually using
	    at() in append mode.
    \row \i IO_Truncate
	 \i Truncates the file.
    \row \i IO_Translate
	 \i Enables carriage returns and linefeed translation for text
	    files under Windows.
    \endtable

    The raw access mode is best when I/O is block-operated using a 4KB
    block size or greater. Buffered access works better when reading
    small portions of data at a time.

    \warning When working with buffered files, data may not be written
    to the file at once. Call flush() to make sure that the data is
    really written.

    \warning If you have a buffered file opened for both reading and
    writing you must not perform an input operation immediately after
    an output operation or vice versa. You should always call flush()
    or a file positioning operation, e.g. at(), between input and
    output operations, otherwise the buffer may contain garbage.

    If the file does not exist and \c IO_WriteOnly or \c IO_ReadWrite
    is specified, it is created.

    Example:
    \code
	QFile f1( "/tmp/data.bin" );
	f1.open( IO_Raw | IO_ReadWrite );

	QFile f2( "readme.txt" );
	f2.open( IO_ReadOnly | IO_Translate );

	QFile f3( "audit.log" );
	f3.open( IO_WriteOnly | IO_Append );
    \endcode

    \sa name(), close(), isOpen(), flush()
*/

bool QFile::open( int m )
{
    if ( isOpen() ) {				// file already open
#if defined(QT_CHECK_STATE)
	qWarning( "QFile::open: File already open" );
#endif
	return FALSE;
    }
    if ( fn.isNull() ) {			// no file name defined
#if defined(QT_CHECK_NULL)
	qWarning( "QFile::open: No file name specified" );
#endif
	return FALSE;
    }
    init();					// reset params
    setMode( m );
    if ( !(isReadable() || isWritable()) ) {
#if defined(QT_CHECK_RANGE)
	qWarning( "QFile::open: File access not specified" );
#endif
	return FALSE;
    }
    bool ok = TRUE;
    struct stat st;
    if ( isRaw() ) {
	int oflags = O_RDONLY;
	if ( isReadable() && isWritable() )
	    oflags = O_RDWR;
	else if ( isWritable() )
	    oflags = O_WRONLY;
	if ( flags() & IO_Append ) {		// append to end of file?
	    if ( flags() & IO_Truncate )
		oflags |= (O_CREAT | O_TRUNC);
	    else
		oflags |= (O_APPEND | O_CREAT);
	    setFlags( flags() | IO_WriteOnly ); // append implies write
	} else if ( isWritable() ) {		// create/trunc if writable
	    if ( flags() & IO_Truncate )
		oflags |= (O_CREAT | O_TRUNC);
	    else
		oflags |= O_CREAT;
	}
#if defined(HAS_TEXT_FILEMODE)
	if ( isTranslated() )
	    oflags |= OPEN_TEXT;
	else
	    oflags |= OPEN_BINARY;
#endif
#if defined(HAS_ASYNC_FILEMODE)
	if ( isAsynchronous() )
	    oflags |= OPEN_ASYNC;
#endif
	fd = qt_open( QFile::encodeName(fn), oflags, 0666 );

	if ( fd != -1 ) {			// open successful
	    ::fstat( fd, &st );			// get the stat for later usage
	} else {
	    ok = FALSE;
	}
    } else {					// buffered file I/O
	QCString perm;
	char perm2[4];
	bool try_create = FALSE;
	if ( flags() & IO_Append ) {		// append to end of file?
	    setFlags( flags() | IO_WriteOnly ); // append implies write
	    perm = isReadable() ? "a+" : "a";
	} else {
	    if ( isReadWrite() ) {
		if ( flags() & IO_Truncate ) {
		    perm = "w+";
		} else {
		    perm = "r+";
		    try_create = TRUE;		// try to create if not exists
		}
	    } else if ( isReadable() ) {
		perm = "r";
	    } else if ( isWritable() ) {
		perm = "w";
	    }
	}
	qstrcpy( perm2, perm );
#if defined(HAS_TEXT_FILEMODE)
	if ( isTranslated() )
	    strcat( perm2, "t" );
	else
	    strcat( perm2, "b" );
#endif
	for (;;) { // At most twice

	    fh = fopen( QFile::encodeName(fn), perm2 );

	    if ( !fh && try_create ) {
		perm2[0] = 'w';			// try "w+" instead of "r+"
		try_create = FALSE;
	    } else {
		break;
	    }
	}
	if ( fh ) {
	    ::fstat( fileno(fh), &st ); 	// get the stat for later usage
	} else {
	    ok = FALSE;
	}
    }
    if ( ok ) {
	setState( IO_Open );
	// on successful open the file stat was got; now test what type
	// of file we have
	if ( (st.st_mode & S_IFMT) != S_IFREG ) {
	    // non-seekable
	    setType( IO_Sequential );
	    length = INT_MAX;
	    ioIndex = 0;
	} else {
	    length = (Offset)st.st_size;
	    ioIndex = (flags() & IO_Append) == 0 ? 0 : length;
	    if ( !(flags()&IO_Truncate) && length == 0 && isReadable() ) {
		// try if you can read from it (if you can, it's a sequential
		// device; e.g. a file in the /proc filesystem)
		int c = getch();
		if ( c != -1 ) {
		    ungetch(c);
		    setType( IO_Sequential );
		    length = INT_MAX;
		    ioIndex = 0;
		}
	    }
	}
    } else {
	init();
	if ( errno == EMFILE )			// no more file handles/descrs
	    setStatus( IO_ResourceError );
	else
	    setStatus( IO_OpenError );
    }
    return ok;
}

/*!
    \overload
  Opens a file in the mode \a m using an existing file handle \a f.
  Returns TRUE if successful, otherwise FALSE.

  Example:
  \code
    #include <stdio.h>

    void printError( const char* msg )
    {
	QFile f;
	f.open( IO_WriteOnly, stderr );
	f.writeBlock( msg, qstrlen(msg) );	// write to stderr
	f.close();
    }
  \endcode

  When a QFile is opened using this function, close() does not actually
  close the file, only flushes it.

  \warning If \a f is \c stdin, \c stdout, \c stderr, you may not
  be able to seek.  See QIODevice::isSequentialAccess() for more
  information.

  \sa close()
*/

bool QFile::open( int m, FILE *f )
{
    if ( isOpen() ) {
#if defined(QT_CHECK_RANGE)
	qWarning( "QFile::open: File already open" );
#endif
	return FALSE;
    }
    init();
    setMode( m &~IO_Raw );
    setState( IO_Open );
    fh = f;
    ext_f = TRUE;
    struct stat st;
    ::fstat( fileno(fh), &st );
#if defined(QT_LARGEFILE_SUPPORT)
    ioIndex = (Offset)ftello( fh );
#else
    ioIndex = (Offset)ftell( fh );
#endif
    if ( (st.st_mode & S_IFMT) != S_IFREG || f == stdin ) { //stdin is non seekable
	// non-seekable
	setType( IO_Sequential );
	length = INT_MAX;
	ioIndex = 0;
    } else {
	length = (Offset)st.st_size;
	if ( !(flags()&IO_Truncate) && length == 0 && isReadable() ) {
	    // try if you can read from it (if you can, it's a sequential
	    // device; e.g. a file in the /proc filesystem)
	    int c = getch();
	    if ( c != -1 ) {
		ungetch(c);
		setType( IO_Sequential );
		length = INT_MAX;
		ioIndex = 0;
	    }
	}
    }
    return TRUE;
}

/*!
    \overload
  Opens a file in the mode \a m using an existing file descriptor \a f.
  Returns TRUE if successful, otherwise FALSE.

  When a QFile is opened using this function, close() does not actually
  close the file.

  The QFile that is opened using this function, is automatically set to be in
  raw mode; this means that the file input/output functions are slow. If you
  run into performance issues, you should try to use one of the other open
  functions.

  \warning If \a f is one of 0 (stdin), 1 (stdout) or 2 (stderr), you may not
  be able to seek. size() is set to \c INT_MAX (in limits.h).

  \sa close()
*/


bool QFile::open( int m, int f )
{
    if ( isOpen() ) {
#if defined(QT_CHECK_RANGE)
	qWarning( "QFile::open: File already open" );
#endif
	return FALSE;
    }
    init();
    setMode( m |IO_Raw );
    setState( IO_Open );
    fd = f;
    ext_f = TRUE;
    struct stat st;
    ::fstat( fd, &st );
    ioIndex = (Offset)::lseek(fd, 0, SEEK_CUR);
    if ( (st.st_mode & S_IFMT) != S_IFREG || f == 0 ) { // stdin is not seekable...
	// non-seekable
	setType( IO_Sequential );
	length = INT_MAX;
	ioIndex = 0;
    } else {
	length = (Offset)st.st_size;
	if ( length == 0 && isReadable() ) {
	    // try if you can read from it (if you can, it's a sequential
	    // device; e.g. a file in the /proc filesystem)
	    int c = getch();
	    if ( c != -1 ) {
		ungetch(c);
		setType( IO_Sequential );
		length = INT_MAX;
		ioIndex = 0;
	    }
	    resetStatus();
	}
    }
    return TRUE;
}

/*!
  Returns the file size.
  \sa at()
*/

QIODevice::Offset QFile::size() const
{
    struct stat st;
    if ( isOpen() ) {
	::fstat( fh ? fileno(fh) : fd, &st );
    } else {
	::stat( QFile::encodeName(fn), &st );
    }
#if defined(QT_LARGEFILE_SUPPORT) && !defined(QT_ABI_64BITOFFSET)
    return (uint)st.st_size > UINT_MAX ? UINT_MAX : (QIODevice::Offset)st.st_size;
#else
    return st.st_size;
#endif
}


/*!
    \overload

    Sets the file index to \a pos. Returns TRUE if successful;
    otherwise returns FALSE.

    Example:
    \code
    QFile f( "data.bin" );
    f.open( IO_ReadOnly );  // index set to 0
    f.at( 100 );	    // set index to 100
    f.at( f.at()+50 );	    // set index to 150
    f.at( f.size()-80 );    // set index to 80 before EOF
    f.close();
    \endcode

    Use \c at() without arguments to retrieve the file offset.

    \warning The result is undefined if the file was open()'ed using
    the \c IO_Append specifier.

    \sa size(), open()
*/

bool QFile::at( Offset pos )
{
    if ( !isOpen() ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QFile::at: File is not open" );
#endif
	return FALSE;
    }
    if ( isSequentialAccess() )
	return FALSE;
    bool ok;
    if ( isRaw() ) {
	off_t l = ::lseek( fd, pos, SEEK_SET );
	ok = ( l != -1 );
	pos = (Offset)l;
    } else {					// buffered file
#if defined(QT_LARGEFILE_SUPPORT)
	ok = ( ::fseeko(fh, pos, SEEK_SET) == 0 );
#else
	ok = ( ::fseek(fh, pos, SEEK_SET) == 0 );
#endif
    }
    if ( ok )
	ioIndex = pos;
#if defined(QT_CHECK_RANGE)
    else
#if defined(QT_LARGEFILE_SUPPORT) && defined(QT_ABI_64BITOFFSET)
	qWarning( "QFile::at: Cannot set file position %llu", pos );
#else
	qWarning( "QFile::at: Cannot set file position %lu", pos );
#endif
#endif
    return ok;
}

/*!
  \reimp

  \warning We have experienced problems with some C libraries when a buffered
  file is opened for both reading and writing. If a read operation takes place
  immediately after a write operation, the read buffer contains garbage data.
  Worse, the same garbage is written to the file. Calling flush() before
  readBlock() solved this problem.
*/

Q_LONG QFile::readBlock( char *p, Q_ULONG len )
{
#if defined(QT_CHECK_NULL)
    if ( !p )
	qWarning( "QFile::readBlock: Null pointer error" );
#endif
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {
	qWarning( "QFile::readBlock: File not open" );
	return -1;
    }
    if ( !isReadable() ) {
	qWarning( "QFile::readBlock: Read operation not permitted" );
	return -1;
    }
#endif
    Q_ULONG nread = 0;					// number of bytes read
    if ( !ungetchBuffer.isEmpty() ) {
	// need to add these to the returned string.
	uint l = ungetchBuffer.length();
	while( nread < l ) {
	    *p = ungetchBuffer[ l - nread - 1 ];
	    p++;
	    nread++;
	}
	ungetchBuffer.truncate( l - nread );
    }

    if ( nread < len ) {
	if ( isRaw() ) {				// raw file
	    nread += ::read( fd, p, len-nread );
	    if ( len && nread <= 0 ) {
		nread = 0;
		setStatus(IO_ReadError);
	    }
	} else {					// buffered file
	    nread += fread( p, 1, len-nread, fh );
	    if ( (uint)nread != len ) {
		if ( ferror( fh ) || nread==0 )
		    setStatus(IO_ReadError);
	    }
	}
    }
    if ( !isSequentialAccess() )
	ioIndex += nread;
    return nread;
}


/*! \reimp

  Writes \a len bytes from \a p to the file and returns the number of
  bytes actually written.

  Returns -1 if a serious error occurred.

  \warning When working with buffered files, data may not be written
  to the file at once. Call flush() to make sure the data is really
  written.

  \sa readBlock()
*/

Q_LONG QFile::writeBlock( const char *p, Q_ULONG len )
{
#if defined(QT_CHECK_NULL)
    if ( p == 0 && len != 0 )
	qWarning( "QFile::writeBlock: Null pointer error" );
#endif
#if defined(QT_CHECK_STATE)
    if ( !isOpen() ) {				// file not open
	qWarning( "QFile::writeBlock: File not open" );
	return -1;
    }
    if ( !isWritable() ) {			// writing not permitted
	qWarning( "QFile::writeBlock: Write operation not permitted" );
	return -1;
    }
#endif
    Q_ULONG nwritten;				// number of bytes written
    if ( isRaw() )				// raw file
	nwritten = ::write( fd, (void *)p, len );
    else					// buffered file
	nwritten = fwrite( p, 1, len, fh );
    if ( nwritten != len ) {		// write error
	if ( errno == ENOSPC )			// disk is full
	    setStatus( IO_ResourceError );
	else
	    setStatus( IO_WriteError );
	if ( !isSequentialAccess() ) {
	    if ( isRaw() )			// recalc file position
		ioIndex = (Offset)::lseek( fd, 0, SEEK_CUR );
	    else
#if defined(QT_LARGEFILE_SUPPORT)
		ioIndex = (Offset)::fseeko( fh, 0, SEEK_CUR );
#else
		ioIndex = (Offset)::fseek( fh, 0, SEEK_CUR );
#endif
	}
    } else {
	if ( !isSequentialAccess() )
	    ioIndex += nwritten;
    }
    if ( ioIndex > length )			// update file length
	length = ioIndex;
    return nwritten;
}

/*!
  Returns the file handle of the file.

  This is a small positive integer, suitable for use with C library
  functions such as fdopen() and fcntl(), as well as with QSocketNotifier.

  If the file is not open or there is an error, handle() returns -1.

  \sa QSocketNotifier
*/

int QFile::handle() const
{
    if ( !isOpen() )
	return -1;
    else if ( fh )
	return fileno( fh );
    else
	return fd;
}

/*!
  Closes an open file.

  The file is not closed if it was opened with an existing file handle.
  If the existing file handle is a \c FILE*, the file is flushed.
  If the existing file handle is an \c int file descriptor, nothing
  is done to the file.

  Some "write-behind" filesystems may report an unspecified error on
  closing the file. These errors only indicate that something may
  have gone wrong since the previous open(). In such a case status()
  reports IO_UnspecifiedError after close(), otherwise IO_Ok.

  \sa open(), flush()
*/


void QFile::close()
{
    bool ok = FALSE;
    if ( isOpen() ) {				// file is not open
	if ( fh ) {				// buffered file
	    if ( ext_f )
		ok = fflush( fh ) != -1;	// flush instead of closing
	    else
		ok = fclose( fh ) != -1;
	} else {				// raw file
	    if ( ext_f )
		ok = TRUE;			// cannot close
	    else
		ok = ::close( fd ) != -1;
	}
	init();					// restore internal state
    }
    if (!ok)
	setStatus( IO_UnspecifiedError );

    return;
}
