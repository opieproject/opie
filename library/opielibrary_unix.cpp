/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "opielibrary_p.h"

#ifndef QT_NO_COMPONENT

/*
  The platform dependent implementations of
  - loadLibrary
  - freeLibrary
  - resolveSymbol

  It's not too hard to guess what the functions do.
*/
#if defined(Q_OS_HPUX)
// for HP-UX < 11.x and 32 bit
#include <dl.h>

bool OpieLibraryPrivate::loadLibrary()
{
    if ( pHnd )
	return TRUE;

    QString filename = library->library();

    pHnd = (void*)shl_load( filename.latin1(), BIND_DEFERRED | BIND_NONFATAL | DYNAMIC_PATH, 0 );
#if defined(QT_DEBUG) || defined(QT_DEBUG_COMPONENT)
    if ( !pHnd )
	qDebug( "Failed to load library %s!", filename.latin1() );
#endif
    return pHnd != 0;
}

bool OpieLibraryPrivate::freeLibrary()
{
    if ( !pHnd )
	return TRUE;

    if ( !shl_unload( (shl_t)pHnd ) ) {
	pHnd = 0;
	return TRUE;
    }
    return FALSE;
}

void* OpieLibraryPrivate::resolveSymbol( const char* symbol )
{
    if ( !pHnd )
	return 0;

    void* address = 0;
    if ( shl_findsym( (shl_t*)&pHnd, symbol, TYPE_UNDEFINED, address ) < 0 ) {
#if defined(QT_DEBUG) || defined(QT_DEBUG_COMPONENT)
	qDebug( "Couldn't resolve symbol \"%s\"", symbol );
#endif
	return 0;
    }
    return address;
}

#else // Q_OS_HPUX
// Something else, assuming POSIX
#include <dlfcn.h>

bool OpieLibraryPrivate::loadLibrary()
{
    if ( pHnd )
	return TRUE;

    QString filename = library->library();

    pHnd = dlopen( filename.latin1() , RTLD_LAZY );
#if defined(QT_DEBUG) || defined(QT_DEBUG_COMPONENT)
    if ( !pHnd )
	qWarning( "%s", dlerror() );
#endif
    return pHnd != 0;
}

bool OpieLibraryPrivate::freeLibrary()
{
    if ( !pHnd )
	return TRUE;

    int ec = dlclose( pHnd );
    if ( !ec )
	pHnd = 0;
#if defined(QT_DEBUG) || defined(QT_DEBUG_COMPONENT)
    else {
	const char* error = dlerror();
	if ( error )
	    qWarning( "%s", error );
    }
#endif
    return pHnd == 0;
}

void* OpieLibraryPrivate::resolveSymbol( const char* f )
{
    if ( !pHnd )
	return 0;

    void* address = dlsym( pHnd, f );
#if defined(QT_DEBUG) || defined(QT_DEBUG_COMPONENT)
    const char* error = dlerror();
    if ( error )
	qWarning( "%s", error );
#endif
    return address;
}

#endif // POSIX

#endif // QT_NO_COMPONENT
