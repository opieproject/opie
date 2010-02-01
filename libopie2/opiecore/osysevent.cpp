/*
                             This file is part of the Opie Project
                             Copyright (C) 2010 Paul Eggleton <bluelightning@bluelightning.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "osysevent.h"

#include <qstring.h>
#include <qfile.h>
#include <qtextstream.h>

#include <qtopia/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>
#include <qpe/alarmserver.h>

#include <opie2/odebug.h>
#include <opie2/oprocess.h>

using namespace Opie;
using namespace Opie::Core;

void OSysEvent::sendSysEvent( int eventtype )
{
#ifndef QT_NO_COP
    QString event;
    switch( eventtype ) {
        case SYSEVENT_POST_SYNC:
            event = "postsync";
            break;
        case SYSEVENT_PRE_SYS_TIME_CHANGE:
            event = "presystemtimechange";
            break;
        case SYSEVENT_POST_SYS_TIME_CHANGE:
            event = "postsystemtimechange";
            break;
        default:
            return;
    }

    QFile f( QPEApplication::qpeDir() + "etc/opie_sysevents.conf" );
    if( f.open( IO_ReadOnly ) ) {
        QTextStream ts(&f);
        while( !ts.atEnd() ) {
            QString s = ts.readLine();
            if(!s.startsWith("#")) {
                QStringList sl = QStringList::split( ' ', s );
                // Lines should be:
                // uniqueid event appname channel message
                // e.g.
                // datebook_postsync postsync datebook QPE/Application/datebook registerAllAlarms()
                if( sl.count() > 3 ) {
                    if( sl[1] == event ) {
                        QCString channel( sl[3] );
                        if( sl.count() > 4 ) {
                            QCString message( sl[4] );
                            QCopEnvelope e( channel, message );
                            // FIXME message args
                        }
                        else if( eventtype == SYSEVENT_PRE_SYS_TIME_CHANGE ) {
                            // Special event - just delete alarms for the specified channel
                            AlarmServer::deleteAlarm( QDateTime(), channel, QCString(), -1 );
                        }
                    }
                }
            }
        }
        f.close();
    }

#endif
}
