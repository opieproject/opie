/****************************************************************************
** $Id: qfileinfo.cpp,v 1.1 2002-11-01 00:10:44 kergoth Exp $
**
** Implementation of QFileInfo class
**
** Created : 950628
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
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

#include "qfileinfo.h"
#include "qdatetime.h"
#include "qdir.h"
#include "qfiledefs_p.h"
#if defined(QT_LARGEFILE_SUPPORT) && !defined(QT_ABI_QT4)
#include <limits.h>
#endif


extern bool qt_file_access( const QString& fn, int t );

/*!
    \class QFileInfo
    \reentrant
    \brief The QFileInfo class provides system-independent file information.

    \ingroup io

    QFileInfo provides information about a file's name and position
    (path) in the file system, its access rights and whether it is a
    directory or symbolic link, etc. The file's size and last
    modified/read times are also available.

    A QFileInfo can point to a file with either a relative or an
    absolute file path. Absolute file paths begin with the directory
    separator "/" (or with a drive specification on Windows). Relative
    file names begin with a directory name or a file name and specify
    a path relative to the current working directory. An example of an
    absolute path is the string "/tmp/quartz". A relative path might
    look like "src/fatlib". You can use the function isRelative() to
    check whether a QFileInfo is using a relative or an absolute file
    path. You can call the function convertToAbs() to convert a
    relative QFileInfo's path to an absolute path.

    The file that the QFileInfo works on is set in the constructor or
    later with setFile(). Use exists() to see if the file exists and
    size() to get its size.

    To speed up performance, QFileInfo caches information about the
    file. Because files can be changed by other users or programs, or
    even by other parts of the same program, there is a function that
    refreshes the file information: refresh(). If you want to switch
    off a QFileInfo's caching and force it to access the file system
    every time you request information from it call setCaching(FALSE).

    The file's type is obtained with isFile(), isDir() and
    isSymLink(). The readLink() function provides the name of the file
    the symlink points to.

    Elements of the file's name can be extracted with dirPath() and
    fileName(). The fileName()'s parts can be extracted with
    baseName() and extension().

    The file's dates are returned by created(), lastModified() and
    lastRead(). Information about the file's access permissions is
    obtained with isReadable(), isWritable() and isExecutable(). The
    file's ownership is available from owner(), ownerId(), group() and
    groupId(). You can examine a file's permissions and ownership in a
    single statement using the permission() function.

    If you need to read and traverse directories, see the QDir class.
*/

/*!
    \enum QFileInfo::PermissionSpec

    This enum is used by the permission() function to report the
    permissions and ownership of a file. The values may be OR-ed
    together to test multiple permissions and ownership values.

    \value ReadUser The file is readable by the user.
    \value WriteUser The file is writable by the user.
    \value ExeUser The file is executable by the user.
    \value ReadGroup The file is readable by the group.
    \value WriteGroup The file is writable by the group.
    \value ExeGroup The file is executable by the group.
    \value ReadOther The file is readable by anyone.
    \value WriteOther The file is writable by anyone.
    \value ExeOther The file is executable by anyone.
*/


/*!
    Constructs a new empty QFileInfo.
*/

QFileInfo::QFileInfo()
{
    fic	  = 0;
    cache = TRUE;
#if defined(Q_OS_UNIX)
    symLink = FALSE;
#endif
}

/*!
    Constructs a new QFileInfo that gives information about the given
    file. The \a file can also include an absolute or relative path.

    \sa setFile(), isRelative(), QDir::setCurrent(), QDir::isRelativePath()
*/

QFileInfo::QFileInfo( const QString &file )
{
    fn	  = file;
    slashify( fn );
    fic	  = 0;
    cache = TRUE;
#if defined(Q_OS_UNIX)
    symLink = FALSE;
#endif
}

/*!
    Constructs a new QFileInfo that gives information about file \a
    file.

    If the \a file has a relative path, the QFileInfo will also have a
    relative path.

    \sa isRelative()
*/

QFileInfo::QFileInfo( const QFile &file )
{
    fn	  = file.name();
    slashify( fn );
    fic	  = 0;
    cache = TRUE;
#if defined(Q_OS_UNIX)
    symLink = FALSE;
#endif
}

/*!
    Constructs a new QFileInfo that gives information about the file
    called \a fileName in the directory \a d.

    If \a d has a relative path, the QFileInfo will also have a
    relative path.

    \sa isRelative()
*/
#ifndef QT_NO_DIR
QFileInfo::QFileInfo( const QDir &d, const QString &fileName )
{
    fn	  = d.filePath( fileName );
    slashify( fn );
    fic	  = 0;
    cache = TRUE;
#if defined(Q_OS_UNIX)
    symLink = FALSE;
#endif
}
#endif
/*!
    Constructs a new QFileInfo that is a copy of \a fi.
*/

QFileInfo::QFileInfo( const QFileInfo &fi )
{
    fn = fi.fn;
    if ( fi.fic ) {
	fic = new QFileInfoCache;
	*fic = *fi.fic;
    } else {
	fic = 0;
    }
    cache = fi.cache;
#if defined(Q_OS_UNIX)
    symLink = fi.symLink;
#endif
}

/*!
    Destroys the QFileInfo and frees its resources.
*/

QFileInfo::~QFileInfo()
{
    delete fic;
}


/*!
    Makes a copy of \a fi and assigns it to this QFileInfo.
*/

QFileInfo &QFileInfo::operator=( const QFileInfo &fi )
{
    fn = fi.fn;
    if ( !fi.fic ) {
	delete fic;
	fic = 0;
    } else {
	if ( !fic ) {
	    fic = new QFileInfoCache;
	    Q_CHECK_PTR( fic );
	}
	*fic = *fi.fic;
    }
    cache = fi.cache;
#if defined(Q_OS_UNIX)
    symLink = fi.symLink;
#endif
    return *this;
}


/*!
    Sets the file that the QFileInfo provides information about to \a
    file.

    The \a file can also include an absolute or relative file path.
    Absolute paths begin with the directory separator (e.g. "/" under
    Unix) or a drive specification (under Windows). Relative file
    names begin with a directory name or a file name and specify a
    path relative to the current directory.

    Example:
    \code
    QString absolute = "/local/bin";
    QString relative = "local/bin";
    QFileInfo absFile( absolute );
    QFileInfo relFile( relative );

    QDir::setCurrent( QDir::rootDirPath() );
    // absFile and relFile now point to the same file

    QDir::setCurrent( "/tmp" );
    // absFile now points to "/local/bin",
    // while relFile points to "/tmp/local/bin"
    \endcode

    \sa isRelative(), QDir::setCurrent(), QDir::isRelativePath()
*/

void QFileInfo::setFile( const QString &file )
{
    fn = file;
    slashify( fn );
    delete fic;
    fic = 0;
}

/*!
    \overload

    Sets the file that the QFileInfo provides information about to \a
    file.

    If \a file includes a relative path, the QFileInfo will also have
    a relative path.

    \sa isRelative()
*/

void QFileInfo::setFile( const QFile &file )
{
    fn	= file.name();
    slashify( fn );
    delete fic;
    fic = 0;
}

/*!
    \overload

    Sets the file that the QFileInfo provides information about to \a
    fileName in directory \a d.

    If \a fileName includes a relative path, the QFileInfo will also
    have a relative path.

    \sa isRelative()
*/
#ifndef QT_NO_DIR
void QFileInfo::setFile( const QDir &d, const QString &fileName )
{
    fn	= d.filePath( fileName );
    slashify( fn );
    delete fic;
    fic = 0;
}
#endif

/*!
    Returns TRUE if the file exists; otherwise returns FALSE.
*/

bool QFileInfo::exists() const
{
    return qt_file_access( fn, F_OK );
}

/*!
    Refreshes the information about the file, i.e. reads in information
    from the file system the next time a cached property is fetched.

    \sa setCaching()
*/

void QFileInfo::refresh() const
{
    QFileInfo *that = (QFileInfo*)this;		// Mutable function
    delete that->fic;
    that->fic = 0;
}

/*!
    \fn bool QFileInfo::caching() const

    Returns TRUE if caching is enabled; otherwise returns FALSE.

    \sa setCaching(), refresh()
*/

/*!
    If \a enable is TRUE, enables caching of file information. If \a
    enable is FALSE caching is disabled.

    When caching is enabled, QFileInfo reads the file information from
    the file system the first time it's needed, but generally not
    later.

    Caching is enabled by default.

    \sa refresh(), caching()
*/

void QFileInfo::setCaching( bool enable )
{
    if ( cache == enable )
	return;
    cache = enable;
    if ( cache ) {
	delete fic;
	fic = 0;
    }
}


/*!
    Returns the file name, including the path (which may be absolute
    or relative).

    \sa isRelative(), absFilePath()
*/

QString QFileInfo::filePath() const
{
    return fn;
}

/*!
    Returns the base name of the file.

    If \a complete is FALSE (the default) the base name consists of
    all characters in the file name up to (but not including) the \e
    first '.' character.

    If \a complete is TRUE the base name consists of all characters in
    the file up to (but not including) the \e last '.' character.

    The path is not included in either case.

    Example:
    \code
	QFileInfo fi( "/tmp/archive.tar.gz" );
	QString base = fi.baseName();  // base = "archive"
	base = fi.baseName( TRUE );    // base = "archive.tar"
    \endcode

    \sa fileName(), extension()
*/

QString QFileInfo::baseName( bool complete ) const
{
    QString tmp = fileName();
    int pos = complete ? tmp.findRev( '.' ) : tmp.find( '.' );
    if ( pos == -1 )
	return tmp;
    else
	return tmp.left( pos );
}

/*!
    Returns the file's extension name.

    If \a complete is TRUE (the default), extension() returns the
    string of all characters in the file name after (but not
    including) the first '.'  character.

    If \a complete is FALSE, extension() returns the string of all
    characters in the file name after (but not including) the last '.'
    character.

    Example:
    \code
	QFileInfo fi( "/tmp/archive.tar.gz" );
	QString ext = fi.extension();  // ext = "tar.gz"
	ext = fi.extension( FALSE );   // ext = "gz"
    \endcode

    \sa fileName(), baseName()
*/

QString QFileInfo::extension( bool complete ) const
{
    QString s = fileName();
    int pos = complete ? s.find( '.' ) : s.findRev( '.' );
    if ( pos < 0 )
	return QString::fromLatin1( "" );
    else
	return s.right( s.length() - pos - 1 );
}

/*!
    Returns the file's path as a QDir object.

    If the QFileInfo is relative and \a absPath is FALSE, the QDir
    will be relative; otherwise it will be absolute.

    \sa dirPath(), filePath(), fileName(), isRelative()
*/
#ifndef QT_NO_DIR
QDir QFileInfo::dir( bool absPath ) const
{
    return QDir( dirPath(absPath) );
}
#endif


/*!
    Returns TRUE if the file is readable; otherwise returns FALSE.

    \sa isWritable(), isExecutable(), permission()
*/

bool QFileInfo::isReadable() const
{
    return qt_file_access( fn, R_OK ) && permission( ReadUser );
}

/*!
    Returns TRUE if the file is writable; otherwise returns FALSE.

    \sa isReadable(), isExecutable(), permission()
*/

bool QFileInfo::isWritable() const
{
    return qt_file_access( fn, W_OK ) && permission( WriteUser );
}

/*!
    Returns TRUE if the file is executable; otherwise returns FALSE.

    \sa isReadable(), isWritable(), permission()
*/

bool QFileInfo::isExecutable() const
{
    return qt_file_access( fn, X_OK ) && permission( ExeUser );
}

#ifndef Q_WS_WIN
bool QFileInfo::isHidden() const
{
    return fileName()[ 0 ] == QChar( '.' );
}
#endif

/*!
    Returns TRUE if the file path name is relative. Returns FALSE if
    the path is absolute (e.g. under Unix a path is absolute if it
    begins with a "/").
*/
#ifndef QT_NO_DIR
bool QFileInfo::isRelative() const
{
    return QDir::isRelativePath( fn );
}

/*!
    Converts the file's path to an absolute path.

    If it is already absolute, nothing is done.

    \sa filePath(), isRelative()
*/

bool QFileInfo::convertToAbs()
{
    if ( isRelative() )
	fn = absFilePath();
    return QDir::isRelativePath( fn );
}
#endif

/*!
    Returns the file size in bytes, or 0 if the file does not exist or
    if the size is 0 or if the size cannot be fetched.
*/
#if defined(QT_ABI_QT4)
QIODevice::Offset QFileInfo::size() const
#else
uint QFileInfo::size() const
#endif
{
    if ( !fic || !cache )
	doStat();
    if ( fic )
#if defined(QT_ABI_QT4)
	return (QIODevice::Offset)fic->st.st_size;
#elif defined(QT_LARGEFILE_SUPPORT)
	return (uint)fic->st.st_size > UINT_MAX ? UINT_MAX : (uint)fic->st.st_size;
#else
	return (uint)fic->st.st_size;
#endif
    else
	return 0;
}

/*!
    Returns the date and time when the file was created.

    On platforms where this information is not available, returns the
    same as lastModified().

    \sa created() lastModified() lastRead()
*/

QDateTime QFileInfo::created() const
{
    QDateTime dt;
    if ( !fic || !cache )
	doStat();
    if ( fic && fic->st.st_ctime != 0 ) {
	dt.setTime_t( fic->st.st_ctime );
	return dt;
    } else {
	return lastModified();
    }
}

/*!
    Returns the date and time when the file was last modified.

    \sa created() lastModified() lastRead()
*/

QDateTime QFileInfo::lastModified() const
{
    QDateTime dt;
    if ( !fic || !cache )
	doStat();
    if ( fic )
	dt.setTime_t( fic->st.st_mtime );
    return dt;
}

/*!
    Returns the date and time when the file was last read (accessed).

    On platforms where this information is not available, returns the
    same as lastModified().

    \sa created() lastModified() lastRead()
*/

QDateTime QFileInfo::lastRead() const
{
    QDateTime dt;
    if ( !fic || !cache )
	doStat();
    if ( fic && fic->st.st_atime != 0 ) {
	dt.setTime_t( fic->st.st_atime );
	return dt;
    } else {
	return lastModified();
    }
}

#ifndef QT_NO_DIR

/*!
    Returns the absolute path including the file name.

    The absolute path name consists of the full path and the file
    name. On Unix this will always begin with the root, '/',
    directory. On Windows this will always begin 'D:/' where D is a
    drive letter, except for network shares that are not mapped to a
    drive letter, in which case the path will begin '//sharename/'.

    This function returns the same as filePath(), unless isRelative()
    is TRUE.

    This function can be time consuming under Unix (in the order of
    milliseconds).

    \sa isRelative(), filePath()
*/
QString QFileInfo::absFilePath() const
{
    QString tmp;
    if ( QDir::isRelativePath(fn)
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
	 && fn[1] != ':'
#endif
	 ) {
	tmp = QDir::currentDirPath();
	tmp += '/';
    }
    tmp += fn;
    makeAbs( tmp );
    return QDir::cleanDirPath( tmp );
}

#endif
