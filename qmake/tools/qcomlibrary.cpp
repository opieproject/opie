/****************************************************************************
** $Id: qcomlibrary.cpp,v 1.1 2002-11-01 00:10:44 kergoth Exp $
**
** Implementation of QComLibrary class
**
** Copyright (C) 2001-2002 Trolltech AS.  All rights reserved.
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

#include "qcomlibrary_p.h"

#ifndef QT_NO_COMPONENT
#include <qapplication.h>
#include <qsettings.h>
#include <qfileinfo.h>
#include <qdatetime.h>
#include <qcleanuphandler.h>
#include <errno.h>

#ifdef QT_THREAD_SUPPORT
#  include "qmutexpool_p.h"
#endif // QT_THREAD_SUPPORT

#ifndef QT_DEBUG_COMPONENT
# if defined(QT_DEBUG)
#  define QT_DEBUG_COMPONENT 1
# endif
#endif


QComLibrary::QComLibrary( const QString &filename )
    : QLibrary( filename ), entry( 0 ), libiface( 0 ), qt_version( 0 )
{
}

QComLibrary::~QComLibrary()
{
    if ( autoUnload() )
	unload();
}

bool QComLibrary::unload()
{
    if ( libiface ) {
	libiface->cleanup();
	if ( !libiface->canUnload() )
	    return FALSE;
	libiface->release();
	libiface = 0;
    }
    int refs = entry ? entry->release() : 0;
    if ( refs )
	return FALSE;

    entry = 0;

    return QLibrary::unload();
}

static bool qt_verify( const QString& library, uint version, uint flags,
		    const QCString &key, bool warn )
{
    uint our_flags = 1;
#if defined(QT_THREAD_SUPPORT)
    our_flags |= 2;
#endif

    if ( (flags & 1) == 0 ) {
	if ( warn )
	    qWarning( "Conflict in %s:\n"
		      "  Plugin cannot be queried successfully!",
		      (const char*) QFile::encodeName(library) );
    } else if ( ( version > QT_VERSION ) ||
		( ( QT_VERSION & 0xff0000 ) > ( version & 0xff0000 ) ) ) {
	if ( warn )
	    qWarning( "Conflict in %s:\n"
		      "  Plugin uses incompatible Qt library (%d.%d.%d)!",
		      (const char*) QFile::encodeName(library),
		      (version&0xff0000) >> 16, (version&0xff00) >> 8, version&0xff );
    } else if ( (flags & 2) != (our_flags & 2) ) {
	if ( warn )
	    qWarning( "Conflict in %s:\n"
		      "  Plugin uses %s Qt library!",
		      (const char*) QFile::encodeName(library),
		      (flags & 2) ? "multi threaded" : "single threaded" );
    } else if ( key != QT_BUILD_KEY ) {
	if ( warn )
	    qWarning( "Conflict in %s:\n"
		      "  Plugin uses incompatible Qt library!\n"
		      "  expected build key \"%s\", got \"%s\".",
		      (const char*) QFile::encodeName(library),
		      QT_BUILD_KEY,
		      key.isEmpty() ? "<null>" : (const char *) key );
    } else {
	return TRUE;
    }
    return FALSE;
}

struct qt_token_info
{
    qt_token_info( const char *f, const ulong fc )
	: fields( f ), field_count( fc ), results( fc ), lengths( fc )
    {
	results.fill( 0 );
	lengths.fill( 0 );
    }

    const char *fields;
    const ulong field_count;

    QMemArray<const char *> results;
    QMemArray<ulong> lengths;
};

/*
  return values:
       1 parse ok
       0 eos
      -1 parse error
*/
static int qt_tokenize( const char *s, ulong s_len, ulong *advance,
			const qt_token_info &token_info )
{
    ulong pos = 0, field = 0, fieldlen = 0;
    char current;
    int ret = -1;
    *advance = 0;
    for (;;) {
	current = s[ pos ];

	// next char
	++pos;
	++fieldlen;
	++*advance;

	if ( ! current || pos == s_len + 1 ) {
	    // save result
	    token_info.results[ (int)field ] = s;
	    token_info.lengths[ (int)field ] = fieldlen - 1;

	    // end of string
	    ret = 0;
	    break;
	}

	if ( current == token_info.fields[ field ] ) {
	    // save result
	    token_info.results[ (int)field ] = s;
	    token_info.lengths[ (int)field ] = fieldlen - 1;

	    // end of field
	    fieldlen = 0;
	    ++field;
	    if ( field == token_info.field_count - 1 ) {
		// parse ok
		ret = 1;
	    }
	    if ( field == token_info.field_count ) {
		// done parsing
		break;
	    }

	    // reset string and its length
	    s = s + pos;
	    s_len -= pos;
	    pos = 0;
	}
    }

    return ret;
}

/*
  returns TRUE if the string s was correctly parsed, FALSE otherwise.
*/
static bool qt_parse_pattern( const char *s, uint *version, uint *flags,
				   QCString *key )
{
    bool ret = TRUE;

    qt_token_info pinfo("=\n", 2);
    int parse;
    ulong at = 0, advance, parselen = qstrlen( s );
    do {
	parse = qt_tokenize( s + at, parselen, &advance, pinfo );
	if ( parse == -1 ) {
	    ret = FALSE;
	    break;
	}

	at += advance;
	parselen -= advance;

	if ( qstrncmp( "version", pinfo.results[ 0 ], pinfo.lengths[ 0 ] ) == 0 ) {
	    // parse version string
	    qt_token_info pinfo2("..-", 3);
	    if ( qt_tokenize( pinfo.results[ 1 ], pinfo.lengths[ 1 ],
			      &advance, pinfo2 ) != -1 ) {
		QCString m( pinfo2.results[ 0 ], pinfo2.lengths[ 0 ] + 1 );
		QCString n( pinfo2.results[ 1 ], pinfo2.lengths[ 1 ] + 1 );
		QCString p( pinfo2.results[ 2 ], pinfo2.lengths[ 2 ] + 1 );
		*version  = (m.toUInt() << 16) | (n.toUInt() << 8) | p.toUInt();
	    } else {
		ret = FALSE;
		break;
	    }
	} else if ( qstrncmp( "flags", pinfo.results[ 0 ], pinfo.lengths[ 0 ] ) == 0 ) {
	    // parse flags string
	    char ch;
	    *flags = 0;
	    ulong p = 0, c = 0, bit = 0;
	    while ( p < pinfo.lengths[ 1 ] ) {
		ch = pinfo.results[ 1 ][ p ];
		bit = pinfo.lengths[ 1 ] - p - 1;
		c = 1 << bit;
		if ( ch == '1' ) {
		    *flags |= c;
		} else if ( ch != '0' ) {
		    ret = FALSE;
		    break;
		}
		++p;
	    }
	} else if ( qstrncmp( "buildkey", pinfo.results[ 0 ],
			      pinfo.lengths[ 0 ] ) == 0 ){
	    // save buildkey
	    *key = QCString( pinfo.results[ 1 ], pinfo.lengths[ 1 ] + 1 );
	}
    } while ( parse == 1 && parselen > 0 );

    return ret;
}

#if defined(Q_OS_UNIX)

#if defined(Q_OS_FREEBSD) || defined(Q_OS_LINUX)
#  define USE_MMAP
#  include <sys/types.h>
#  include <sys/mman.h>
#endif // Q_OS_FREEBSD || Q_OS_LINUX

static long qt_find_pattern( const char *s, ulong s_len,
			     const char *pattern, ulong p_len )
{
    /*
      this uses the same algorithm as QString::findRev...

      we search from the end of the file because on the supported
      systems, the read-only data/text segments are placed at the end
      of the file.  HOWEVER, when building with debugging enabled, all
      the debug symbols are placed AFTER the data/text segments.

      what does this mean?  when building in release mode, the search
      is fast because the data we are looking for is at the end of the
      file... when building in debug mode, the search is slower
      because we have to skip over all the debugging symbols first
    */

    if ( ! s || ! pattern || p_len > s_len ) return -1;
    ulong i, hs = 0, hp = 0, delta = s_len - p_len;

    for (i = 0; i < p_len; ++i ) {
	hs += s[delta + i];
	hp += pattern[i];
    }
    i = delta;
    for (;;) {
	if ( hs == hp && qstrncmp( s + i, pattern, p_len ) == 0 )
	    return i;
	if ( i == 0 )
	    break;
	--i;
	hs -= s[i + p_len];
	hs += s[i];
    }

    return -1;
}

/*
  This opens the specified library, mmaps it into memory, and searches
  for the QT_UCM_VERIFICATION_DATA.  The advantage of this approach is that
  we can get the verification data without have to actually load the library.
  This lets us detect mismatches more safely.

  Returns FALSE if version/flags/key information is not present, or if the
                information could not be read.
  Returns  TRUE if version/flags/key information is present and succesfully read.
*/
static bool qt_unix_query( const QString &library, uint *version, uint *flags,
			   QCString *key )
{
    QFile file( library );
    if (! file.open( IO_ReadOnly ) ) {
	qWarning( "%s: %s", (const char*) QFile::encodeName(library),
	    strerror( errno ) );
	return FALSE;
    }

    QByteArray data;
    char *filedata = 0;
    ulong fdlen = 0;

#ifdef USE_MMAP
    char *mapaddr = 0;
    size_t maplen = file.size();
    mapaddr = (char *) mmap( mapaddr, maplen, PROT_READ, MAP_PRIVATE, file.handle(), 0 );
    if ( mapaddr != MAP_FAILED ) {
	// mmap succeeded
	filedata = mapaddr;
	fdlen = maplen;
    } else {
	// mmap failed
	qWarning( "mmap: %s", strerror( errno ) );
#endif // USE_MMAP
	// try reading the data into memory instead
	data = file.readAll();
	filedata = data.data();
	fdlen = data.size();
#ifdef USE_MMAP
    }
#endif // USE_MMAP

    // verify that the pattern is present in the plugin
    const char *pattern = "pattern=QT_UCM_VERIFICATION_DATA";
    const ulong plen = qstrlen( pattern );
    long pos = qt_find_pattern( filedata, fdlen, pattern, plen );

    bool ret = FALSE;
    if ( pos >= 0 ) {
	ret = qt_parse_pattern( filedata + pos, version, flags, key );
    }

#ifdef USE_MMAP
    if ( mapaddr != MAP_FAILED && munmap(mapaddr, maplen) != 0 ) {
	qWarning( "munmap: %s", strerror( errno ) );
    }
#endif // USE_MMAP

    file.close();
    return ret;
}

#endif // Q_OS_UNIX


static QSettings *cache = 0;
static QSingleCleanupHandler<QSettings> cleanup_cache;

void QComLibrary::createInstanceInternal()
{
    if ( library().isEmpty() )
	return;

    QFileInfo fileinfo( library() );
    QString lastModified  = fileinfo.lastModified().toString();
    QString regkey = QString("/Qt Plugins %1.%2/%3")
		     .arg( ( QT_VERSION & 0xff0000 ) >> 16 )
		     .arg( ( QT_VERSION & 0xff00 ) >> 8 )
		     .arg( library() );
    QStringList reg;
    uint flags = 0;
    QCString key;
    bool query_done = FALSE;
    bool warn_mismatch = TRUE;

    if ( ! query_done ) {

#ifdef QT_THREAD_SUPPORT
	QMutexLocker locker( qt_global_mutexpool->get( &cache ) );
#endif // QT_THREAD_SUPPORT

	if ( ! cache ) {
	    cache = new QSettings;
	    cache->insertSearchPath( QSettings::Windows, "/Trolltech" );
	    cleanup_cache.set( &cache );
	}

	reg = cache->readListEntry( regkey );
	if ( reg.count() == 4 ) {
	    // check timestamp
	    if ( lastModified == reg[3] ) {
		qt_version = reg[0].toUInt(0, 16);
		flags = reg[1].toUInt(0, 16);
		key = reg[2].latin1();

		query_done = TRUE;
		warn_mismatch = FALSE;
	    }
	}
    }

#if defined(Q_OS_UNIX)
    if ( ! query_done ) {
	// get the query information directly from the plugin without loading
	if ( qt_unix_query( library(), &qt_version, &flags, &key ) ) {
 	    // info read succesfully from library
 	    query_done = TRUE;
	}
    }
#else // !Q_OS_UNIX
    if ( ! query_done ) {
	// get the query information by loading the plugin
	if ( !isLoaded() ) {
	    Q_ASSERT( entry == 0 );
	    if ( !load() )
		return;
	}

#  ifdef Q_CC_BOR
	typedef const char * __stdcall (*UCMQueryVerificationDataProc)();
#  else
	typedef const char * (*UCMQueryVerificationDataProc)();
#  endif
	UCMQueryVerificationDataProc ucmQueryVerificationdataProc;
	ucmQueryVerificationdataProc =
	    (UCMQueryVerificationDataProc) resolve( "qt_ucm_query_verification_data" );

	if ( !ucmQueryVerificationdataProc ||
	     !qt_parse_pattern( ucmQueryVerificationdataProc(),
				&qt_version, &flags, &key ) ) {
	    qt_version = flags = 0;
	    key = "unknown";
	} else {
	    query_done = TRUE;
	}
    }
#endif // Q_OS_UNIX

    QStringList queried;
    queried << QString::number( qt_version,16 )
	    << QString::number( flags, 16 )
	    <<  key
	    << lastModified;

    if ( queried != reg ) {

#ifdef QT_THREAD_SUPPORT
	QMutexLocker locker( qt_global_mutexpool->get( &cache ) );
#endif // QT_THREAD_SUPPORT

	cache->writeEntry( regkey, queried );
	// delete the cache, which forces the settings to be written
	delete cache;
	cache = 0;
    }

    if ( ! query_done ) {
	if ( warn_mismatch ) {
	    qWarning( "Conflict in %s:\n Plugin cannot be queried successfully!",
		      (const char*) QFile::encodeName( library() ) );
	}
	unload();
	return;
    }

    if ( ! qt_verify( library(), qt_version, flags, key, warn_mismatch ) ) {
	unload();
	return;
    } else if ( !isLoaded() ) {
	Q_ASSERT( entry == 0 );
	if ( !load() )
	    return;
    }

#ifdef Q_CC_BOR
    typedef QUnknownInterface* __stdcall (*UCMInstanceProc)();
#else
    typedef QUnknownInterface* (*UCMInstanceProc)();
#endif
    UCMInstanceProc ucmInstanceProc;
    ucmInstanceProc = (UCMInstanceProc) resolve( "ucm_instantiate" );
#if defined(QT_DEBUG_COMPONENT)
    if ( !ucmInstanceProc )
	qWarning( "%s: Not a UCOM library.", (const char*) QFile::encodeName(library()) );
#endif
    entry = ucmInstanceProc ? ucmInstanceProc() : 0;

    if ( entry ) {
	if ( entry->queryInterface( IID_QLibrary, (QUnknownInterface**)&libiface ) == QS_OK ) {
	    if ( libiface && !libiface->init() ) {
		libiface->release();
		libiface = 0;
		unload();
		return;
	    }
	}
    } else {
#if defined(QT_DEBUG_COMPONENT)
	qWarning( "%s: No exported component provided.", (const char*) QFile::encodeName(library()) );
#endif
	unload();
    }
}

QRESULT QComLibrary::queryInterface( const QUuid& request, QUnknownInterface** iface )
{
    if ( !entry )
	createInstanceInternal();
    return entry ? entry->queryInterface( request, iface ) : QE_NOCOMPONENT;
}

uint QComLibrary::qtVersion()
{
    if ( !entry )
	createInstanceInternal();
    return entry ? qt_version : 0;
}


#endif // QT_NO_COMPONENT
