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

#ifndef QLIBRARY_P_H
#define QLIBRARY_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the OpieLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//
//

#include "opielibrary.h"

//#ifndef QT_NO_COMPONENT

//#ifndef QT_H
#include "qwindowdefs.h"
//#ifndef QT_LITE_COMPONENT
#include "qobject.h"
//#endif
//#endif // QT_H

class QTimer;
class OpieLibrary;
class OpieLibraryInterface;

/*
  Private helper class that saves the platform dependent handle
  and does the unload magic using a QTimer.
*/
//#ifndef QT_LITE_COMPONENT
class OpieLibraryPrivate : public QObject
{
    Q_OBJECT
public:
    OpieLibraryPrivate( OpieLibrary *lib );
    ~OpieLibraryPrivate();
 
    void startTimer();
    void killTimer();

#ifdef Q_WS_WIN
    HINSTANCE pHnd;
#else
    void *pHnd;
#endif

    OpieLibraryInterface *libIface;

    bool loadLibrary();
    bool freeLibrary();
    void *resolveSymbol( const char * );

private slots:
    void tryUnload();

private:
    QTimer *unloadTimer;
    OpieLibrary *library;
};

#else // QT_LITE_COMPONENT
class OpieLibraryPrivate
{
public:
    OpieLibraryPrivate( OpieLibrary *lib );

    void startTimer();
    void killTimer();

#ifdef Q_WS_WIN
    HINSTANCE pHnd;
#else
    void *pHnd;
#endif
    OpieLibraryInterface *libIface;

    bool loadLibrary();
    bool freeLibrary();
    void *resolveSymbol( const char * );

private:
    OpieLibrary *library;
};
//#endif // QT_LITE_COMPONENT

//#endif // QT_NO_COMPONENT

#endif // QLIBRARY_P_H
