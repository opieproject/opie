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
#include <qobject.h>
#include <qsignal.h>
#include <qintdict.h>
#include <qdir.h>

/* STD */
#include <sys/types.h>
#include <sys/stat.h>
#ifndef _GNU_SOURCE
    #define _GNU_SOURCE
    #include <fcntl.h>
    #undef _GNU_SOURCE
#else
    #include <fcntl.h>
#endif
#include <string.h>
#include <errno.h>
#include <unistd.h>

static QIntDict<OFileNotification> notification_list;

namespace Opie {
namespace Core {

OFileNotification::OFileNotification( QObject* parent, const char* name )
                  :QObject( parent, name ), _active( false )
{
    qDebug( "OFileNotification::OFileNotification()" );
}


OFileNotification::~OFileNotification()
{
    qDebug( "OFileNotification::~OFileNotification()" );
}


bool OFileNotification::isActive() const
{
    return _active;
}


int OFileNotification::start( const QString& path, bool sshot, OFileNotificationType type )
{
    _path = QString::null;
    _fd = 0;
    if ( _active ) stop();

    int fd = ::open( (const char*) path, O_RDONLY );
    if ( fd != -1 )
    {
        if ( notification_list.isEmpty() )
        {
            OFileNotification::registerSignalHandler();
        }
        int result = ::fcntl( fd, F_SETSIG, SIGRTMIN );
        if ( result == -1 )
        {
            qWarning( "OFileNotification::start(): Can't subscribe to '%s': %s.", (const char*) path, strerror( errno ) );
            return -1;
        }
        if ( !sshot ) (int) type |= (int) Multi;
        result = ::fcntl( fd, F_NOTIFY, type );
        if ( result == -1 )
        {
            qWarning( "OFileNotification::start(): Can't subscribe to '%s': %s.", (const char*) path, strerror( errno ) );
            return -1;
        }
        qDebug( "OFileNotification::start(): Subscribed for changes to %s (fd = %d, mask = 0x%0x)", (const char*) path, fd, type );
        notification_list.insert( fd, this );
        _type = type;
        _path = path;
        _fd = fd;
        return fd;
    }
    else
    {
        qWarning( "OFileNotification::start(): Error with path '%s': %s.", (const char*) path, strerror( errno ) );
        return -1;
    }
}


void OFileNotification::stop()
{
    if ( !_active ) return;

    int result = ::fcntl( _fd, F_NOTIFY, 0 );
    if ( result == -1 )
    {
        qWarning( "OFileNotification::stop(): Can't remove subscription to '%s': %s.", (const char*) _path, strerror( errno ) );
    }
    else
    {
        ::close( _fd );
        _type = Single;
        _path = QString::null;
        _fd = 0;
        _active = false;
    }
}


OFileNotificationType OFileNotification::type() const
{
    return _type;
}


QString OFileNotification::path() const
{
    return _path;
}

int OFileNotification::fileno() const
{
    return _fd;
}

void OFileNotification::activate()
{
    emit triggered();
    _signal.activate();
}


void OFileNotification::singleShot( const QString& path, QObject* receiver, const char* member, OFileNotificationType type )
{
    OFileNotification* ofn = new OFileNotification();
    ofn->_signal.connect( receiver, member );
    ofn->start( path, true, type );
}


void OFileNotification::__signalHandler( int sig, siginfo_t *si, void *data )
{
    qWarning( "OFileNotification::__signalHandler(): reached." );
    int fd = si->si_fd;
    OFileNotification* fn = notification_list[fd];
    if ( fn )
    {
        fn->activate();
        #if 1
        if ( !(fn->type() & Multi) )
        {
            qDebug( "OFileNotification::__signalHandler(): '%d' was singleShot. Removing from list.", fd );
            notification_list.remove( fd );
            if ( notification_list.isEmpty() )
            {
                OFileNotification::unregisterSignalHandler();
            }
        }
        #endif
    }
    else
    {
        qWarning( "OFileNotification::__signalHandler(): D'oh! Called without fd in notification_list. Race condition?" );
    }
}


bool OFileNotification::registerSignalHandler()
{
    struct sigaction act;
    act.sa_sigaction = OFileNotification::__signalHandler;
    ::sigemptyset( &act.sa_mask );
    act.sa_flags = SA_SIGINFO;
    if ( ::sigaction( SIGRTMIN, &act, NULL ) == -1 )
    {
        qWarning( "OFileNotification::registerSignalHandler(): couldn't register signal handler: %s", strerror( errno ) );
        return false;
    }
    qDebug( "OFileNotification::registerSignalHandler(): done" );
}


void OFileNotification::unregisterSignalHandler()
{
    struct sigaction act;
    act.sa_sigaction = ( void (*)(int, siginfo_t*, void*) ) SIG_DFL;
    ::sigemptyset( &act.sa_mask );
    if ( ::sigaction( SIGRTMIN, &act, NULL ) == -1 )
    if ( ::sigaction( SIGRTMIN, &act, NULL ) == -1 )
    {
        qWarning( "OFileNotification::unregisterSignalHandler(): couldn't deregister signal handler: %s", strerror( errno ) );
    }
    qDebug( "OFileNotification::unregisterSignalHandler(): done" );
}


}
}
