/* This file is part of the KDE libraries
    Copyright (C) 1997 Christian Czezakte (e9025461@student.tuwien.ac.at)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/
//
//  KPROCESSCONTROLLER -- A helper class for KProcess
//
//  version 0.3.1, Jan 8th 1997
//
//  (C) Christian Czezatke
//  e9025461@student.tuwien.ac.at
//  Ported by Holger Freyther
//

#ifndef __KPROCCTRL_H__
#define __KPROCCTRL_H__

#include <qvaluelist.h>
#include <qtimer.h>

#include "oprocess.h"

class QSocketNotifier;


namespace Opie {
namespace Core {
namespace Internal {
class OProcessControllerPrivate;

/**
 * @short Used internally by @ref OProcess
 * @internal
 * @author Christian Czezakte <e9025461@student.tuwien.ac.at>
 *
 *  A class for internal use by OProcess only. -- Exactly one instance
 *  of this class is generated by the first instance of OProcess that is
 *  created (a pointer to it gets stored in @ref theOProcessController ).
 *
 * This class takes care of the actual (UN*X) signal handling.
*/
class OProcessController : public QObject
{
    Q_OBJECT

public:
    OProcessController();
    ~OProcessController();
    //CC: WARNING! Destructor Not virtual (but you don't derive classes from this anyhow...)

public:

    /**
     * Only a single instance of this class is allowed at a time,
     * and this static variable is used to track the one instance.
     */
    static OProcessController *theOProcessController;

    /**
     * Automatically called upon SIGCHLD.
     *
     * Normally you do not need to do anything with this function but
     * if your application needs to disable SIGCHLD for some time for
     * reasons beyond your control, you should call this function afterwards
     * to make sure that no SIGCHLDs where missed.
     */
    static void theSigCHLDHandler(int signal);
    // handler for sigchld

    /**
     * @internal
    */
    static void setupHandlers();
    /**
     * @internal
    */
    static void resetHandlers();
    /**
     * @internal
    */
    void addOProcess( OProcess* );
    /**
     * @internal
    */
    void removeOProcess( OProcess* );
public slots:
    /**
     * @internal
     */
    void slotDoHousekeeping(int socket);

private slots:
    void delayedChildrenCleanup();
private:
    int fd[2];
    QSocketNotifier *notifier;
    static struct sigaction oldChildHandlerData;
    static bool handlerSet;
    QValueList<OProcess*> processList;
    QTimer delayedChildrenCleanupTimer;

    // Disallow assignment and copy-construction
    OProcessController( const OProcessController& );
    OProcessController& operator= ( const OProcessController& );

    OProcessControllerPrivate *d;
};

}
}
}


#endif

