/****************************************************************************
** $Id: qcriticalsection_p.cpp,v 1.1 2002-11-01 00:10:44 kergoth Exp $
**
** Implementation of QCriticalSection class
**
** Created : 
**
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
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

#if defined(QT_THREAD_SUPPORT)

#include "qt_windows.h"

#include <private/qcriticalsection_p.h>

class QCriticalSectionPrivate 
{
public:
    QCriticalSectionPrivate() {}

    CRITICAL_SECTION section;
};


QCriticalSection::QCriticalSection()
{
    d = new QCriticalSectionPrivate;
    InitializeCriticalSection( &d->section );
}

QCriticalSection::~QCriticalSection()
{
    DeleteCriticalSection( &d->section );
    delete d;
}

void QCriticalSection::enter()
{
    EnterCriticalSection( &d->section );
}

void QCriticalSection::leave()
{
    LeaveCriticalSection( &d->section );
}

#endif
