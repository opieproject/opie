/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** Licensees holding valid Qtopia Developer license may use this
** file in accordance with the Qtopia Developer License Agreement
** provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
** THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
** PURPOSE.
**
** email sales@trolltech.com for information about Qtopia License
** Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QPC_GLOBAL_H
#define QPC_GLOBAL_H

#if ( defined(Q_OS_WIN32) || defined(Q_OS_WIN64) ) && defined(PALMTOPCENTER)
#include <qglobal.h>
// #  if defined(QT_NODLL)
//#    undef QPC_MAKEDLL
//#    undef QPC_DLL
#  if defined(QPC_MAKEDLL)	/* create a Qt DLL library */
#    if defined(QPC_DLL)
#      undef QPC_DLL
#    endif
#    define QPC_EXPORT  __declspec(dllexport)
#    define QPC_TEMPLATEDLL
#    undef  QPC_DISABLE_COPY	/* avoid unresolved externals */
#  elif defined(QPC_DLL)		/* use a Qt DLL library */
#    define QPC_EXPORT  __declspec(dllimport)
#    define QPC_TEMPLATEDLL
#    undef  QPC_DISABLE_COPY	/* avoid unresolved externals */
#  endif
#else
#  undef QPC_MAKEDLL		/* ignore these for other platforms */
#  undef QPC_DLL
#endif
#endif

#ifndef QPC_EXPORT
#  define QPC_EXPORT
#endif
