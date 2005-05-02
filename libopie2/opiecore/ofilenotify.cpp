/*
                            This file is part of the Opie Project
             =.             Copyright (C) 2004-2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
           .=l.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
          .>+-=
_;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=         redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :          the terms of the GNU Library General Public
.="- .-=="i,     .._        License as published by the Free Software
- .   .-<_>     .<>         Foundation; version 2 of the License.
    ._= =}       :
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
#include <qsocketnotifier.h>
#include <qsignal.h>
#include <qintdict.h>
#include <qdir.h>

/* STD */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

static QIntDict<OFileNotification> notification_list;

QSocketNotifier* OFileNotification::_sn;
int OFileNotification::_fd = -1;

#define INOTIFY_DEVICE "/dev/inotify"

namespace Opie {
namespace Core {

OFileNotification::OFileNotification( QObject* parent, const char* name )
                  :QObject( parent, name ), _active( false ), _multi( true )
{
    qDebug( "OFileNotification::OFileNotification()" );
}


OFileNotification::~OFileNotification()
{
    stop();
    qDebug( "OFileNotification::~OFileNotification()" );
}


bool OFileNotification::isActive() const
{
    return _active;
}


int OFileNotification::watch( const QString& path, bool sshot, OFileNotificationType type )
{
    if ( QFile::exists( path ) )
    {
        if ( notification_list.isEmpty() )
        {
            OFileNotification::registerEventHandler();
        }

        struct inotify_watch_request iwr;
        ::memset( &iwr, 0, sizeof iwr );
        iwr.name = const_cast<char*>( (const char*) path );
        iwr.mask = type;

        _wd = ::ioctl( OFileNotification::_fd, INOTIFY_WATCH, &iwr );

        if ( _wd < 0 )
        {
            qWarning( "OFileNotification::watch(): inotify can't watch '%s': %s.", (const char*) path, strerror( errno ) );
            return -1;
        }

        notification_list.insert( _wd, this );
        _multi = !sshot;
        _type = type;
        _active = true;
        qDebug( "OFileNotification::watch(): watching '%s' [wd=%d].", (const char*) path, _wd );
        return _wd;
    }
    else
    {
        qWarning( "OFileNotification::watch(): Can't watch '%s': %s.", (const char*) path, strerror( errno ) );
        return -1;
    }
}


void OFileNotification::stop()
{
    notification_list.remove( _wd );
    _path = QString::null;
    _wd = 0;
    _active = false;
    if ( notification_list.isEmpty() )
    {
        OFileNotification::unregisterEventHandler();
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


bool OFileNotification::activate()
{
    emit triggered();
    _signal.activate();
    if ( !_multi ) stop();
    return true;
}


void OFileNotification::singleShot( const QString& path, QObject* receiver, const char* member, OFileNotificationType type )
{
    OFileNotification* ofn = new OFileNotification();
    ofn->_signal.connect( receiver, member );
    ofn->watch( path, true, type );
}


void OFileNotification::inotifyEventHandler()
{
    qWarning( "OFileNotification::__eventHandler(): reached." );

    char buffer[16384];
    size_t buffer_i;
    struct inotify_event *pevent, *event;
    ssize_t r;
    size_t event_size;
    int count = 0;

    r = ::read(_fd, buffer, 16384);

    if ( r <= 0 )
        return;

    buffer_i = 0;
    while ( buffer_i < r )
    {
            /* Parse events and queue them ! */
            pevent = (struct inotify_event *)&buffer[buffer_i];
            event_size = sizeof(struct inotify_event) + pevent->len;
            qDebug( "pevent->len = %d\n", pevent->len);

            OFileNotification* fn = notification_list[ pevent->wd ];
            if ( fn )
                fn->activate();
            else
                assert( false );

            //event = malloc(event_size);
            //memmove(event, pevent, event_size);
            //queue_enqueue(event, q);
            buffer_i += event_size;
            count++;
    }

    qDebug( "received %d events...", count );
    return;
}


bool OFileNotification::registerEventHandler()
{
    OFileNotification::_fd = ::open( INOTIFY_DEVICE, O_RDONLY );
    if ( OFileNotification::_fd < 0 )
    {
        qWarning( "OFileNotification::registerEventHandler(): couldn't register event handler: %s", strerror( errno ) );
        return false;
    }

    OFileNotification::_sn = new QSocketNotifier( _fd, QSocketNotifier::Read, this, "inotify event" );
    connect( OFileNotification::_sn, SIGNAL( activated(int) ), this, SLOT( inotifyEventHandler() ) );

    qDebug( "OFileNotification::registerEventHandler(): done" );
    return true;
}


void OFileNotification::unregisterEventHandler()
{
    if ( OFileNotification::_fd )
    ::close( OFileNotification::_fd );
    qDebug( "OFileNotification::unregisterEventHandler(): done" );
}


}
}
