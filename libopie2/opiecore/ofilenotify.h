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
#if defined (__GNUC__) && (__GNUC__ < 3)
#define _GNU_SOURCE
#endif

/* QT */
#include <qobject.h>
#include <qsignal.h>
#include <qstring.h>

/* STD */
#include <signal.h>
#include <fcntl.h>

namespace Opie {
namespace Core {

/*======================================================================================
 * OFileNotificationType
 *======================================================================================*/

/**
 * @brief An enumerate for the different types of file notifications
 *
 * This enumerate provides a means to specify the type of events that you are interest in.
 * Valid values are:
 * <ul>
 *    <li>Access: The file was accessed (read)
 *    <li>Modify The file was modified (write,truncate)
 *    <li>Create = The file was created in the directory
 *    <li>Delete = The file was unlinked from directory
 *    <li>Rename = The file was renamed
 *    <li>Attrib = The file had its attributes changed (chmod,chown,chgrp)
 * </ul>
 *
 **/

enum OFileNotificationType { Single = 0x0000000,
                             Multi  = DN_MULTISHOT,
                             Access = DN_ACCESS,
                             Modify = DN_MODIFY,
                             Create = DN_CREATE,
                             Delete = DN_DELETE,
                             Rename = DN_RENAME,
                             Attrib = DN_ATTRIB };

/*======================================================================================
 * OFileNotification
 *======================================================================================*/

/**
 * @brief Represents a file notification
 *
 * This class allows to watch for events happening to files.
 * It uses the dnotify kernel interface which is a very efficient signalling interface.
 *
 * @see <file:///usr/src/linux/Documentation/dnotify.txt>
 *
 * @author Michael 'Mickey' Lauer <mickey@vanille.de>
 *
 **/

class OFileNotification : public QObject
{
  Q_OBJECT

  public:
    OFileNotification( QObject* parent = 0, const char* name = 0 );
    ~OFileNotification();
    /**
     * This static function calls a slot when an event with @a type happens to file @a path.
     *
     *  It is very convenient to use this function because you do not need to
     *  bother with a timerEvent or to create a local QTimer object.
     *
     *  Example:
     *  <pre>
     *
     *     #include <opie2/oapplication.h>
     *     #include <opie2/onitify.h>
     *     using namespace Opie::Core;
     *
     *     int main( int argc, char **argv )
     *     {
     *         OApplication a( argc, argv, "File Notification Example" );
     *         OFileNotification::singleShot( "/tmp/quit", &a, SLOT(quit()), Create );
     *         ... // create and show your widgets
     *         return a.exec();
     *     }
     *
     * This sample program automatically terminates when the file "/tmp/quite" has been created.
     *
     *
     * The @a receiver is the receiving object and the @a member is the slot.
     **/
    static void singleShot( const QString& path, QObject* receiver, const char* member, OFileNotificationType type = Modify );
    /**
     * Starts to watch for @a type changes to @a path. Set @a sshot to True if you want to be notified only once.
     * Note that in that case it may be more convenient to use @ref OFileNotification::singleShot() then.
     **/
    int start( const QString& path, bool sshot = false, OFileNotificationType type = Modify );
    /**
     * Stop watching for file events.
     **/
    void stop();
    /**
     * @returns the notification type as set by @ref start().
     **/
    OFileNotificationType type() const;
    /**
     * @returns the path to the file being watched by this instance.
     **/
    QString path() const;
    /**
     * @returns the UNIX file descriptor for the file being watched.
     **/
    int fileno() const;
    /**
     * @returns if a file is currently being watched.
     **/
    bool isActive() const;

  signals:
    /**
     * This signal is emitted if an event happens of the specified type happens to the file being watched.
     **/
    void triggered();

  protected:
    bool activate();
    virtual bool hasChanged();
    static bool registerSignalHandler();
    static void unregisterSignalHandler();
    static void __signalHandler( int sig, siginfo_t *si, void *data );

  private:
    QString _path;
    OFileNotificationType _type;
    QSignal _signal;
    int _fd;
    bool _active;
    struct stat _stat;
};

#if 0

class ODirectoryNotification : public OFileNotification
{

  public:
      virtual bool hasChanged() { return true; };
};

#endif

}
}

#endif

