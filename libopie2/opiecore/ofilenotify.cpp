/*
                            This file is part of the Opie Project
             =.             Copyright (C) 2004 Michael 'Mickey' Lauer <mickey@Vanille.de>
           .=l.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
          .>+-=
_;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=         redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :          the terms of the GNU Library General Public
.="- .-=="i,     .._        License as published by the Free Software
- .   .-<_>     .<>         Foundation; either version 2 of the License,
    ._= =}       :          or (at your option) any later version.
   .%`+i>       _;_.
   .i_,=:_.      -<s.       This program is distributed in the hope that
    +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
   : ..    .:,     . . .    without even the implied warranty of
   =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
 _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;     Library General Public License for more
++=   -.     .`     .:      details.
:     =  ...= . :.=-
-.   .:....=;==+<;          You should have received a copy of the GNU
 -_. . .   )=.  =           Library General Public License along with
   --        :-=`           this library; see the file COPYING.LIB.
                            If not, write to the Free Software Foundation,
                            Inc., 59 Temple Place - Suite 330,
                            Boston, MA 02111-1307, USA.
*/

#include "ofilenotify.h"
using namespace Opie::Core;

/* OPIE */

/* QT */
#include <qsignal.h>
#include <qintdict.h>
#include <qdir.h>

/* STD */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

namespace Opie {
namespace Core {

class OFileNotification
{
  public:
    OFileNotification( QObject* receiver, const char* member, OFileNotificationType type ) : _type( type )
    {
        _signal.connect( receiver, member );
    }
    ~OFileNotification()
    {
    }

    void activate()
    {
        _signal.activate();
    }

    OFileNotificationType type()
    {
        return _type;
    }

  private:
    OFileNotificationType _type;
    QSignal _signal;
};


static QIntDict<OFileNotification> notification_list;


void OFileNotifier::singleShot( const QString& path, QObject* receiver, const char* member, OFileNotificationType type )
{
    int fd = ::open( (const char*) path, O_RDONLY );
    if ( fd != -1 )
    {
        if ( notification_list.isEmpty() )
        {
            OFileNotifier::registerSignalHandler();
        }
        int result = ::fcntl( fd, F_SETSIG, SIGRTMIN );
        if ( result == -1 )
        {
            qWarning( "OFileNotifier::singleShot(): Can't subscribe to '%s': %s.", (const char*) path, strerror( errno ) );
            return;
        }
        result = ::fcntl( fd, F_NOTIFY, type );
        if ( result == -1 )
        {
            qWarning( "OFileNotifier::singleShot(): Can't subscribe to '%s': %s.", (const char*) path, strerror( errno ) );
            return;
        }
        qDebug( "OFileNotifier::singleShot(): Subscribed for changes to %s (fd = %d)", (const char*) path, fd );
        notification_list.insert( fd, new OFileNotification( receiver, member, type ) );
    }
    else
    {
        qWarning( "OFileNotifier::singleShot(): Error with path '%s': %s.", (const char*) path, strerror( errno ) );
    }
}


void OFileNotifier::__signalHandler( int sig, siginfo_t *si, void *data )
{
    qWarning( "OFileNotifier::__signalHandler(): reached." );
    int fd = si->si_fd;
    OFileNotification* fn = notification_list[fd];
    if ( fn )
    {
        fn->activate();
        #if 1
        if ( !(fn->type() & Multi) )
        {
            qDebug( "OFileNotifier::__signalHandler(): '%d' was singleShot. Removing from list.", fd );
            notification_list.remove( fd );
            if ( notification_list.isEmpty() )
            {
                OFileNotifier::unregisterSignalHandler();
            }
        }
        #endif
    }
    else
    {
        qWarning( "OFileNotifier::__signalHandler(): D'oh! Called without fd in notification_list. Race condition?" );
    }
}


void OFileNotifier::registerSignalHandler()
{
    struct sigaction act;
    act.sa_sigaction = OFileNotifier::__signalHandler;
    ::sigemptyset( &act.sa_mask );
    act.sa_flags = SA_SIGINFO;
    ::sigaction( SIGRTMIN, &act, NULL );
    qDebug( "OFileNotifier::registerSignalHandler(): done" );
}


void OFileNotifier::unregisterSignalHandler()
{
    struct sigaction act;
    act.sa_sigaction = ( void (*)(int, siginfo_t*, void*) ) SIG_DFL;
    sigemptyset( &act.sa_mask );
    ::sigaction( SIGRTMIN, &act, NULL );
    qDebug( "OFileNotifier::unregisterSignalHandler(): done" );
}


}
}
