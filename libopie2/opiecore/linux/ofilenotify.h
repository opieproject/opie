/*
                            This file is part of the Opie Project
             =.             Copyright (C) 2004-2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
           .=l.             Copyright (C) The Opie Team <opie-devel@lists.sourceforge.net>
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

#ifndef OFILENOTIFY_H
#define OFILENOTIFY_H
#if defined (__GNUC__) && (__GNUC__ < 3)
#define _GNU_SOURCE
#endif

#include "linux_inotify.h"

/* QT */
#include <qsocketnotifier.h>
#include <qsignal.h>
#include <qstring.h>
#include <qobject.h>
#include <qfile.h>

namespace Opie {
namespace Core {

class OFile : public QObject, public QFile
{
  Q_OBJECT

  public:
    OFile();
    OFile( const QString & name );
    virtual ~OFile();

  protected:
    virtual void connectNotify( const char* signal );
    virtual void disconnectNotify( const char* signal );

  signals:
    void accessed( const QString& );
    void modified( const QString& );
    void attributed( const QString& );
    void closed( const QString&, bool );
    void opened( const QString& );
    void deleted( const QString& );
    void unmounted( const QString& );
};

/*
    void movedTo( const QString&, const QString& );
    void movedFrom( const QString&, const QString& );
    void deletedSubdir( const QString&, const QString& );
    void deletedFile( const QString&, const QString& );
    void createdSubdir( const QString&, const QString& );
    void createdFile( const QString&, const QString& );
*/

class OFileNotificationEvent;

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
 *    <li>Attrib = The file had its attributes changed (chmod,chown,chgrp)
 *    <li>CloseWrite = Writable file was closed
 *    <li>CloseNoWrite = Unwritable file was closed
 *    <li>Open = File was opened
 *    <li>MovedFrom = File was moved from X
 *    <li>MovedTo = File was moved to Y
 *    <li>DeleteSubdir = Subdir was deleted
 *    <li>DeleteFile = Subfile was deleted
 *    <li>CreateSubdir = Subdir was created
 *    <li>CreateFile = Subfile was created
 *    <li>DeleteSelf = Self was deleted
 *    <li>Unmount = The backing filesystem was unmounted
 * </ul>
 *
 **/

enum OFileNotificationType
{
    Nothing         =   0,
    Access          =   IN_ACCESS,
    Modify          =   IN_MODIFY,
    Attrib          =   IN_ATTRIB,
    CloseWrite      =   IN_CLOSE_WRITE,
    CloseNoWrite    =   IN_CLOSE_NOWRITE,
    Open            =   IN_OPEN,
    MovedFrom       =   IN_MOVED_FROM,
    MovedTo         =   IN_MOVED_TO,
    DeleteSubdir    =   IN_DELETE_SUBDIR,
    DeleteFile      =   IN_DELETE_FILE,
    CreateSubdir    =   IN_CREATE_SUBDIR,
    CreateFile      =   IN_CREATE_FILE,
    DeleteSelf      =   IN_DELETE_SELF,
    Unmount         =   IN_UNMOUNT,
    _QueueOverflow  =   IN_Q_OVERFLOW,  /* Internal, don't use this in client code */
    _Ignored        =   IN_IGNORED,     /* Internal, don't use this in client code */
};

/*======================================================================================
 * OFileNotification
 *======================================================================================*/

/**
 * @brief Represents a file notification
 *
 * This class allows to watch for events happening to files.
 * It uses the inotify linux (2.6.x) kernel interface.
 *
 * @see http://www.kernel.org/pub/linux/kernel/people/rml/inotify/
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
     *     #include <opie2/ofilenotify.h>
     *     using namespace Opie::Core;
     *
     *     int main( int argc, char **argv )
     *     {
     *         OApplication a( argc, argv, "File Notification Example" );
     *         OFileNotification::singleShot( "/tmp/quit", &a, SLOT(quit()), Access );
     *         ... // create and show your widgets
     *         return a.exec();
     *     }
     *  </pre>
     *
     * This sample program automatically terminates when the file "/tmp/quit" has been accessed.
     *
     *
     * The @a receiver is the receiving object and the @a member is the slot.
     **/
    static bool singleShot( const QString& path, QObject* receiver, const char* member, OFileNotificationType type = Modify );
    /**
     * Starts to watch for @a type changes to @a path. Set @a sshot to True if you want to be notified only once.
     * Note that in that case it may be more convenient to use @ref OFileNotification::singleShot() then.
     **/
    int watch( const QString& path, bool sshot = false, OFileNotificationType type = Modify );
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
     * @returns if the notification is single-shot
     */
    bool isSingleShot() const;
    /**
     * @returns if a file is currently being watched.
     **/
    bool isActive() const;
    /**
      * @internal
      */
    int startWatching( const QString& path, bool sshot = false, OFileNotificationType type = Modify );

  signals:
    void triggered( const QString&, unsigned int, const QString& );
    void accessed( const QString& );
    void modified( const QString& );
    void attributed( const QString& );
    void closed( const QString&, bool );
    void opened( const QString& );
    void movedTo( const QString&, const QString& );
    void movedFrom( const QString&, const QString& );
    void deletedSubdir( const QString&, const QString& );
    void deletedFile( const QString&, const QString& );
    void createdSubdir( const QString&, const QString& );
    void createdFile( const QString&, const QString& );
    void deleted( const QString& );
    void unmounted( const QString& );

  protected:
    bool activate( const OFileNotificationEvent* e );

  private slots:
    void inotifyEventHandler();

  private:
    bool registerEventHandler();
    void unregisterEventHandler();

    QString _path;
    OFileNotificationType _type;
    QSignal _signal;
    bool _active;
    bool _multi;
    static QSocketNotifier* _sn;
    int _wd; // inotify watch descriptor
    static int _fd; // inotify device descriptor

    friend class OFileNotificationEvent;
};

/*======================================================================================
 * ODirNotification
 *======================================================================================*/

/**
 * @brief Represents a directory notification
 *
 * This class allows to watch for events happening to directories
 * It uses the OFileNotification class and (for recursive watches)
 * implements the Love-Trowbridge recursive directory scanning algorithm.
 *
 * @see http://www.kernel.org/pub/linux/kernel/people/rml/inotify/
 *
 * @author Michael 'Mickey' Lauer <mickey@vanille.de>
 *
 **/

class ODirNotification : public QObject
{
  Q_OBJECT

  public:
    ODirNotification( QObject* parent = 0, const char* name = 0 );
    ~ODirNotification();
    /**
     * Starts to watch for @a type changes to @a path. Recurse @a recurse levels down the filesystem tree,
     * use 0 for no recursion and -1 for unlimited recursion.
     * Set @a sshot to True if you want to be notified only once.
     **/
    int watch( const QString& path, bool sshot = false, OFileNotificationType type = Modify, int recurse = 0 );

  signals:
    /**
     * This signal is emitted if an event happens of the specified type happens to the directory being watched.
     **/
    void triggered( const QString&, unsigned int, const QString& );
    void accessed( const QString& );
    void modified( const QString& );
    void attributed( const QString& );
    void closed( const QString&, bool );
    void opened( const QString& );
    void movedTo( const QString&, const QString& );
    void movedFrom( const QString&, const QString& );
    void deletedSubdir( const QString&, const QString& );
    void deletedFile( const QString&, const QString& );
    void createdSubdir( const QString&, const QString& );
    void createdFile( const QString&, const QString& );
    void deleted( const QString& );
    void unmounted( const QString& );

  private slots:
    void subdirCreated( const QString&, const QString& );

  private:
    OFileNotification* _topfilenotification;
    OFileNotificationType _type;
    int _depth;
};

/*======================================================================================
 * OFileNotificationEvent
 *======================================================================================*/

class OFileNotificationEvent
{
  public:
    OFileNotificationEvent( OFileNotification* parent, int wd, unsigned int mask, unsigned int cookie, const QString& name );
    ~OFileNotificationEvent();
    OFileNotification* parent() const { return _parent; };
    int descriptor() const            { return _wd; };
    unsigned int mask() const         { return _mask; };
    unsigned int cookie() const       { return _cookie; };
    QString name() const              { return _name; };
    void activate()                   { _parent->activate( this ); };

  private:
    OFileNotification* _parent;
    int _wd;
    unsigned int _mask;
    unsigned int _cookie;
    QString _name;
};


}
}

#endif

