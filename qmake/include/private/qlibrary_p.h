/****************************************************************************
** $Id: qlibrary_p.h,v 1.1 2002-11-01 00:10:43 kergoth Exp $
**
** Definition of an internal QLibrary class
**
** Created : 2000-01-01
**
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
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

#ifndef QLIBRARY_P_H
#define QLIBRARY_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//
//

#include "qlibrary.h"

#ifndef QT_NO_LIBRARY

#ifndef QT_H
#if defined(Q_CC_GNU)
#warning "avoid including header file \"qwindowdefs.h\" in directory 'tools'"
#endif
#include "qwindowdefs.h"
#endif // QT_H

class QLibraryPrivate
{
public:
    QLibraryPrivate( QLibrary *lib );

#ifdef Q_WS_WIN
    HINSTANCE pHnd;
#else
    void *pHnd;
#endif

    bool loadLibrary();
    bool freeLibrary();
    void *resolveSymbol( const char * );

private:
    QLibrary *library;
};

#endif // QT_NO_LIBRARY
#endif // QLIBRARY_P_H
