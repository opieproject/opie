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

#ifndef OFILENOTIFY_H
#define OFILENOTIFY_H

/* QT */
#include <qobject.h>
#include <qsignal.h>
#include <qstring.h>

/* STD */
#include <signal.h>
#include <fcntl.h>

namespace Opie {
namespace Core {

enum OFileNotificationType { Single = 0x0000000,
                             Multi  = DN_MULTISHOT,
                             Access = DN_ACCESS,
                             Modify = DN_MODIFY,
                             Create = DN_CREATE,
                             Delete = DN_DELETE,
                             Rename = DN_RENAME,
                             Attrib = DN_ATTRIB };

class OFileNotification : public QObject
{
  Q_OBJECT

  public:
    OFileNotification( QObject* parent = 0, const char* name = 0 );
    ~OFileNotification();

    static void singleShot( const QString& path, QObject* receiver, const char* member, OFileNotificationType type = Modify );

    int start( const QString& path, bool sshot = false, OFileNotificationType type = Modify );
    void stop();

    OFileNotificationType type() const;
    QString path() const;
    int fileno() const;
    bool isActive() const;

  signals:
    void triggered();

  protected:
    void activate();
    static bool registerSignalHandler();
    static void unregisterSignalHandler();
    static void __signalHandler( int sig, siginfo_t *si, void *data );

  private:
    QString _path;
    OFileNotificationType _type;
    QSignal _signal;
    int _fd;
    bool _active;
};

}
}

#endif

