/****************************************************************************
** $Id: qsettings.cpp,v 1.1 2002-11-01 00:10:44 kergoth Exp $
**
** Implementation of QSettings class
**
** Created: 2000.06.26
**
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the tools module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qplatformdefs.h"

// POSIX Large File Support redefines open -> open64
static inline int qt_open( const char *pathname, int flags, mode_t mode )
{ return ::open( pathname, flags, mode ); }
#if defined(open)
# undef open
#endif

// POSIX Large File Support redefines truncate -> truncate64
#if defined(truncate)
# undef truncate
#endif

#include "qsettings.h"

#ifndef QT_NO_SETTINGS

#include "qdir.h"
#include "qfile.h"
#include "qfileinfo.h"
#include "qmap.h"
#include "qtextstream.h"
#include "qregexp.h"
#include <private/qsettings_p.h>
#include <errno.h>

/*!
  \class QSettings
  \brief The QSettings class provides persistent platform-independent application settings.

  \ingroup io
  \ingroup misc
  \mainclass

  On Unix systems, QSettings uses text files to store settings. On Windows
  systems, QSettings uses the system registry.  On Mac OS X, QSettings will
  behave as on Unix, and store to text files.

  Each setting comprises an identifying key and the data associated with
  the key. A key is a unicode string which consists of \e two or more
  subkeys. A subkey is a slash, '/', followed by one or more unicode
  characters (excluding slashes, newlines, carriage returns and equals,
  '=', signs). The associated data, called the entry or value, may be a
  boolean, an integer, a double, a string or a list of strings. Entry
  strings may contain any unicode characters.

  If you want to save and restore the entire desktop's settings, i.e.
  which applications are running, use QSettings to save the settings
  for each individual application and QSessionManager to save the
  desktop's session.

    Example settings:
    \code
    /MyCompany/MyApplication/background color
    /MyCompany/MyApplication/foreground color
    /MyCompany/MyApplication/geometry/x
    /MyCompany/MyApplication/geometry/y
    /MyCompany/MyApplication/geometry/width
    /MyCompany/MyApplication/geometry/height
    /MyCompany/MyApplication/recent files/1
    /MyCompany/MyApplication/recent files/2
    /MyCompany/MyApplication/recent files/3
    \endcode
    Each line above is a complete key, made up of subkeys.

    A typical usage pattern for application startup:
    \code
    QSettings settings;
    settings.insertSearchPath( QSettings::Windows, "/MyCompany" );
    // No search path needed for Unix; see notes further on.
    // Use default values if the keys don't exist
    QString bgColor = settings.readEntry( "/MyApplication/background color", "white" );
    int width = settings.readNumEntry( "/MyApplication/geometry/width", 640 );
    // ...
    \endcode

    A typical usage pattern for application exit or 'save preferences':
    \code
    QSettings settings;
    settings.insertSearchPath( QSettings::Windows, "/MyCompany" );
    // No search path needed for Unix; see notes further on.
    settings.writeEntry( "/MyApplication/background color", bgColor );
    settings.writeEntry( "/MyApplication/geometry/width", width );
    // ...
    \endcode

    You can get a list of entry-holding keys by calling entryList(), and
    a list of key-holding keys using subkeyList().

    \code
    QStringList keys = entryList( "/MyApplication" );
    // keys contains 'background color' and 'foreground color'.

    QStringList keys = entryList( "/MyApplication/recent files" );
    // keys contains '1', '2' and '3'.

    QStringList subkeys = subkeyList( "/MyApplication" );
    // subkeys contains 'geometry' and 'recent files'

    QStringList subkeys = subkeyList( "/MyApplication/recent files" );
    // subkeys is empty.
    \endcode

    If you wish to use a different search path call insertSearchPath()
    as often as necessary to add your preferred paths. Call
    removeSearchPath() to remove any unwanted paths.

    Since settings for Windows are stored in the registry there are size
    limits as follows:
    \list
    \i A subkey may not exceed 255 characters.
    \i An entry's value may not exceed 16,300 characters.
    \i All the values of a key (for example, all the 'recent files'
    subkeys values), may not exceed 65,535 characters.
    \endlist

    These limitations are not enforced on Unix.

    \section1 Notes for Unix Applications

    There is no universally accepted place for storing application
    settings under Unix. In the examples the settings file will be
    searched for in the following directories:
    \list 1
    \i INSTALL/etc/settings
    \i /opt/MyCompany/share/etc
    \i /opt/MyCompany/share/MyApplication/etc
    \i $HOME/.qt
    \endlist
    When reading settings the files are searched in the order shown
    above, with later settings overriding earlier settings. Files for
    which the user doesn't have read permission are ignored. When saving
    settings QSettings works in the order shown above, writing
    to the first settings file for which the user has write permission.
    (\c INSTALL is the directory where Qt was installed.  This can be
    modified by using the configure script's -prefix argument )

    If you want to put the settings in a particular place in the
    filesystem you could do this:
    \code
    settings.insertSearchPath( QSettings::Unix, "/opt/MyCompany/share" );
    \endcode

    But in practice you may prefer not to use a search path for Unix.
    For example the following code:
    \code
    settings.writeEntry( "/MyApplication/geometry/width", width );
    \endcode
    will end up writing the "geometry/width" setting to the file
    \c{$HOME/.qt/myapplicationrc} (assuming that the application is
    being run by an ordinary user, i.e. not by root).

    For cross-platform applications you should ensure that the Windows
    size limitations are not exceeded.
*/

/*!
    \enum QSettings::System

    \value Mac Macintosh execution environments
    \value Unix Mac OS X, Unix, Linux and Unix-like execution environments
    \value Windows Windows execution environments
*/

/*!
    \enum QSettings::Format

    \value Native Store the settings in a platform dependent location
    \value Ini Store the settings in a text file
*/

/*!
    \enum QSettings::Scope

    \value Global Save settings as global as possible
    \value User Save settings in user space
*/

#if defined(Q_OS_UNIX)
typedef int HANDLE;
#define Q_LOCKREAD F_RDLCK
#define Q_LOCKWRITE F_WRLCK
/*
  Locks the file specified by name.  The lockfile is created as a
  hidden file in the same directory as the target file, with .lock
  appended to the name. For example, "/etc/settings/onerc" uses a
  lockfile named "/etc/settings/.onerc.lock".  The type argument
  controls the type of the lock, it can be either F_RDLCK for a read
  lock, or F_WRLCK for a write lock.

  A file descriptor for the lock file is returned, and should be
  closed with closelock() when the lock is no longer needed.
 */
static HANDLE openlock( const QString &name, int type )
{
    QFileInfo info( name );
    // lockfile should be hidden, and never removed
    QString lockfile = info.dirPath() + "/." + info.fileName() + ".lock";

    // open the lockfile
    HANDLE fd = qt_open( QFile::encodeName( lockfile ),
			 O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );

    if ( fd < 0 ) {
 	// failed to open the lock file, most likely because of permissions
	return fd;
    }

    struct flock fl;
    fl.l_type = type;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    if ( fcntl( fd, F_SETLKW, &fl ) == -1 ) {
	// the lock failed, so we should fail silently, so that people
	// using filesystems that do not support locking don't see
	// numerous warnings about a failed lock
	close( fd );
	fd = -1;
    }

    return fd;
}

/*
  Closes the lock file specified by fd.  fd is the file descriptor
  returned by the openlock() function.
*/
static void closelock( HANDLE fd )
{
    if ( fd < 0 ) {
	// the lock file is not open
	return;
    }

    struct flock fl;
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    // ignore the return value, so that the unlock fails silently
    (void) fcntl( fd, F_SETLKW, &fl );

    close( fd );
}
#elif defined(Q_WS_WIN)
#define Q_LOCKREAD 1
#define Q_LOCKWRITE 2

static HANDLE openlock( const QString &name, int /*type*/ )
{
    if ( !QFile::exists( name ) )
	return 0;

    return 0;

    HANDLE fd = 0;

    QT_WA( {
	fd = CreateFileW( (TCHAR*)name.ucs2(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    } , {
	fd = CreateFileA( name.local8Bit(), GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    } );

    if ( !LockFile( fd, 0, 0, -1, -1 ) ) {
#ifdef QT_CHECK_STATE
	qWarning( "QSettings: openlock failed!" );
#endif
    }
    return fd;
}

void closelock( HANDLE fd )
{
    if ( !fd )
	return;

    if ( !UnlockFile( fd, 0, 0, -1, -1 ) ) {
#ifdef QT_CHECK_STATE
	qWarning( "QSettings: closelock failed!");
#endif
    }
    CloseHandle( fd );
}
#endif


QSettingsGroup::QSettingsGroup()
    : modified(FALSE)
{
}




void QSettingsHeading::read(const QString &filename)
{
    if (! QFileInfo(filename).exists())
	return;

    HANDLE lockfd = openlock( filename, Q_LOCKREAD );

    QFile file(filename);
    if (! file.open(IO_ReadOnly)) {
#if defined(QT_CHECK_STATE)
	qWarning("QSettings: failed to open file '%s'", filename.latin1());
#endif
	return;
    }

    git = end();

    QTextStream stream(&file);
    stream.setEncoding(QTextStream::UnicodeUTF8);
    while (! stream.atEnd())
	parseLine(stream);

    git = end();

    file.close();

    closelock( lockfd );
}


void QSettingsHeading::parseLine(QTextStream &stream)
{
    QString line = stream.readLine();
    if (line.isEmpty())
	// empty line... we'll allow it
	return;

    if (line[0] == QChar('#'))
	// commented line
	return;

    if (line[0] == QChar('[')) {
	QString gname = line;

	gname = gname.remove(0, 1);
	if (gname[(int)gname.length() - 1] == QChar(']'))
	    gname = gname.remove(gname.length() - 1, 1);

	git = find(gname);
	if (git == end())
	    git = replace(gname, QSettingsGroup());
    } else {
	if (git == end()) {
#if defined(QT_CHECK_STATE)
	    qWarning("QSettings: line '%s' out of group", line.latin1());
#endif
	    return;
	}

	int i = line.find('=');
       	if (i == -1) {
#if defined(QT_CHECK_STATE)
	    qWarning("QSettings: malformed line '%s' in group '%s'",
		     line.latin1(), git.key().latin1());
#endif
	    return;
	} else {
	    QString key, value;
	    key = line.left(i);
	    value = "";
	    bool esc=TRUE;
	    i++;
	    while (esc) {
		esc = FALSE;
		for ( ; i < (int)line.length(); i++ ) {
		    if ( esc ) {
			if ( line[i] == 'n' )
			    value.append('\n'); // escaped newline
			else if ( line[i] == '0' )
			    value = QString::null; // escaped empty string
			else
			    value.append(line[i]);
			esc = FALSE;
		    } else if ( line[i] == '\\' )
			esc = TRUE;
		    else
			value.append(line[i]);
		}
		if ( esc ) {
		    // Backwards-compatiblity...
		    // still escaped at EOL - manually escaped "newline"
		    if (stream.atEnd()) {
#if defined(QT_CHECK_STATE)
			qWarning("QSettings: reached end of file, expected continued line");
#endif
			break;
		    }
		    value.append('\n');
		    line = stream.readLine();
		    i = 0;
		}
	    }

	    (*git).insert(key, value);
	}
    }
}

#ifdef Q_WS_WIN // for homedirpath reading from registry
#include "qt_windows.h"
#include "qlibrary.h"

#ifndef CSIDL_APPDATA
#define CSIDL_APPDATA                   0x001a        // <user name>\Application Data
#endif
#ifndef CSIDL_COMMON_APPDATA
#define CSIDL_COMMON_APPDATA            0x0023        // All Users\Application Data
#endif

#endif

QSettingsPrivate::QSettingsPrivate( QSettings::Format format )
    : groupDirty( TRUE ), modified(FALSE), globalScope(TRUE)
{
#if defined(Q_WS_WIN) || defined(Q_OS_MAC)
    if ( format != QSettings::Ini )
	return;
#endif

    QString appSettings(QDir::homeDirPath() + "/.qt/");
    QString defPath;
#ifdef Q_WS_WIN
#ifdef Q_OS_TEMP
	TCHAR path[MAX_PATH];
	SHGetSpecialFolderPath( 0, path, CSIDL_APPDATA, FALSE );
	appSettings  = QString::fromUcs2( path );
	SHGetSpecialFolderPath( 0, path, CSIDL_COMMON_APPDATA, FALSE );
	defPath = QString::fromUcs2( path );
#else
    QLibrary library( "shell32" );
    library.setAutoUnload( FALSE );
    QT_WA( {
	typedef BOOL (WINAPI*GetSpecialFolderPath)(HWND, LPTSTR, int, BOOL);
	GetSpecialFolderPath SHGetSpecialFolderPath = (GetSpecialFolderPath)library.resolve( "SHGetSpecialFolderPathW" );
	if ( SHGetSpecialFolderPath ) {
	    TCHAR path[MAX_PATH];
	    SHGetSpecialFolderPath( 0, path, CSIDL_APPDATA, FALSE );
	    appSettings  = QString::fromUcs2( (ushort*)path );
	    SHGetSpecialFolderPath( 0, path, CSIDL_COMMON_APPDATA, FALSE );
	    defPath = QString::fromUcs2( (ushort*)path );
	}
    } , {
	typedef BOOL (WINAPI*GetSpecialFolderPath)(HWND, char*, int, BOOL);
	GetSpecialFolderPath SHGetSpecialFolderPath = (GetSpecialFolderPath)library.resolve( "SHGetSpecialFolderPathA" );
	if ( SHGetSpecialFolderPath ) {
	    char path[MAX_PATH];
	    SHGetSpecialFolderPath( 0, path, CSIDL_APPDATA, FALSE );
	    appSettings = QString::fromLocal8Bit( path );
	    SHGetSpecialFolderPath( 0, path, CSIDL_COMMON_APPDATA, FALSE );
	    defPath = QString::fromLocal8Bit( path );
	}
    } );
#endif // Q_OS_TEMP
#else
// for now
#define QSETTINGS_DEFAULT_PATH_SUFFIX "/etc/settings"

    defPath = qInstallPath();
    defPath += QSETTINGS_DEFAULT_PATH_SUFFIX;
#endif
    QDir dir(appSettings);
    if (! dir.exists()) {
	if (! dir.mkdir(dir.path()))
#if defined(QT_CHECK_STATE)
	    qWarning("QSettings: error creating %s", dir.path().latin1());
#endif
    }

    if ( !!defPath )
	searchPaths.append(defPath);
    searchPaths.append(dir.path());
}

QSettingsPrivate::~QSettingsPrivate()
{
}

QSettingsGroup QSettingsPrivate::readGroup()
{
    QSettingsHeading hd;
    QSettingsGroup grp;

    QMap<QString,QSettingsHeading>::Iterator headingsit = headings.find(heading);
    if (headingsit != headings.end())
	hd = *headingsit;

    QSettingsHeading::Iterator grpit = hd.find(group);
    if (grpit == hd.end()) {
	QStringList::Iterator it = searchPaths.begin();
	while (it != searchPaths.end()) {
	    QString filebase = heading.lower().replace(QRegExp("\\s+"), "_");
	    QString fn((*it++) + "/" + filebase + "rc");
	    if (! hd.contains(fn + "cached")) {
		hd.read(fn);
		hd.insert(fn + "cached", QSettingsGroup());
	    }
	}

	headings.replace(heading, hd);

	grpit = hd.find(group);
	if (grpit != hd.end())
	    grp = *grpit;
    } else if (hd.count() != 0)
	grp = *grpit;

    return grp;
}


void QSettingsPrivate::removeGroup(const QString &key)
{
    QSettingsHeading hd;
    QSettingsGroup grp;
    bool found = FALSE;

    QMap<QString,QSettingsHeading>::Iterator headingsit = headings.find(heading);
    if (headingsit != headings.end())
	hd = *headingsit;

    QSettingsHeading::Iterator grpit = hd.find(group);
    if (grpit == hd.end()) {
	QStringList::Iterator it = searchPaths.begin();
	while (it != searchPaths.end()) {
	    QString filebase = heading.lower().replace(QRegExp("\\s+"), "_");
	    QString fn((*it++) + "/" + filebase + "rc");
	    if (! hd.contains(fn + "cached")) {
		hd.read(fn);
		hd.insert(fn + "cached", QSettingsGroup());
	    }
	}

	headings.replace(heading, hd);

	grpit = hd.find(group);
	if (grpit != hd.end()) {
	    found = TRUE;
	    grp = *grpit;
	}
    } else if (hd.count() != 0) {
	found = TRUE;
	grp = *grpit;
    }

    if (found) {
	grp.remove(key);

	if (grp.count() > 0)
	    hd.replace(group, grp);
	else
	    hd.remove(group);

	if (hd.count() > 0)
	    headings.replace(heading, hd);
	else
	    headings.remove(heading);

	modified = TRUE;
    }
}


void QSettingsPrivate::writeGroup(const QString &key, const QString &value)
{
    QSettingsHeading hd;
    QSettingsGroup grp;

    QMap<QString,QSettingsHeading>::Iterator headingsit = headings.find(heading);
    if (headingsit != headings.end())
	hd = *headingsit;

    QSettingsHeading::Iterator grpit = hd.find(group);
    if (grpit == hd.end()) {
	QStringList::Iterator it = searchPaths.begin();
	while (it != searchPaths.end()) {
	    QString filebase = heading.lower().replace(QRegExp("\\s+"), "_");
	    QString fn((*it++) + "/" + filebase + "rc");
	    if (! hd.contains(fn + "cached")) {
		hd.read(fn);
		hd.insert(fn + "cached", QSettingsGroup());
	    }
	}

	headings.replace(heading, hd);

	grpit = hd.find(group);
	if (grpit != hd.end())
	    grp = *grpit;
    } else if (hd.count() != 0)
	grp = *grpit;

    grp.modified = TRUE;
    grp.replace(key, value);
    hd.replace(group, grp);
    headings.replace(heading, hd);

    modified = TRUE;
}


QDateTime QSettingsPrivate::modificationTime()
{
    QSettingsHeading hd = headings[heading];
    QSettingsGroup grp = hd[group];

    QDateTime datetime;

    QStringList::Iterator it = searchPaths.begin();
    while (it != searchPaths.end()) {
	QFileInfo fi((*it++) + "/" + heading + "rc");
	if (fi.exists() && fi.lastModified() > datetime)
	    datetime = fi.lastModified();
    }

    return datetime;
}

static bool verifyKey( const QString &key )
{
    if ( key.isEmpty() || key[0] != '/' || key.contains( QRegExp("[=\\\\r\\\\n" ) ) )
	return FALSE;
    return TRUE;
}

static inline QString groupKey( const QString &group, const QString &key )
{
    if ( group.endsWith( "/" ) || key.startsWith( "/" ) )
	return group + key;
    return group + "/" + key;
}

/*!
  Inserts \a path into the settings search path. The semantics of \a
  path depends on the system \a s.

  When \a s is \e Windows and the execution environment is \e not
  Windows the function does nothing. Similarly when \a s is \e Unix and
  the execution environment is \e not Unix the function does nothing.

  When \a s is \e Windows, and the execution environment is Windows, the
  search path list will be used as the first subfolder of the "Software"
  folder in the registry.

  When reading settings the folders are searched forwards from the
  first folder (listed below) to the last, returning the first
  settings found, and ignoring any folders for which the user doesn't 
  have read permission.
  \list 1
  \i HKEY_CURRENT_USER/Software/MyCompany/MyApplication
  \i HKEY_LOCAL_MACHINE/Software/MyCompany/MyApplication
  \i HKEY_CURRENT_USER/Software/MyApplication
  \i HKEY_LOCAL_MACHINE/Software/MyApplication
  \endlist

  \code
  QSettings settings;
  settings.insertSearchPath( QSettings::Windows, "/MyCompany" );
  settings.writeEntry( "/MyApplication/Tip of the day", TRUE );
  \endcode
  The code above will write the subkey "Tip of the day" into the \e
  first of the registry folders listed below that is found and for
  which the user has write permission.
  \list 1
  \i HKEY_LOCAL_MACHINE/Software/MyCompany/MyApplication
  \i HKEY_CURRENT_USER/Software/MyCompany/MyApplication
  \i HKEY_LOCAL_MACHINE/Software/MyApplication
  \i HKEY_CURRENT_USER/Software/MyApplication
  \endlist
  If a setting is found in the HKEY_CURRENT_USER space, this setting
  is overwritten independently of write permissions in the 
  HKEY_LOCAL_MACHINE space.

  When \a s is \e Unix, and the execution environment is Unix, the
  search path list will be used when trying to determine a suitable
  filename for reading and writing settings files. By default, there are
  two entries in the search path:

  \list 1
  \i INSTALL/etc - where \c INSTALL is the directory where Qt was installed.
  \i $HOME/.qt/ - where \c $HOME is the user's home directory.
  \endlist

  All insertions into the search path will go before $HOME/.qt/.
  For example:
  \code
  QSettings settings;
  settings.insertSearchPath( QSettings::Unix, "/opt/MyCompany/share/etc" );
  settings.insertSearchPath( QSettings::Unix, "/opt/MyCompany/share/MyApplication/etc" );
  // ...
  \endcode
  Will result in a search path of:
  \list 1
  \i INSTALL/etc
  \i /opt/MyCompany/share/etc
  \i /opt/MyCompany/share/MyApplication/etc
  \i $HOME/.qt
  \endlist
    When reading settings the files are searched in the order shown
    above, with later settings overriding earlier settings. Files for
    which the user doesn't have read permission are ignored. When saving
    settings QSettings works in the order shown above, writing
    to the first settings file for which the user has write permission.

  Settings under Unix are stored in files whose names are based on the
  first subkey of the key (not including the search path). The algorithm
  for creating names is essentially: lowercase the first subkey, replace
  spaces with underscores and add 'rc', e.g.
  <tt>/MyCompany/MyApplication/background color</tt> will be stored in
  <tt>myapplicationrc</tt> (assuming that <tt>/MyCompany</tt> is part of
  the search path).

  \sa removeSearchPath()

*/
void QSettings::insertSearchPath( System s, const QString &path)
{
#if defined(Q_WS_WIN) || defined(Q_OS_MAC)
    if ( d->sysd ) {
	d->sysInsertSearchPath( s, path );
	return;
    }
#endif

    if ( !verifyKey( path ) ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QSettings::insertSearchPath: Invalid key: '%s'", path.isNull() ? "(null)" : path.latin1() );
#endif
	return;
    }

#if defined(Q_WS_WIN) || defined(Q_OS_MAC)
    if ( d->sysd && s != Unix ) {
#else
    if ( s != Unix ) {
#endif
#ifdef Q_OS_MAC
	if(s != Mac) //mac is respected on the mac as well
#endif
	    return;
    }

    QStringList::Iterator it = d->searchPaths.find(d->searchPaths.last());
    if (it != d->searchPaths.end()) {
	d->searchPaths.insert(it, path);
    }
}


/*!
  Removes all occurrences of \a path (using exact matching) from the
  settings search path for system \a s. Note that the default search
  paths cannot be removed.

  \sa insertSearchPath()
*/
void QSettings::removeSearchPath( System s, const QString &path)
{
    if ( !verifyKey( path ) ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QSettings::insertSearchPath: Invalid key: '%s'", path.isNull() ? "(null)" : path.latin1() );
#endif
	return;
    }

#ifdef Q_WS_WIN
    if ( d->sysd ) {
	d->sysRemoveSearchPath( s, path );
	return;
    }
#endif
#if defined(Q_WS_WIN) || defined(Q_OS_MAC)
    if ( d->sysd && s != Unix ) {
#else
    if ( s != Unix ) {
#endif
#ifdef Q_OS_MAC
	if(s != Mac) //mac is respected on the mac as well
#endif
	    return;
    }

    if (path == d->searchPaths.first() || path == d->searchPaths.last())
	return;

    d->searchPaths.remove(path);
}


/*!
  Creates a settings object.
*/
QSettings::QSettings()
{
    d = new QSettingsPrivate( Native );
    Q_CHECK_PTR(d);

#if defined(Q_WS_WIN) || defined(Q_OS_MAC)
    d->sysd = 0;
    d->sysInit();
#endif
}

/*!
  Creates a settings object. If \a format is 'Ini' the settings will
  be stored in a text file, using the Unix strategy (see above). If \a format
  is 'Native', the settings will be stored in a platform specific way
  (ie. the Windows registry).
*/
QSettings::QSettings( Format format )
{
    d = new QSettingsPrivate( format );
    Q_CHECK_PTR(d);

#if defined(Q_WS_WIN) || defined(Q_OS_MAC)
    d->sysd = 0;
    if ( format == Native )
	d->sysInit();
#else
    Q_UNUSED(format);
#endif
}

/*!
  Destroys the settings object.  All modifications made to the settings
  will automatically be saved.

*/
QSettings::~QSettings()
{
    sync();

#if defined(Q_WS_WIN) || defined(Q_OS_MAC)
    if ( d->sysd )
	d->sysClear();
#endif

    delete d;
}


/*! \internal
  Writes all modifications to the settings to disk.  If any errors are
  encountered, this function returns FALSE, otherwise it will return TRUE.
*/
bool QSettings::sync()
{
#if defined(Q_WS_WIN) || defined(Q_OS_MAC)
    if ( d->sysd )
	return d->sysSync();
#endif
    if (! d->modified)
	// fake success
	return TRUE;

    bool success = TRUE;
    QMap<QString,QSettingsHeading>::Iterator it = d->headings.begin();

    while (it != d->headings.end()) {
	// determine filename
	QSettingsHeading hd(*it);
	QSettingsHeading::Iterator hdit = hd.begin();
	QFile file;

	QStringList::Iterator pit = d->searchPaths.begin();
	while (pit != d->searchPaths.end()) {
	    QString filebase = it.key().lower().replace(QRegExp("\\s+"), "_");
	    QFileInfo di(*pit);
	    QFileInfo fi((*pit++) + "/" + filebase + "rc");

	    if ((fi.exists() && fi.isFile() && fi.isWritable()) ||
		(! fi.exists() && di.isDir() && di.isWritable())) {
		file.setName(fi.filePath());
		break;
	    }
	}

	it++;

	if (file.name().isNull() || file.name().isEmpty()) {

#ifdef QT_CHECK_STATE
	    qWarning("QSettings::sync: filename is null/empty");
#endif // QT_CHECK_STATE

	    success = FALSE;
	    continue;
	}

	HANDLE lockfd = openlock( file.name(), Q_LOCKWRITE );

	if (! file.open(IO_WriteOnly)) {

#ifdef QT_CHECK_STATE
	    qWarning("QSettings::sync: failed to open '%s' for writing",
		     file.name().latin1());
#endif // QT_CHECK_STATE

	    success = FALSE;
	    continue;
	}

	// spew to file
	QTextStream stream(&file);
	stream.setEncoding(QTextStream::UnicodeUTF8);

	while (hdit != hd.end()) {
	    if ((*hdit).count() > 0) {
		stream << "[" << hdit.key() << "]" << endl;

		QSettingsGroup grp(*hdit);
		QSettingsGroup::Iterator grpit = grp.begin();

		while (grpit != grp.end()) {
		    QString v = grpit.data();
		    if ( v.isNull() ) {
			v = "\\0"; // escape null string
		    } else {
			v.replace("\\", "\\\\"); // escape backslash
			v.replace("\n", "\\n"); // escape newlines
		    }

		    stream << grpit.key() << "=" << v << endl;
		    grpit++;
		}

		stream << endl;
	    }

	    hdit++;
	}

	if (file.status() != IO_Ok) {

#ifdef QT_CHECK_STATE
	    qWarning("QSettings::sync: error at end of write");
#endif // QT_CHECK_STATE

	    success = FALSE;
	}

	file.close();

	closelock( lockfd );
    }

    d->modified = FALSE;

    return success;
}


/*!
  \fn bool QSettings::readBoolEntry(const QString &key, bool def, bool *ok ) const

  Reads the entry specified by \a key, and returns a bool, or the
  default value, \a def, if the entry couldn't be read.
  If \a ok is non-null, *ok is set to TRUE if the key was read, FALSE
  otherwise.

  \sa readEntry(), readNumEntry(), readDoubleEntry(), writeEntry(), removeEntry()
*/

/*!
    \internal
*/
bool QSettings::readBoolEntry(const QString &key, bool def, bool *ok )
{
    if ( !verifyKey( key ) ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QSettings::readBoolEntry: Invalid key: '%s'", key.isNull() ? "(null)" : key.latin1() );
#endif
    	if ( ok )
	    *ok = FALSE;

	return def;
    }

    QString theKey = groupKey( group(), key );
#if defined(Q_WS_WIN) || defined(Q_OS_MAC)
    if ( d->sysd )
	return d->sysReadBoolEntry( theKey, def, ok );
#endif

    QString value = readEntry( theKey, ( def ? "true" : "false" ), ok );

    if (value.lower() == "true")
	return TRUE;
    else if (value.lower() == "false")
	return FALSE;
    else if (value == "1")
	return TRUE;
    else if (value == "0")
	return FALSE;

    if (! value.isEmpty())
	qWarning("QSettings::readBoolEntry: '%s' is not 'true' or 'false'",
		 value.latin1());
    if ( ok )
	*ok = FALSE;
    return def;
}


/*!
    \fn double QSettings::readDoubleEntry(const QString &key, double def, bool *ok ) const

  Reads the entry specified by \a key, and returns a double, or the
  default value, \a def, if the entry couldn't be read.
  If \a ok is non-null, *ok is set to TRUE if the key was read, FALSE
  otherwise.

  \sa readEntry(), readNumEntry(), readBoolEntry(), writeEntry(), removeEntry()
*/

/*!
    \internal
*/
double QSettings::readDoubleEntry(const QString &key, double def, bool *ok )
{
    if ( !verifyKey( key ) ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QSettings::readDoubleEntry: Invalid key: '%s'", key.isNull() ? "(null)" : key.latin1() );
#endif
    	if ( ok )
	    *ok = FALSE;

	return def;
    }

    QString theKey = groupKey( group(), key );
#if defined(Q_WS_WIN) || defined(Q_OS_MAC)
    if ( d->sysd )
	return d->sysReadDoubleEntry( theKey, def, ok );
#endif

    QString value = readEntry( theKey, QString::number(def), ok );
    bool conv_ok;
    double retval = value.toDouble( &conv_ok );
    if ( conv_ok )
	return retval;
    if ( ! value.isEmpty() )
	qWarning( "QSettings::readDoubleEntry: '%s' is not a number",
		  value.latin1() );
    if ( ok )
	*ok = FALSE;
    return def;
}


/*!
    \fn int QSettings::readNumEntry(const QString &key, int def, bool *ok ) const

  Reads the entry specified by \a key, and returns an integer, or the
  default value, \a def, if the entry couldn't be read.
  If \a ok is non-null, *ok is set to TRUE if the key was read, FALSE
  otherwise.

  \sa readEntry(), readDoubleEntry(), readBoolEntry(), writeEntry(), removeEntry()
*/

/*!
    \internal
*/
int QSettings::readNumEntry(const QString &key, int def, bool *ok )
{
    if ( !verifyKey( key ) ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QSettings::readNumEntry: Invalid key: '%s'", key.isNull() ? "(null)" : key.latin1() );
#endif
	if ( ok )
	    *ok = FALSE;
	return def;
    }

    QString theKey = groupKey( group(), key );

#if defined(Q_WS_WIN) || defined(Q_OS_MAC)
    if ( d->sysd )
	return d->sysReadNumEntry( theKey, def, ok );
#endif

    QString value = readEntry( theKey, QString::number( def ), ok );
    bool conv_ok;
    int retval = value.toInt( &conv_ok );
    if ( conv_ok )
	return retval;
    if ( ! value.isEmpty() )
	qWarning( "QSettings::readNumEntry: '%s' is not a number",
		  value.latin1() );
    if ( ok )
	*ok = FALSE;
    return def;
}


/*!
    \fn QString QSettings::readEntry(const QString &key, const QString &def, bool *ok ) const

  Reads the entry specified by \a key, and returns a QString, or the
  default value, \a def, if the entry couldn't be read.
  If \a ok is non-null, *ok is set to TRUE if the key was read, FALSE
  otherwise.

  \sa readListEntry(), readNumEntry(), readDoubleEntry(), readBoolEntry(), writeEntry(), removeEntry()
*/

/*!
    \internal
*/
QString QSettings::readEntry(const QString &key, const QString &def, bool *ok )
{
    if ( !verifyKey( key ) ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QSettings::readEntry: Invalid key: '%s'", key.isNull() ? "(null)" : key.latin1() );
#endif
	if ( ok )
	    *ok = FALSE;

	return def;
    }

    QString theKey = groupKey( group(), key );

#if defined(Q_WS_WIN) || defined(Q_OS_MAC)
    if ( d->sysd )
	return d->sysReadEntry( theKey, def, ok );
#endif

    if ( ok ) // no, everything is not ok
	*ok = FALSE;

    QString realkey;

    if (theKey[0] == '/') {
	// parse our key
	QStringList list(QStringList::split('/', theKey));

	if (list.count() < 2) {
#ifdef QT_CHECK_STATE
	    qWarning("QSettings::readEntry: invalid key '%s'", theKey.latin1());
#endif // QT_CHECK_STATE
	    if ( ok )
		*ok = FALSE;
	    return def;
	}

	if (list.count() == 2) {
	    d->heading = list[0];
	    d->group = "General";
	    realkey = list[1];
	} else {
	    d->heading = list[0];
	    d->group = list[1];

	    // remove the group from the list
	    list.remove(list.at(1));
	    // remove the heading from the list
	    list.remove(list.at(0));

	    realkey = list.join("/");
	}
    } else
    	realkey = theKey;

    QSettingsGroup grp = d->readGroup();
    QString retval = grp[realkey];
    if ( retval.isNull() )
	retval = def;
    else if ( ok ) // everything is ok
	*ok = TRUE;
    return retval;
}


#if !defined(Q_NO_BOOL_TYPE)
/*!
    Writes the boolean entry \a value into key \a key. The \a key is
    created if it doesn't exist. Any previous value is overwritten by \a
    value.

    If an error occurs the settings are left unchanged and FALSE is
    returned; otherwise TRUE is returned.

  \sa readListEntry(), readNumEntry(), readDoubleEntry(), readBoolEntry(), removeEntry()
*/
bool QSettings::writeEntry(const QString &key, bool value)
{
    if ( !verifyKey( key ) ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QSettings::writeEntry: Invalid key: '%s'", key.isNull() ? "(null)" : key.latin1() );
#endif
	return FALSE;
    }

    QString theKey = groupKey( group(), key );

#if defined(Q_WS_WIN) || defined(Q_OS_MAC)
    if ( d->sysd )
	return d->sysWriteEntry( theKey, value );
#endif
    QString s(value ? "true" : "false");
    return writeEntry(theKey, s);
}
#endif


/*!
    \overload
    Writes the double entry \a value into key \a key. The \a key is
    created if it doesn't exist. Any previous value is overwritten by \a
    value.

    If an error occurs the settings are left unchanged and FALSE is
    returned; otherwise TRUE is returned.

  \sa readListEntry(), readNumEntry(), readDoubleEntry(), readBoolEntry(), removeEntry()
*/
bool QSettings::writeEntry(const QString &key, double value)
{
    if ( !verifyKey( key ) ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QSettings::writeEntry: Invalid key: '%s'", key.isNull() ? "(null)" : key.latin1() );
#endif
	return FALSE;
    }

    QString theKey = groupKey( group(), key );

#if defined(Q_WS_WIN) || defined(Q_OS_MAC)
    if ( d->sysd )
	return d->sysWriteEntry( theKey, value );
#endif
    QString s(QString::number(value));
    return writeEntry(theKey, s);
}


/*!
    \overload
    Writes the integer entry \a value into key \a key. The \a key is
    created if it doesn't exist. Any previous value is overwritten by \a
    value.

    If an error occurs the settings are left unchanged and FALSE is
    returned; otherwise TRUE is returned.

  \sa readListEntry(), readNumEntry(), readDoubleEntry(), readBoolEntry(), removeEntry()
*/
bool QSettings::writeEntry(const QString &key, int value)
{
    if ( !verifyKey( key ) ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QSettings::writeEntry: Invalid key: '%s'", key.isNull() ? "(null)" : key.latin1() );
#endif
	return FALSE;
    }

    QString theKey = groupKey( group(), key );

#if defined(Q_WS_WIN) || defined(Q_OS_MAC)
    if ( d->sysd )
	return d->sysWriteEntry( theKey, value );
#endif
    QString s(QString::number(value));
    return writeEntry(theKey, s);
}


/*!
    \internal

  Writes the entry specified by \a key with the string-literal \a value,
  replacing any previous setting.  If \a value is zero-length or null, the
  entry is replaced by an empty setting.

  \e NOTE: This function is provided because some compilers use the
  writeEntry (const QString &, bool) overload for this code:
  writeEntry ("/foo/bar", "baz")

  If an error occurs, this functions returns FALSE and the object is left
  unchanged.

  \sa readEntry(), removeEntry()
*/
bool QSettings::writeEntry(const QString &key, const char *value)
{
    if ( !verifyKey( key ) ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QSettings::writeEntry: Invalid key: '%s'", key.isNull() ? "(null)" : key.latin1() );
#endif
	return FALSE;
    }

    QString theKey = groupKey( group(), key );

    return writeEntry(theKey, QString(value));
}


/*!
    \overload
    Writes the string entry \a value into key \a key. The \a key is
    created if it doesn't exist. Any previous value is overwritten by \a
    value. If \a value is an empty string or a null string the key's
    value will be an empty string.

    If an error occurs the settings are left unchanged and FALSE is
    returned; otherwise TRUE is returned.

  \sa readListEntry(), readNumEntry(), readDoubleEntry(), readBoolEntry(), removeEntry()
*/
bool QSettings::writeEntry(const QString &key, const QString &value)
{
    if ( !verifyKey( key ) ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QSettings::writeEntry: Invalid key: '%s'", key.isNull() ? "(null)" : key.latin1() );
#endif
	return FALSE;
    }

    QString theKey = groupKey( group(), key );

#if defined(Q_WS_WIN) || defined(Q_OS_MAC)
    if ( d->sysd )
	return d->sysWriteEntry( theKey, value );
#endif
    // NOTE: we *do* allow value to be a null/empty string

    QString realkey;

    if (theKey[0] == '/') {
	// parse our key
	QStringList list(QStringList::split('/', theKey));

	if (list.count() < 2) {
#ifdef QT_CHECK_STATE
	    qWarning("QSettings::writeEntry: invalid key '%s'", theKey.latin1());
#endif // QT_CHECK_STATE

	    return FALSE;
	}

	if (list.count() == 2) {
	    d->heading = list[0];
	    d->group = "General";
	    realkey = list[1];
	} else {
	    d->heading = list[0];
	    d->group = list[1];

	    // remove the group from the list
	    list.remove(list.at(1));
	    // remove the heading from the list
	    list.remove(list.at(0));

	    realkey = list.join("/");
	}
    } else
	realkey = theKey;

    d->writeGroup(realkey, value);
    return TRUE;
}


/*!
  Removes the entry specified by \a key.

    Returns TRUE if the entry existed and was removed; otherwise returns FALSE.

  \sa readEntry(), writeEntry()
*/
bool QSettings::removeEntry(const QString &key)
{
    if ( !verifyKey( key ) ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QSettings::removeEntry: Invalid key: '%s'", key.isNull() ? "(null)" : key.latin1() );
#endif
	return FALSE;
    }

    QString theKey = groupKey( group(), key );

#if defined(Q_WS_WIN) || defined(Q_OS_MAC)
    if ( d->sysd )
	return d->sysRemoveEntry( theKey );
#endif

    QString realkey;

    if (theKey[0] == '/') {
	// parse our key
	QStringList list(QStringList::split('/', theKey));

	if (list.count() < 2) {
#ifdef QT_CHECK_STATE
	    qWarning("QSettings::removeEntry: invalid key '%s'", theKey.latin1());
#endif // QT_CHECK_STATE

	    return FALSE;
	}

	if (list.count() == 2) {
	    d->heading = list[0];
	    d->group = "General";
	    realkey = list[1];
	} else {
	    d->heading = list[0];
	    d->group = list[1];

	    // remove the group from the list
	    list.remove(list.at(1));
	    // remove the heading from the list
	    list.remove(list.at(0));

	    realkey = list.join("/");
	}
    } else
	realkey = theKey;

    d->removeGroup(realkey);
    return TRUE;
}


/*!
  Returns a list of the keys which contain entries under \a key. Does \e
  not return any keys that contain keys.

    Example settings:
    \code
    /MyCompany/MyApplication/background color
    /MyCompany/MyApplication/foreground color
    /MyCompany/MyApplication/geometry/x
    /MyCompany/MyApplication/geometry/y
    /MyCompany/MyApplication/geometry/width
    /MyCompany/MyApplication/geometry/height
    \endcode
    \code
    QStringList keys = entryList( "/MyCompany/MyApplication" );
    \endcode
    \c keys contains 'background color' and 'foreground color'. It does
    not contain 'geometry' because this key contains keys not entries.

    To access the geometry values could either use subkeyList() to read
    the keys and then read each entry, or simply read each entry
    directly by specifying its full key, e.g.
    "/MyCompany/MyApplication/geometry/y".

  \sa subkeyList()
*/
QStringList QSettings::entryList(const QString &key) const
{
    if ( !verifyKey( key ) ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QSettings::entryList: Invalid key: %s", key.isNull() ? "(null)" : key.latin1() );
#endif
	return QStringList();
    }

    QString theKey = groupKey( group(), key );

#if defined(Q_WS_WIN) || defined(Q_OS_MAC)
    if ( d->sysd )
	return d->sysEntryList( theKey );
#endif

    QString realkey;
    if (theKey[0] == '/') {
	// parse our key
	QStringList list(QStringList::split('/', theKey));

	if (list.count() < 1) {
#ifdef QT_CHECK_STATE
	    qWarning("QSettings::listEntries: invalid key '%s'", theKey.latin1());
#endif // QT_CHECK_STATE

	    return QStringList();
	}

	if (list.count() == 1) {
	    d->heading = list[0];
	    d->group = "General";
	} else {
	    d->heading = list[0];
	    d->group = list[1];

	    // remove the group from the list
	    list.remove(list.at(1));
	    // remove the heading from the list
	    list.remove(list.at(0));

	    realkey = list.join("/");
	}
    } else
	realkey = theKey;

    QSettingsGroup grp = d->readGroup();
    QSettingsGroup::Iterator it = grp.begin();
    QStringList ret;
    QString itkey;
    while (it != grp.end()) {
	itkey = it.key();
	it++;

	if ( realkey.length() > 0 ) {
	    if ( itkey.left( realkey.length() ) != realkey )
		continue;
	    else
		itkey.remove( 0, realkey.length() + 1 );
	}

	if ( itkey.find( '/' ) != -1 )
	    continue;

	ret << itkey;
    }

    return ret;
}


/*!
  Returns a list of the keys which contain keys under \a key. Does \e
  not return any keys that contain entries.

    Example settings:
    \code
    /MyCompany/MyApplication/background color
    /MyCompany/MyApplication/foreground color
    /MyCompany/MyApplication/geometry/x
    /MyCompany/MyApplication/geometry/y
    /MyCompany/MyApplication/geometry/width
    /MyCompany/MyApplication/geometry/height
    /MyCompany/MyApplication/recent files/1
    /MyCompany/MyApplication/recent files/2
    /MyCompany/MyApplication/recent files/3
    \endcode
    \code
    QStringList keys = subkeyList( "/MyCompany/MyApplication" );
    \endcode
    \c keys contains 'geometry' and 'recent files'. It does not contain
    'background color' or 'foreground color' because they are keys which
    contain entries not keys. To get a list of keys that have values
    rather than subkeys use entryList().

  \sa entryList()
*/
QStringList QSettings::subkeyList(const QString &key) const
{
    if ( !verifyKey( key ) ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QSettings::subkeyList: Invalid key: %s", key.isNull() ? "(null)" : key.latin1() );
#endif
	return QStringList();
    }

    QString theKey = groupKey( group(), key );

#if defined(Q_WS_WIN) || defined(Q_OS_MAC)
    if ( d->sysd )
	return d->sysSubkeyList( theKey );
#endif

    QString realkey;
    if (theKey[0] == '/') {
	// parse our key
	QStringList list(QStringList::split('/', theKey));

	if (list.count() < 1) {
#ifdef QT_CHECK_STATE
	    qWarning("QSettings::subkeyList: invalid key '%s'", theKey.latin1());
#endif // QT_CHECK_STATE

	    return QStringList();
	}

	if (list.count() == 1) {
	    d->heading = list[0];
	    d->group = "General";
	} else {
	    d->heading = list[0];
	    d->group = list[1];

	    // remove the group from the list
	    list.remove(list.at(1));
	    // remove the heading from the list
	    list.remove(list.at(0));

	    realkey = list.join("/");
	}
    } else
	realkey = theKey;

    QSettingsGroup grp = d->readGroup();
    QSettingsGroup::Iterator it = grp.begin();
    QStringList ret;
    QString itkey;
    while (it != grp.end()) {
	itkey = it.key();
	it++;

	if ( realkey.length() > 0 ) {
	    if ( itkey.left( realkey.length() ) != realkey )
		continue;
	    else
		itkey.remove( 0, realkey.length() + 1 );
	}

	int slash = itkey.find( '/' );
	if ( slash == -1 )
	    continue;
	itkey.truncate( slash );

	if ( ! ret.contains( itkey ) )
	    ret << itkey;
    }

    return ret;
}


/*!
    \internal

  This function returns the time of last modification for \a key.
*/
QDateTime QSettings::lastModficationTime(const QString &key)
{
    if ( !verifyKey( key ) ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QSettings::lastModficationTime: Invalid key: '%s'", key.isNull() ? "(null)" : key.latin1() );
#endif
	return QDateTime();
    }

    QString theKey = groupKey( group(), key );

#if defined(Q_WS_WIN) || defined(Q_OS_MAC)
    if ( d->sysd )
	return QDateTime();
#endif

    if (theKey[0] == '/') {
	// parse our key
	QStringList list(QStringList::split('/', theKey));

	if (list.count() < 2) {
#ifdef QT_CHECK_STATE
	    qWarning("QSettings::lastModficationTime: invalid key '%s'", theKey.latin1());
#endif // QT_CHECK_STATE

	    return QDateTime();
	}

	if (list.count() == 2) {
	    d->heading = list[0];
	    d->group = "General";
	} else {
	    d->heading = list[0];
	    d->group = list[1];
	}
    }

    return d->modificationTime();
}


/*!
    \overload

    Writes the string list entry \a value into key \a key. The \a key
    is created if it doesn't exist. Any previous value is overwritten
    by \a value. The list is stored as a sequence of strings separated
    by \a separator, so none of the strings in the list should contain
    the separator. If the list is empty or null the key's value will
    be an empty string.

    If an error occurs the settings are left unchanged and FALSE is
    returned; otherwise returns TRUE.

    \sa readListEntry(), readNumEntry(), readDoubleEntry(), readBoolEntry(), removeEntry()
*/
bool QSettings::writeEntry(const QString &key, const QStringList &value,
			   const QChar &separator)
{
    QString s(value.join(separator));
    return writeEntry(key, s);
}

/*!
    \overload

    Writes the string list entry \a value into key \a key. The \a key
    is created if it doesn't exist. Any previous value is overwritten
    by \a value.

    If an error occurs the settings are left unchanged and FALSE is
    returned; otherwise returns TRUE.

    \sa readListEntry(), readNumEntry(), readDoubleEntry(), readBoolEntry(), removeEntry()
*/
bool QSettings::writeEntry(const QString &key, const QStringList &value)
{
    QString s;
    for (QStringList::ConstIterator it=value.begin(); it!=value.end(); ++it) {
	QString el = *it;
	if ( el.isNull() ) {
	    el = "^0";
	} else {
	    el.replace("^", "^^");
	}
	s+=el;
	s+="^e"; // end of element
    }
    return writeEntry(key, s);
}


/*!
    \overload QStringList QSettings::readListEntry(const QString &key, const QChar &separator, bool *ok ) const

    Reads the entry specified by \a key as a string. The \a separator
    is used to create a QStringList by calling QStringList::split(\a
    separator, entry). If \a ok is not 0: \a *ok is set to TRUE if the
    key was read, otherwise \a *ok is set to FALSE.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    QStringList list = mySettings.readListEntry( "size", " " );
    QStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

    \sa readEntry(), readDoubleEntry(), readBoolEntry(), writeEntry(), removeEntry(), QStringList::split()
*/

/*!
    \internal
*/
QStringList QSettings::readListEntry(const QString &key, const QChar &separator, bool *ok )
{
    QString value = readEntry( key, QString::null, ok );
    if ( ok && !*ok )
	return QStringList();

    return QStringList::split(separator, value);
}

/*!
    \fn QStringList QSettings::readListEntry(const QString &key, bool *ok ) const
    Reads the entry specified by \a key as a string. If \a ok is not
    0, \a *ok is set to TRUE if the key was read, otherwise \a *ok is
    set to FALSE.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    QStringList list = mySettings.readListEntry( "recentfiles" );
    QStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

    \sa readEntry(), readDoubleEntry(), readBoolEntry(), writeEntry(), removeEntry(), QStringList::split()
*/

/*!
    \internal
*/
QStringList QSettings::readListEntry(const QString &key, bool *ok )
{
    QString value = readEntry( key, QString::null, ok );
    if ( ok && !*ok )
	return QStringList();
    QStringList l;
    QString s;
    bool esc=FALSE;
    for (int i=0; i<(int)value.length(); i++) {
	if ( esc ) {
	    if ( value[i] == 'e' ) { // end-of-string
		l.append(s);
		s="";
	    } else if ( value[i] == '0' ) { // null string
		s=QString::null;
	    } else {
		s.append(value[i]);
	    }
	    esc=FALSE;
	} else if ( value[i] == '^' ) {
	    esc = TRUE;
	} else {
	    s.append(value[i]);
	    if ( i == (int)value.length()-1 )
		l.append(s);
	}
    }
    return l;
}

/*!
  Insert platform-dependent paths from platform-independent information.

  The \a domain should be an Internet domain name
  controlled by the producer of the software, eg. Trolltech products
  use "trolltech.com".

  The \a product should be the official name of the product.

  The \a scope should be
  QSettings::User for user-specific settings, or
  QSettings::Global for system-wide settings (generally
  these will be read-only to many users).
*/

void QSettings::setPath( const QString &domain, const QString &product, Scope scope )
{
//    On Windows, any trailing ".com(\..*)" is stripped from the domain. The
//    Global scope corresponds to HKEY_LOCAL_MACHINE, and User corresponds to
//    HKEY_CURRENT_USER. Note that on some installations, not all users can
//    write to the Global scope. On UNIX, any trailing ".com(\..*)" is stripped
//    from the domain. The Global scope corresponds to "/opt" (this would be
//    configurable at library build time - eg. to "/usr/local" or "/usr"),
//    while the User scope corresponds to $HOME/.*rc.
//    Note that on most installations, not all users can write to the System
//    scope.
//    
//    On MacOS X, if there is no "." in domain, append ".com", then reverse the
//    order of the elements (Mac OS uses "com.apple.finder" as domain+product).
//    The Global scope corresponds to /Library/Preferences/*.plist, while the
//    User scope corresponds to ~/Library/Preferences/*.plist.
//    Note that on most installations, not all users can write to the System
//    scope.
    QString actualSearchPath;
    int lastDot = domain.findRev( '.' );

#if defined(Q_WS_WIN)
    actualSearchPath = "/" + domain.mid( 0, lastDot ) + "/" + product;
    insertSearchPath( Windows, actualSearchPath );
#elif defined(Q_WS_MAC)
    QString topLevelDomain = domain.right( domain.length() - lastDot - 1 ) + ".";
    if ( topLevelDomain.isEmpty() )
	topLevelDomain = "com.";
    actualSearchPath = "/" + topLevelDomain + domain.left( lastDot ) + product;
    insertSearchPath( Mac, actualSearchPath );
#else
    actualSearchPath = "/" + domain.mid( 0, lastDot ) + "/" + product;
    insertSearchPath( Unix, actualSearchPath );
#endif

    d->globalScope = scope == Global;
}

/*!
    Appends \a group to the current key prefix.
*/
void QSettings::beginGroup( const QString &group )
{
    d->groupStack.push( group );
    d->groupDirty = TRUE;
}

/*!
    Undo previous calls to beginGroup(). Note that a single beginGroup("a/b/c") is undone
    by a single call to endGroup().
*/
void QSettings::endGroup()
{
    d->groupStack.pop();
    d->groupDirty = TRUE;
}

/*!
    Set the current key prefix to the empty string.
*/
void QSettings::resetGroup()
{
    d->groupStack.clear();
    d->groupDirty = FALSE;
    d->groupPrefix = QString::null;
}

/*!
    Returns the current key prefix, or a null string if there is no key prefix set.

    \sa beginGroup();
*/
QString QSettings::group() const
{
    if ( d->groupDirty ) {
	d->groupDirty = FALSE;
	d->groupPrefix = QString::null;

	QValueStack<QString>::Iterator it = d->groupStack.begin();
	while ( it != d->groupStack.end() ) {
	    QString group = *it;
	    ++it;
	    if ( group[0] != '/' )
		group = "/" + group;
	    d->groupPrefix += group;
	}
    }
    return d->groupPrefix;
}

#endif
