/****************************************************************************
** $Id: qeventloop_p.h,v 1.2 2003-07-10 02:40:11 llornkcor Exp $
**
** Definition of QEventLoop class
**
** Copyright (C) 1992-2003 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses for Qt/Embedded may use this file in accordance with the
** Qt Embedded Commercial License Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QEVENTLOOP_P_H
#define QEVENTLOOP_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  This header file may
// change from version to version without notice, or even be
// removed.
//
// We mean it.
//
//

#include "qplatformdefs.h"

// SCO OpenServer redefines raise -> kill
#if defined(raise)
# undef raise
#endif

#include "qwindowdefs.h"

class QSocketNotifier;

#if defined(Q_OS_UNIX) || defined (Q_WS_WIN)
#include <qptrlist.h>
#endif // Q_OS_UNIX || Q_WS_WIN

#if defined(Q_OS_UNIX)
struct QSockNot
{
    QSocketNotifier *obj;
    int fd;
    fd_set *queue;
};

class QSockNotType
{
public:
    QSockNotType();
    ~QSockNotType();

    QPtrList<QSockNot> *list;
    fd_set select_fds;
    fd_set enabled_fds;
    fd_set pending_fds;

};
#endif // Q_OS_UNIX

#if defined(Q_WS_WIN)
struct QSockNot {
    QSocketNotifier *obj;
    int fd;
};
#endif // Q_WS_WIN

class QEventLoopPrivate
{
public:
    QEventLoopPrivate()
    {
	reset();
    }

    void reset() {
	looplevel = 0;
	quitcode = 0;
	quitnow = FALSE;
	exitloop = FALSE;
	shortcut = FALSE;
    }

    int looplevel;
    int quitcode;
    unsigned int quitnow  : 1;
    unsigned int exitloop : 1;
    unsigned int shortcut : 1;

#if defined(Q_WS_MAC)
    EventLoopTimerRef select_timer;
#endif

#if defined(Q_WS_X11)
    int xfd;
#endif // Q_WS_X11

#if defined(Q_OS_UNIX)
    int thread_pipe[2];

    // pending socket notifiers list
    QPtrList<QSockNot> sn_pending_list;
    // highest fd for all socket notifiers
    int sn_highest;
    // 3 socket notifier types - read, write and exception
    QSockNotType sn_vec[3];
#endif

#ifdef Q_WS_WIN
    // pending socket notifiers list
    QPtrList<QSockNot> sn_pending_list;
#endif // Q_WS_WIN

};

#endif // QEVENTLOOP_P_H
