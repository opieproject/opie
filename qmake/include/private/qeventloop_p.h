#ifndef QEVENTLOOP_P_H
#define QEVENTLOOP_P_H

#include "qplatformdefs.h"
#include "qwindowdefs.h"

class QSocketNotifier;

#if defined(Q_OS_UNIX)
#include <qptrlist.h>

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
    }

    int looplevel;
    int quitcode;
    bool quitnow;
    bool exitloop;

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
};

#endif // QEVENTLOOP_P_H
