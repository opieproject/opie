/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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

#include "qlibrary_p.h"

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

bool QLibraryPrivate::loadLibrary()
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

bool QLibraryPrivate::freeLibrary()
{
    if ( !pHnd )
	return TRUE;

    if ( !shl_unload( (shl_t)pHnd ) ) {
	pHnd = 0;
	return TRUE;
    }
    return FALSE;
}

void* QLibraryPrivate::resolveSymbol( const char* symbol )
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

#elif defined(_NULL_LIB_)

bool QLibraryPrivate::loadLibrary()
{
	//qDebug("QLibraryPrivate::loadLibrary\n");
	return FALSE;
}
bool QLibraryPrivate::freeLibrary()
{
	//qDebug("QLibraryPrivate::freeLibrary\n");
	return FALSE;
}
void* QLibraryPrivate::resolveSymbol( const char* symbol )
{
	//qDebug("QLibraryPrivate::resolveSymbol\n");
	return FALSE;
}

#elif defined(Q_OS_MACX)

#define ENUM_DYLD_BOOL
enum DYLD_BOOL {
	DYLD_FALSE,
	DYLD_TRUE
};
#include <mach-o/dyld.h>
typedef struct {
	NSObjectFileImage img;
	NSModule mod;
} DyldLibDesc;

bool QLibraryPrivate::loadLibrary()
{
	// qDebug("QLibraryPrivate::loadLibrary\n");
	// return FALSE;
    if ( pHnd )
	return TRUE;

    QString filename = library->library();

    NSObjectFileImage img = 0;
    NSModule mod = 0;
    NSObjectFileImageReturnCode ret = NSCreateObjectFileImageFromFile( filename.latin1() , &img );
    if ( ret != NSObjectFileImageSuccess ) {
		qWarning( "Error in NSCreateObjectFileImageFromFile(): %d; Filename: %s", ret, filename.latin1() );
		if (ret == NSObjectFileImageAccess) {
			qWarning ("(NSObjectFileImageAccess)" );
		}
	} else {
		mod = NSLinkModule(img, filename.latin1(), NSLINKMODULE_OPTION_BINDNOW |
				                                    NSLINKMODULE_OPTION_PRIVATE |
													NSLINKMODULE_OPTION_RETURN_ON_ERROR);
		if (mod == 0) {
			qWarning( "Error in NSLinkModule()" );
			NSDestroyObjectFileImage(img);
		}
	}
	DyldLibDesc* desc = 0;
	if (img != 0 && mod != 0) {
		desc = new DyldLibDesc;
		desc->img = img;
		desc->mod = mod;
	}
	pHnd = desc;
    return pHnd != 0;
}

bool QLibraryPrivate::freeLibrary()
{
	//qDebug("QLibraryPrivate::freeLibrary\n");
	//return FALSE;
    if ( !pHnd )
	return TRUE;

	DyldLibDesc* desc = (DyldLibDesc*) pHnd;
	NSModule mod = desc->mod;
	NSObjectFileImage img = desc->img;
	DYLD_BOOL success = NSUnLinkModule(mod, NSUNLINKMODULE_OPTION_NONE);
	if ( success ) {
	NSDestroyObjectFileImage(img);
	delete desc;
	pHnd = 0;
	}
#if defined(QT_DEBUG) || defined(QT_DEBUG_COMPONENT)
    else {
	    qWarning( "Error in NSUnLinkModule(): %d", ret );
    }
#endif
    return pHnd == 0;
}

void* QLibraryPrivate::resolveSymbol( const char* symbol )
{
	//qDebug("QLibraryPrivate::resolveSymbol\n");
	//return FALSE;
    if ( !pHnd )
	return 0;

	DyldLibDesc* desc = (DyldLibDesc*) pHnd;
	NSSymbol sym = NSLookupSymbolInModule(desc->mod, symbol);
	void* address = 0;
	if (sym != 0) {
		address = NSAddressOfSymbol(sym);
	}
#if defined(QT_DEBUG) || defined(QT_DEBUG_COMPONENT)
    if ( address == 0 )
	qWarning( "Cannot find symbol: %s", symbol );
#endif
    return address;
}

#else
// Something else, assuming POSIX
#include <dlfcn.h>

bool QLibraryPrivate::loadLibrary()
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

bool QLibraryPrivate::freeLibrary()
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

void* QLibraryPrivate::resolveSymbol( const char* f )
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
