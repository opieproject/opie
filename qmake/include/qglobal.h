/****************************************************************************
** $Id: qglobal.h,v 1.1 2002-11-01 00:10:43 kergoth Exp $
**
** Global type declarations and definitions
**
** Created : 920529
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

#ifndef QGLOBAL_H
#define QGLOBAL_H

#define QT_VERSION_STR   "3.1.0-b2"
/*
   QT_VERSION is (major << 16) + (minor << 8) + patch.
 */
#define QT_VERSION 0x030100

/*
   The operating system, must be one of: (Q_OS_x)

     MACX	- Mac OS X
     MAC9	- Mac OS 9
     MSDOS	- MS-DOS and Windows
     OS2	- OS/2
     OS2EMX	- XFree86 on OS/2 (not PM)
     WIN32	- Win32 (Windows 95/98/ME and Windows NT/2000/XP)
     CYGWIN	- Cygwin
     SOLARIS	- Sun Solaris
     HPUX	- HP-UX
     ULTRIX	- DEC Ultrix
     LINUX	- Linux
     FREEBSD	- FreeBSD
     NETBSD	- NetBSD
     OPENBSD	- OpenBSD
     BSDI	- BSD/OS
     IRIX	- SGI Irix
     OSF	- HP Tru64 UNIX
     SCO	- SCO OpenServer 5
     UNIXWARE	- UnixWare 7, Open UNIX 8
     AIX	- AIX
     HURD	- GNU Hurd
     DGUX	- DG/UX
     RELIANT	- Reliant UNIX
     DYNIX	- DYNIX/ptx
     QNX	- QNX
     QNX6	- QNX RTP 6.1
     LYNX	- LynxOS
     BSD4	- Any BSD 4.4 system
     UNIX	- Any UNIX BSD/SYSV system
*/

#if defined(__APPLE__) && defined(__GNUC__)
#  define Q_OS_MACX
#elif defined(__MACOSX__)
#  define Q_OS_MACX
#elif defined(macintosh)
#  define Q_OS_MAC9
#elif defined(__CYGWIN__)
#  define Q_OS_CYGWIN
#elif defined(MSDOS) || defined(_MSDOS)
#  define Q_OS_MSDOS
#elif defined(__OS2__)
#  if defined(__EMX__)
#    define Q_OS_OS2EMX
#  else
#    define Q_OS_OS2
#  endif
#elif !defined(SAG_COM) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define Q_OS_WIN32
#  define Q_OS_WIN64
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  define Q_OS_WIN32
#elif defined(__MWERKS__) && defined(__INTEL__)
#  define Q_OS_WIN32
#elif defined(__sun) || defined(sun)
#  define Q_OS_SOLARIS
#elif defined(hpux) || defined(__hpux)
#  define Q_OS_HPUX
#elif defined(__ultrix) || defined(ultrix)
#  define Q_OS_ULTRIX
#elif defined(sinix)
#  define Q_OS_RELIANT
#elif defined(__linux__) || defined(__linux)
#  define Q_OS_LINUX
#elif defined(__FreeBSD__)
#  define Q_OS_FREEBSD
#  define Q_OS_BSD4
#elif defined(__NetBSD__)
#  define Q_OS_NETBSD
#  define Q_OS_BSD4
#elif defined(__OpenBSD__)
#  define Q_OS_OPENBSD
#  define Q_OS_BSD4
#elif defined(__bsdi__)
#  define Q_OS_BSDI
#  define Q_OS_BSD4
#elif defined(__sgi)
#  define Q_OS_IRIX
#elif defined(__osf__)
#  define Q_OS_OSF
#elif defined(_AIX)
#  define Q_OS_AIX
#elif defined(__Lynx__)
#  define Q_OS_LYNX
#elif defined(__GNU_HURD__)
#  define Q_OS_HURD
#elif defined(__DGUX__)
#  define Q_OS_DGUX
#elif defined(__QNXNTO__)
#  define Q_OS_QNX6
#elif defined(__QNX__)
#  define Q_OS_QNX
#elif defined(_SEQUENT_)
#  define Q_OS_DYNIX
#elif defined(_SCO_DS)      /* SCO OpenServer 5 */
#  define Q_OS_SCO
#elif defined(__UNIXWARE__) /* UnixWare 7 + GCC, Open UNIX 8 + GCC */
#  define Q_OS_UNIXWARE
#  define Q_OS_UNIXWARE7
#elif defined(__USLC__)     /* UnixWare 7 + UDK, Open UNIX 8 + OUDK */
#  define Q_OS_UNIXWARE
#  define Q_OS_UNIXWARE7
#else
#  error "Qt has not been ported to this OS - talk to qt-bugs@trolltech.com"
#endif

#if defined(Q_OS_MAC9) || defined(Q_OS_MACX)
#  define Q_OS_MAC
#endif

#if defined(Q_OS_MAC9) || defined(Q_OS_MSDOS) || defined(Q_OS_OS2) || defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#  undef Q_OS_UNIX
#elif !defined(Q_OS_UNIX)
#  define Q_OS_UNIX
#endif


/*
   The compiler, must be one of: (Q_CC_x)

     SYM	- Symantec C++ for both PC and Macintosh
     MPW	- MPW C++
     MWERKS	- Metrowerks CodeWarrior
     MSVC	- Microsoft Visual C/C++
     BOR	- Borland/Turbo C++
     WAT	- Watcom C++
     GNU	- GNU C++
     COMEAU	- Comeau C++
     EDG	- Edison Design Group C++
     OC		- CenterLine C++
     SUN	- Sun WorkShop, Forte Developer, or Sun ONE Studio C++
     MIPS	- MIPSpro C++
     DEC	- DEC C++
     HP		- HPUX C++
     HPACC	- HPUX ANSI C++
     USLC	- SCO OUDK, UDK, and UnixWare 2.X C++
     CDS	- Reliant C++
     KAI	- KAI C++
     INTEL	- Intel C++
     HIGHC	- MetaWare High C/C++
     PGI	- Portland Group C++
     GHS	- Green Hills Optimizing C++ Compilers

   Should be sorted most to least authoritative.
*/

/* Symantec C++ is now Digital Mars */
#if defined(__DMC__) || defined(__SC__)
#  define Q_CC_SYM
/* "explicit" semantics implemented in 8.1e but keyword recognized since 7.5 */
#  if defined(__SC__) && __SC__ < 0x750
#    define Q_NO_EXPLICIT_KEYWORD
#  endif
#  define Q_NO_USING_KEYWORD
#  if !defined(_CPPUNWIND)
#    define Q_NO_EXCEPTIONS
#  endif

#elif defined(applec)
#  define Q_CC_MPW
#  define Q_NO_BOOL_TYPE
#  define Q_NO_EXPLICIT_KEYWORD
#  define Q_NO_USING_KEYWORD

#elif defined(__MWERKS__)
#  define Q_CC_MWERKS
/* "explicit" recognized since 4.0d1 */
#  define QMAC_PASCAL pascal
#  define Q_NO_USING_KEYWORD /* ### check "using" status */

#elif defined(_MSC_VER)
#  define Q_CC_MSVC
/* proper support of bool for _MSC_VER >= 1100 */
#  define Q_CANNOT_DELETE_CONSTANT
#  define Q_INLINE_TEMPLATES inline
/* Visual C++.Net issues for _MSC_VER >= 1300 */
#  if _MSC_VER >= 1300
#    define Q_CC_MSVC_NET
#    define Q_TYPENAME
#  endif
#  define Q_NO_USING_KEYWORD /* ### check "using" status */

#elif defined(__BORLANDC__) || defined(__TURBOC__)
#  define Q_CC_BOR
#  if __BORLANDC__ < 0x502
#    define Q_NO_BOOL_TYPE
#    define Q_NO_EXPLICIT_KEYWORD
#  endif
#  define Q_NO_USING_KEYWORD /* ### check "using" status */

#elif defined(__WATCOMC__)
#  define Q_CC_WAT
#  if defined(Q_OS_QNX4)
/* compiler flags */
#    define Q_TYPENAME
#    define Q_NO_BOOL_TYPE
#    define Q_CANNOT_DELETE_CONSTANT
#    define mutable
/* ??? */
#    define Q_BROKEN_TEMPLATE_SPECIALIZATION
/* no template classes in QVariant */
#    define QT_NO_TEMPLATE_VARIANT
/* Wcc does not fill in functions needed by valuelists, maps, and
   valuestacks implicitly */
#    define Q_FULL_TEMPLATE_INSTANTIATION
/* can we just compare the structures? */
#    define Q_FULL_TEMPLATE_INSTANTIATION_MEMCMP
/* these are not useful to our customers */
#    define QT_QWS_NO_SHM
#    define QT_NO_QWS_MULTIPROCESS
#    define QT_NO_SQL
#    define QT_NO_QWS_CURSOR
#  endif

#elif defined(__GNUC__)
#  define Q_CC_GNU
#  define Q_C_CALLBACKS
#  if __GNUC__ == 2 && __GNUC_MINOR__ <= 7
#    define Q_FULL_TEMPLATE_INSTANTIATION
#  endif
/* GCC 2.95 knows "using" but does not support it correctly */
#  if __GNUC__ == 2 && __GNUC_MINOR__ <= 95
#    define Q_NO_USING_KEYWORD
#  endif
#  if (defined(__arm__) || defined(__ARMEL__)) && !defined(QT_MOC_CPP)
#    define Q_PACKED __attribute__ ((packed))
#  endif
#  if !defined(__EXCEPTIONS)
#    define Q_NO_EXCEPTIONS
#  endif

/* IBM compiler versions are a bit messy. There are actually two products:
   the C product, and the C++ product. The C++ compiler is always packaged
   with the latest version of the C compiler. Version numbers do not always
   match. This little table (I'm not sure it's accurate) should be helpful:

   C++ product                C product

            C Set 3.1         C Compiler 3.0
                  ...         ...
   C++ Compiler 3.6.6         C Compiler 4.3
                  ...         ...
   Visual Age C++ 4.0         ...
                  ...         ...
   Visual Age C++ 5.0         C Compiler 5.0

   Now:
   __xlC__    is the version of the C compiler in hexadecimal notation
              is only an approximation of the C++ compiler version
   __IBMCPP__ is the version of the C++ compiler in decimal notation
              but it is not defined on older compilers like C Set 3.1 */
#elif defined(__xlC__)
#  define Q_CC_XLC
#  define Q_FULL_TEMPLATE_INSTANTIATION
#  if __xlC__ < 0x400
#    define Q_NO_BOOL_TYPE
#    define Q_NO_EXPLICIT_KEYWORD
#    define Q_NO_USING_KEYWORD
#    define Q_TYPENAME
#    define Q_INLINE_TEMPLATES inline
#    define Q_BROKEN_TEMPLATE_SPECIALIZATION
#    define Q_CANNOT_DELETE_CONSTANT
#  endif

/* Older versions of DEC C++ do not define __EDG__ or __EDG - observed
   on DEC C++ V5.5-004. New versions do define  __EDG__ - observed on
   Compaq C++ V6.3-002.
   This compiler is different enough from other EDG compilers to handle
   it separately anyway. */
#elif defined(__DECCXX)
#  define Q_CC_DEC
/* Compaq C++ V6 compilers are EDG-based but I'm not sure about older
   DEC C++ V5 compilers. */
#  if defined(__EDG__)
#    define Q_CC_EDG
#  endif
/* Compaq have disabled EDG's _BOOL macro and use _BOOL_EXISTS instead
   - observed on Compaq C++ V6.3-002.
   In any case versions prior to Compaq C++ V6.0-005 do not have bool. */
#  if !defined(_BOOL_EXISTS)
#    define Q_NO_BOOL_TYPE
#  endif
/* Spurious (?) error messages observed on Compaq C++ V6.5-014. */
#  define Q_NO_USING_KEYWORD
/* Apply to all versions prior to Compaq C++ V6.0-000 - observed on
   DEC C++ V5.5-004. */
#  if __DECCXX_VER < 60060000
#    define Q_TYPENAME
#    define Q_BROKEN_TEMPLATE_SPECIALIZATION
#    define Q_CANNOT_DELETE_CONSTANT
#  endif
/* avoid undefined symbol problems with out-of-line template members */
#  define Q_INLINE_TEMPLATES inline

/* Compilers with EDG front end are similar. To detect them we test:
   __EDG documented by SGI, observed on MIPSpro 7.3.1.1 and KAI C++ 4.0b
   __EDG__ documented in EDG online docs, observed on Compaq C++ V6.3-002 */
#elif defined(__EDG) || defined(__EDG__)
#  define Q_CC_EDG
/* From the EDG documentation (does not seem to apply to Compaq C++):
   _BOOL
    	Defined in C++ mode when bool is a keyword. The name of this
	predefined macro is specified by a configuration flag. _BOOL
	is the default.
   __BOOL_DEFINED
   	Defined in Microsoft C++ mode when bool is a keyword. */
#  if !defined(_BOOL) && !defined(__BOOL_DEFINED)
#    define Q_NO_BOOL_TYPE
#  endif

/* The Portland Group compiler is based on EDG and does define __EDG__ */
#  if defined(__COMO__)
#    define Q_CC_COMEAU
#    define Q_C_CALLBACKS

/* Using the `using' keyword avoids KAI C++ warnings */
#  elif defined(__KCC)
#    define Q_CC_KAI
#    if !defined(_EXCEPTIONS)
#      define Q_NO_EXCEPTIONS
#    endif

/* Using the `using' keyword avoids Intel C++ warnings */
#  elif defined(__INTEL_COMPILER)
#    define Q_CC_INTEL
#    if !defined(__EXCEPTIONS)
#      define Q_NO_EXCEPTIONS
#    endif

/* The Portland Group compiler is based on EDG and does define __EDG__ */
#  elif defined(__PGI)
#    define Q_CC_PGI
#    if !defined(__EXCEPTIONS)
#      define Q_NO_EXCEPTIONS
#    endif

/* Never tested! */
#  elif defined(__ghs)
#    define Q_CC_GHS

/* The UnixWare 7 UDK compiler is based on EDG and does define __EDG__ */
#  elif defined(__USLC__) && defined(__SCO_VERSION__)
#    define Q_CC_USLC
#    define Q_NO_USING_KEYWORD /* ### check "using" status */

/* Never tested! */
#  elif defined(CENTERLINE_CLPP) || defined(OBJECTCENTER)
#    define Q_CC_OC
#    define Q_NO_USING_KEYWORD

/* CDS++ is not documented to define __EDG__ or __EDG in the Reliant
   documentation but we suppose it does, in any case it does follow
   conventions like _BOOL */
#  elif defined(sinix)
#    define Q_CC_CDS
#    define Q_NO_USING_KEYWORD
#    if defined(__cplusplus) && (__cplusplus < 2) /* Cfront C++ mode */
#      define Q_NO_EXCEPTIONS
#    endif

/* The MIPSpro compiler in o32 mode is based on EDG but disables features
   such as template specialization nevertheless */
#  elif defined(__sgi)
#    define Q_CC_MIPS
#    if defined(_MIPS_SIM) && (_MIPS_SIM == _ABIO32) /* o32 ABI */
#      define Q_TYPENAME
#      define Q_BROKEN_TEMPLATE_SPECIALIZATION
#      define Q_STRICT_INLINING_RULES
#    elif defined(_COMPILER_VERSION) && (_COMPILER_VERSION < 730) /* 7.2 */
#      define Q_TYPENAME
#      define Q_BROKEN_TEMPLATE_SPECIALIZATION
#    endif
#    define Q_NO_USING_KEYWORD /* ### check "using" status */
#  endif

/* The older UnixWare 2.X compiler? */
#elif defined(__USLC__) && !defined(__SCO_VERSION__)
#  define Q_CC_USLC
#  define Q_NO_BOOL_TYPE
#  define Q_NO_EXPLICIT_KEYWORD
#  define Q_NO_USING_KEYWORD

/* Never tested! */
#elif defined(__HIGHC__)
#  define Q_CC_HIGHC

#elif defined(__SUNPRO_CC)
#  define Q_CC_SUN
/* 5.0 compiler or better
    'bool' is enabled by default but can be disabled using -features=nobool
    in which case _BOOL is not defined
        this is the default in 4.2 compatibility mode triggered by -compat=4 */
#  if __SUNPRO_CC >= 0x500
#    if !defined(_BOOL)
#      define Q_NO_BOOL_TYPE
#    endif
#    if defined(__SUNPRO_CC_COMPAT) && (__SUNPRO_CC_COMPAT <= 4)
#      define Q_NO_USING_KEYWORD
#    endif
#    define Q_C_CALLBACKS
/* 4.2 compiler or older */
#  else
#    define Q_NO_BOOL_TYPE
#    define Q_NO_EXPLICIT_KEYWORD
#    define Q_NO_USING_KEYWORD
#  endif

#elif defined(Q_OS_HPUX)
/* __HP_aCC was not defined in first aCC releases */
#  if defined(__HP_aCC) || __cplusplus >= 199707L
#    define Q_CC_HPACC
#  else
#    define Q_CC_HP
#    define Q_NO_BOOL_TYPE
#    define Q_FULL_TEMPLATE_INSTANTIATION
#    define Q_BROKEN_TEMPLATE_SPECIALIZATION
#    define Q_NO_EXPLICIT_KEYWORD
#  endif
#  define Q_NO_USING_KEYWORD /* ### check "using" status */

#else
#  error "Qt has not been tested with this compiler - talk to qt-bugs@trolltech.com"
#endif

#ifndef Q_PACKED
#  define Q_PACKED
#endif


/*
   The window system, must be one of: (Q_WS_x)

     MACX	- Mac OS X
     MAC9	- Mac OS 9
     QWS	- Qt/Embedded
     WIN32	- Windows
     X11	- X Window System
     PM		- unsupported
     WIN16	- unsupported
*/

#if defined(Q_OS_MAC9)
#  define Q_WS_MAC9
#elif defined(Q_OS_MSDOS)
#  define Q_WS_WIN16
#  error "Qt requires Win32 and does not work with Windows 3.x"
#elif defined(_WIN32_X11_)
#  define Q_WS_X11
#elif defined(Q_OS_WIN32)
#  define Q_WS_WIN32
#  if defined(Q_OS_WIN64)
#    define Q_WS_WIN64
#  endif
#elif defined(Q_OS_OS2)
#  define Q_WS_PM
#  error "Qt does not work with OS/2 Presentation Manager or Workplace Shell"
#elif defined(Q_OS_UNIX)
#  if defined(QWS)
#    define Q_WS_QWS
#  elif defined(Q_OS_MACX)
#    define Q_WS_MACX
#  else
#    define Q_WS_X11
#  endif
#endif
#if defined(Q_OS_MAC) && !defined(QMAC_PASCAL)
#  define QMAC_PASCAL
#endif

#if defined(Q_WS_WIN16) || defined(Q_WS_WIN32)
#  define Q_WS_WIN
#endif

#if (defined(Q_WS_MAC9) || defined(Q_WS_MACX)) && !defined(Q_WS_QWS) && !defined(Q_WS_X11)
#  define Q_WS_MAC
#endif


/*
   Some classes do not permit copies to be made of an object.
   These classes contains a private copy constructor and operator=
   to disable copying (the compiler gives an error message).
   Undefine Q_DISABLE_COPY to turn off this checking.
*/

#define Q_DISABLE_COPY

#if defined(__cplusplus)


//
// Useful type definitions for Qt
//

#if defined(Q_NO_BOOL_TYPE)
#if defined(Q_CC_HP)
// bool is an unsupported reserved keyword in later versions
#define bool int
#else
typedef int bool;
#endif
#endif

typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned	uint;
typedef unsigned long   ulong;
typedef char	       *pchar;
typedef uchar	       *puchar;
typedef const char     *pcchar;


//
// Constant bool values
//

#ifndef TRUE
const bool FALSE = 0;
const bool TRUE = !0;
#endif
#if defined(__WATCOMC__)
#  if defined(Q_OS_QNX4)
const bool false = FALSE;
const bool true = TRUE;
#  endif
#endif


//
// Use the "explicit" keyword on platforms that support it.
//

#if !defined(Q_NO_EXPLICIT_KEYWORD)
#  define Q_EXPLICIT explicit
#else
#  define Q_EXPLICIT
#endif


//
// Workaround for static const members on MSVC++.
//

#if defined(Q_CC_MSVC)
#  define QT_STATIC_CONST static
#  define QT_STATIC_CONST_IMPL
#else
#  define QT_STATIC_CONST static const
#  define QT_STATIC_CONST_IMPL const
#endif


//
// Utility macros and inline functions
//

#define QMAX(a, b)	((b) < (a) ? (a) : (b))
#define QMIN(a, b)	((a) < (b) ? (a) : (b))
#define QABS(a)	((a) >= 0  ? (a) : -(a))

inline int qRound( double d )
{
    return d >= 0.0 ? int(d + 0.5) : int( d - ((int)d-1) + 0.5 ) + ((int)d-1);
}


//
// Size-dependent types (architechture-dependent byte order)
//

#if !defined(QT_CLEAN_NAMESPACE)
// source compatibility with Qt 1.x
typedef signed char		INT8;		// 8 bit signed
typedef unsigned char		UINT8;		// 8 bit unsigned
typedef short			INT16;		// 16 bit signed
typedef unsigned short		UINT16;		// 16 bit unsigned
typedef int			INT32;		// 32 bit signed
typedef unsigned int		UINT32;		// 32 bit unsigned
#endif

typedef signed char		Q_INT8;		// 8 bit signed
typedef unsigned char		Q_UINT8;	// 8 bit unsigned
typedef short			Q_INT16;	// 16 bit signed
typedef unsigned short		Q_UINT16;	// 16 bit unsigned
typedef int			Q_INT32;	// 32 bit signed
typedef unsigned int		Q_UINT32;	// 32 bit unsigned
#if defined(Q_OS_WIN64)
// LLP64 64-bit model on Windows
typedef __int64			Q_LONG;		// word up to 64 bit signed
typedef unsigned __int64	Q_ULONG;	// word up to 64 bit unsigned
#else
// LP64 64-bit model on Linux
typedef long			Q_LONG;
typedef unsigned long		Q_ULONG;
#endif

#if !defined(QT_CLEAN_NAMESPACE)
// mininum size of 64 bits is not guaranteed
#define Q_INT64			Q_LONG
#define Q_UINT64		Q_ULONG
#endif

#if defined(Q_OS_MACX) && !defined(QT_LARGEFILE_SUPPORT)
#  define QT_LARGEFILE_SUPPORT 64
#endif
#if defined(QT_LARGEFILE_SUPPORT)
    typedef unsigned long long QtOffset;
#else
    typedef Q_ULONG QtOffset;
#endif


//
// Data stream functions is provided by many classes (defined in qdatastream.h)
//

class QDataStream;


//
// Feature subsetting
//
// Note that disabling some features will produce a libqt that is not
// compatible with other libqt builds. Such modifications are only
// supported on Qt/Embedded where reducing the library size is important
// and where the application-suite is often a fixed set.
//

#if !defined(QT_MOC)
#if defined(QCONFIG_LOCAL)
#include <qconfig-local.h>
#elif defined(QCONFIG_MINIMAL)
#include <qconfig-minimal.h>
#elif defined(QCONFIG_SMALL)
#include <qconfig-small.h>
#elif defined(QCONFIG_MEDIUM)
#include <qconfig-medium.h>
#elif defined(QCONFIG_LARGE)
#include <qconfig-large.h>
#else // everything...
#include <qconfig.h>
#endif
#endif


#ifndef QT_BUILD_KEY
#define QT_BUILD_KEY "unspecified"
#endif

// prune to local config
#include "qmodules.h"
#ifndef QT_MODULE_ICONVIEW
# define QT_NO_ICONVIEW
#endif
#ifndef QT_MODULE_WORKSPACE
# define QT_NO_WORKSPACE
#endif
#ifndef QT_MODULE_NETWORK
#define QT_NO_NETWORK
#endif
#ifndef QT_MODULE_CANVAS
# define QT_NO_CANVAS
#endif
#ifndef QT_MODULE_TABLE
#define QT_NO_TABLE
#endif
#ifndef QT_MODULE_XML
# define QT_NO_XML
#endif
#ifndef QT_MODULE_OPENGL
# define QT_NO_OPENGL
#endif
#if !defined(QT_MODULE_SQL)
# define QT_NO_SQL
#endif

#if defined(Q_WS_MAC9)
//No need for menu merging
#  ifndef QMAC_QMENUBAR_NO_MERGE
#    define QMAC_QMENUBAR_NO_MERGE
#  endif
//Mac9 does not use quartz
#  ifndef QMAC_NO_QUARTZ
#    define QMAC_NO_QUARTZ
#  endif
#  ifndef QMAC_QMENUBAR_NO_EVENT
#    define QMAC_QMENUBAR_NO_EVENT
#  endif
#endif
#if defined(Q_WS_MACX) //for no nobody uses quartz, just putting in first level hooks
#  ifndef QMAC_NO_QUARTZ
#    define QMAC_NO_QUARTZ
#  endif
#  ifndef QMAC_QMENUBAR_NO_EVENT
#    define QMAC_QMENUBAR_NO_EVENT
#  endif
#endif

#ifndef QT_H
#include <qfeatures.h>
#endif // QT_H


//
// Create Qt DLL if QT_DLL is defined (Windows only)
//

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#  if defined(QT_NODLL)
#    undef QT_MAKEDLL
#    undef QT_DLL
#  elif defined(QT_MAKEDLL)	/* create a Qt DLL library */
#    if defined(QT_DLL)
#      undef QT_DLL
#    endif
#    define Q_EXPORT  __declspec(dllexport)
#    define Q_TEMPLATEDLL
#    define Q_TEMPLATE_EXTERN
#    undef  Q_DISABLE_COPY	/* avoid unresolved externals */
#  elif defined(QT_DLL)		/* use a Qt DLL library */
#    define Q_EXPORT  __declspec(dllimport)
#    define Q_TEMPLATEDLL
#    ifndef Q_TEMPLATE_EXTERN
#      if defined(Q_CC_MSVC)
#        define Q_TEMPLATE_EXTERN /*extern*/ //### too many warnings, even though disabled
#      else
#        define Q_TEMPLATE_EXTERN
#      endif
#    endif
#    undef  Q_DISABLE_COPY  /* avoid unresolved externals */
#  endif
#else
#  undef QT_MAKEDLL		/* ignore these for other platforms */
#  undef QT_DLL
#endif

#ifndef Q_EXPORT
#  define Q_EXPORT
#endif


//
// Some platform specific stuff
//

#if defined(Q_WS_WIN)
extern Q_EXPORT bool qt_winunicode;
#endif


//
// System information
//

Q_EXPORT const char *qVersion();
Q_EXPORT bool qSysInfo( int *wordSize, bool *bigEndian );
#if defined(Q_WS_WIN)
Q_EXPORT int qWinVersion();
#if defined(UNICODE)
#define QT_WA( uni, ansi ) if ( qt_winunicode ) { uni } else { ansi }
#define QT_WA_INLINE( uni, ansi ) ( qt_winunicode ? uni : ansi )
#else
#define QT_WA( uni, ansi ) ansi
#define QT_WA_INLINE( uni, ansi ) ansi
#endif
#endif

#ifdef Q_OS_TEMP
#ifdef QT_WA
#undef QT_WA
#undef QT_WA_INLINE
#endif
#define QT_WA( uni, ansi ) uni
#define QT_WA_INLINE( uni, ansi ) ( uni )
#endif

#ifndef Q_INLINE_TEMPLATES
#  define Q_INLINE_TEMPLATES
#endif

#ifndef Q_TYPENAME
#  define Q_TYPENAME typename
#endif

//
// Use to avoid "unused parameter" warnings
//

#define Q_UNUSED(x) (void)x;

//
// Debugging and error handling
//

#if !defined(QT_NO_CHECK)
#  define QT_CHECK_STATE			// check state of objects etc.
#  define QT_CHECK_RANGE			// check range of indexes etc.
#  define QT_CHECK_NULL				// check null pointers
#  define QT_CHECK_MATH				// check math functions
#endif

#if !defined(QT_NO_DEBUG) && !defined(QT_DEBUG)
#  define QT_DEBUG				// display debug messages
#  if !defined(QT_NO_COMPAT)			// compatibility with Qt 2
#    if !defined(NO_DEBUG) && !defined(DEBUG)
#      if !defined(Q_OS_MACX)			// clash with MacOS X headers
#        define DEBUG
#      endif
#    endif
#  endif
#endif


Q_EXPORT void qDebug( const char *, ... )	// print debug message
#if defined(Q_CC_GNU) && !defined(__INSURE__)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

Q_EXPORT void qWarning( const char *, ... )	// print warning message
#if defined(Q_CC_GNU) && !defined(__INSURE__)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

Q_EXPORT void qFatal( const char *, ... )	// print fatal message and exit
#if defined(Q_CC_GNU)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

Q_EXPORT void qSystemWarning( const char *, int code = -1 );

#if !defined(QT_CLEAN_NAMESPACE) 		// compatibility with Qt 1

Q_EXPORT void debug( const char *, ... )	// print debug message
#if defined(Q_CC_GNU) && !defined(__INSURE__)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

Q_EXPORT void warning( const char *, ... )	// print warning message
#if defined(Q_CC_GNU) && !defined(__INSURE__)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

Q_EXPORT void fatal( const char *, ... )	// print fatal message and exit
#if defined(Q_CC_GNU) && !defined(__INSURE__)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

#endif // QT_CLEAN_NAMESPACE


#if !defined(Q_ASSERT)
#  if defined(QT_CHECK_STATE)
#    if defined(QT_FATAL_ASSERT)
#      define Q_ASSERT(x)  ((x) ? (void)0 : qFatal("ASSERT: \"%s\" in %s (%d)",#x,__FILE__,__LINE__))
#    else
#      define Q_ASSERT(x)  ((x) ? (void)0 : qWarning("ASSERT: \"%s\" in %s (%d)",#x,__FILE__,__LINE__))
#    endif
#  else
#    define Q_ASSERT(x)
#  endif
#endif

#if !defined(QT_NO_COMPAT)			// compatibility with Qt 2
#  if !defined(ASSERT)
#    if !defined(Q_OS_TEMP)
#      define ASSERT(x) Q_ASSERT(x)
#    endif
#  endif
#endif // QT_NO_COMPAT


Q_EXPORT bool qt_check_pointer( bool c, const char *, int );

#if defined(QT_CHECK_NULL)
#  define Q_CHECK_PTR(p) (qt_check_pointer((p)==0,__FILE__,__LINE__))
#else
#  define Q_CHECK_PTR(p)
#endif

#if !defined(QT_NO_COMPAT)			// compatibility with Qt 2
#  if !defined(CHECK_PTR)
#    define CHECK_PTR(x) Q_CHECK_PTR(x)
#  endif
#endif // QT_NO_COMPAT

enum QtMsgType { QtDebugMsg, QtWarningMsg, QtFatalMsg };

typedef void (*QtMsgHandler)(QtMsgType, const char *);
Q_EXPORT QtMsgHandler qInstallMsgHandler( QtMsgHandler );

#if !defined(QT_NO_COMPAT)			// compatibility with Qt 2
typedef QtMsgHandler msg_handler;
#endif // QT_NO_COMPAT

Q_EXPORT void qSuppressObsoleteWarnings( bool = TRUE );

Q_EXPORT void qObsolete( const char *obj, const char *oldfunc,
		   const char *newfunc );
Q_EXPORT void qObsolete( const char *obj, const char *oldfunc );
Q_EXPORT void qObsolete( const char *message );


//
// Install paths from configure
//

Q_EXPORT const char *qInstallPath();
Q_EXPORT const char *qInstallPathDocs();
Q_EXPORT const char *qInstallPathHeaders();
Q_EXPORT const char *qInstallPathLibs();
Q_EXPORT const char *qInstallPathBins();
Q_EXPORT const char *qInstallPathPlugins();
Q_EXPORT const char *qInstallPathData();

#endif // __cplusplus

#endif // QGLOBAL_H

//
// Avoid some particularly useless warnings from some stupid compilers.
// To get ALL C++ compiler warnings, define QT_CC_WARNINGS or comment out
// the line "#define QT_NO_WARNINGS"
//

#if !defined(QT_CC_WARNINGS)
#  define QT_NO_WARNINGS
#endif
#if defined(QT_NO_WARNINGS)
#  if defined(Q_CC_MSVC)
#    pragma warning(disable: 4244) // 'conversion' conversion from 'type1' to 'type2', possible loss of data
#    pragma warning(disable: 4275) // non - DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier'
#    pragma warning(disable: 4514) // unreferenced inline/local function has been removed
#    pragma warning(disable: 4800) // 'type' : forcing value to bool 'true' or 'false' (performance warning)
#    pragma warning(disable: 4097) // typedef-name 'identifier1' used as synonym for class-name 'identifier2'
#    pragma warning(disable: 4706) // assignment within conditional expression
#    pragma warning(disable: 4786) // truncating debug info after 255 characters
#    pragma warning(disable: 4660) // template-class specialization 'identifier' is already instantiated
#    pragma warning(disable: 4355) // 'this' : used in base member initializer list
#    pragma warning(disable: 4231) // nonstandard extension used : 'extern' before template explicit instantiation
#  elif defined(Q_CC_BOR)
#    pragma option -w-inl
#    pragma option -w-aus
#    pragma warn -inl
#    pragma warn -pia
#    pragma warn -ccc
#    pragma warn -rch
#    pragma warn -sig
#  elif defined(Q_CC_MWERKS)
#    pragma warn_possunwant off
#  endif
#endif
