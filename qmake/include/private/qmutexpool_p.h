/****************************************************************************
** $Id: qmutexpool_p.h,v 1.2 2003-07-10 02:40:11 llornkcor Exp $
**
** ...
**
** Copyright (C) 2002 Trolltech AS.  All rights reserved.
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

#ifndef QMUTEXPOOL_P_H
#define QMUTEXPOOL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QSettings. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//
//

#ifdef QT_THREAD_SUPPORT

#include "qmutex.h"
#include "qmemarray.h"

class QMutexPool
{
public:
    QMutexPool( bool recursive = FALSE, int size = 17 );
    ~QMutexPool();

    QMutex *get( void *address );

private:
    QMutex mutex;
    QMutex **mutexes;
    int count;
    bool recurs;
};

extern QMutexPool *qt_global_mutexpool;

#endif // QT_THREAD_SUPPORT

#endif // QMUTEXPOOL_P_H
