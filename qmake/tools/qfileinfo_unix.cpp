/****************************************************************************
** $Id: qfileinfo_unix.cpp,v 1.1 2002-11-01 00:10:44 kergoth Exp $
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
** licenses for Unix/X11 or for Qt/Embedded may use this file in accordance
** with the Qt Commercial License Agreement provided with the Software.
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
#include "qfiledefs_p.h"
#include "qdatetime.h"
#include "qdir.h"

#include <limits.h>


void QFileInfo::slashify( QString& )
{
    return;
}


void QFileInfo::makeAbs( QString & )
{
    return;
}

/*!
    Returns TRUE if this object points to a file. Returns FALSE if the
    object points to something which isn't a file, e.g. a directory or
    a symlink.

    \sa isDir(), isSymLink()
*/
bool QFileInfo::isFile() const
{
    if ( !fic || !cache )
	doStat();
    return fic ? (fic->st.st_mode & S_IFMT) == S_IFREG : FALSE;
}

/*!
    Returns TRUE if this object points to a directory or to a symbolic
    link to a directory; otherwise returns FALSE.

    \sa isFile(), isSymLink()
*/
bool QFileInfo::isDir() const
{
    if ( !fic || !cache )
	doStat();
    return fic ? (fic->st.st_mode & S_IFMT) == S_IFDIR : FALSE;
}

/*!
    Returns TRUE if this object points to a symbolic link (or to a
    shortcut on Windows); otherwise returns FALSE.

    \sa isFile(), isDir(), readLink()
*/

bool QFileInfo::isSymLink() const
{
    if ( !fic || !cache )
	doStat();
    return symLink;
}

/*!
    Returns the name a symlink (or shortcut on Windows) points to, or
    a QString::null if the object isn't a symbolic link.

    This name may not represent an existing file; it is only a string.
    QFileInfo::exists() returns TRUE if the symlink points to an
    existing file.

    \sa exists(), isSymLink(), isDir(), isFile()
*/

QString QFileInfo::readLink() const
{
    QString r;

#if defined(Q_OS_UNIX) && !defined(Q_OS_OS2EMX)
    char s[PATH_MAX+1];
    if ( !isSymLink() )
	return QString();
    int len = readlink( QFile::encodeName(fn).data(), s, PATH_MAX );
    if ( len >= 0 ) {
	s[len] = '\0';
	r = QFile::decodeName(s);
    }
#endif

    return r;
}

static const uint nobodyID = (uint) -2;

/*!
    Returns the owner of the file. On Windows, on systems where files
    do not have owners, or if an error occurs, QString::null is
    returned.

    This function can be time consuming under Unix (in the order of
    milliseconds).

    \sa ownerId(), group(), groupId()
*/

QString QFileInfo::owner() const
{
    passwd *pw = getpwuid( ownerId() );
    if ( pw )
	return QFile::decodeName( pw->pw_name );
    return QString::null;
}

/*!
    Returns the id of the owner of the file.

    On Windows and on systems where files do not have owners this
    function returns ((uint) -2).

    \sa owner(), group(), groupId()
*/

uint QFileInfo::ownerId() const
{
    if ( !fic || !cache )
	doStat();
    if ( fic )
	return fic->st.st_uid;
    return nobodyID;
}

/*!
    Returns the group of the file. On Windows, on systems where files
    do not have groups, or if an error occurs, QString::null is
    returned.

    This function can be time consuming under Unix (in the order of
    milliseconds).

    \sa groupId(), owner(), ownerId()
*/

QString QFileInfo::group() const
{
    struct group *gr = getgrgid( groupId() );
    if ( gr )
	return QFile::decodeName( gr->gr_name );
    return QString::null;
}

/*!
    Returns the id of the group the file belongs to.

    On Windows and on systems where files do not have groups this
    function always returns (uint) -2.

    \sa group(), owner(), ownerId()
*/

uint QFileInfo::groupId() const
{
    if ( !fic || !cache )
	doStat();
    if ( fic )
	return fic->st.st_gid;
    return nobodyID;
}


/*!
    Tests for file permissions. The \a permissionSpec argument can be
    several flags of type \c PermissionSpec OR-ed together to check
    for permission combinations.

    On systems where files do not have permissions this function
    always returns TRUE.

    Example:
    \code
	QFileInfo fi( "/tmp/archive.tar.gz" );
	if ( fi.permission( QFileInfo::WriteUser | QFileInfo::ReadGroup ) )
	    qWarning( "I can change the file; my group can read the file.");
	if ( fi.permission( QFileInfo::WriteGroup | QFileInfo::WriteOther ) )
	    qWarning( "The group or others can change the file!" );
    \endcode

    \sa isReadable(), isWritable(), isExecutable()
*/

bool QFileInfo::permission( int permissionSpec ) const
{
    if ( !fic || !cache )
	doStat();
    if ( fic ) {
	uint mask = 0;
	if ( permissionSpec & ReadUser )
	    mask |= S_IRUSR;
	if ( permissionSpec & WriteUser )
	    mask |= S_IWUSR;
	if ( permissionSpec & ExeUser )
	    mask |= S_IXUSR;
	if ( permissionSpec & ReadGroup )
	    mask |= S_IRGRP;
	if ( permissionSpec & WriteGroup )
	    mask |= S_IWGRP;
	if ( permissionSpec & ExeGroup )
	    mask |= S_IXGRP;
	if ( permissionSpec & ReadOther )
	    mask |= S_IROTH;
	if ( permissionSpec & WriteOther )
	    mask |= S_IWOTH;
	if ( permissionSpec & ExeOther )
	    mask |= S_IXOTH;
	if ( mask ) {
	   return (fic->st.st_mode & mask) == mask;
	} else {
#if defined(QT_CHECK_NULL)
	   qWarning( "QFileInfo::permission: permissionSpec is 0" );
#endif
	   return TRUE;
	}
    } else {
	return FALSE;
    }
}

void QFileInfo::doStat() const
{
    QFileInfo *that = ((QFileInfo*)this);	// mutable function
    if ( !that->fic )
	that->fic = new QFileInfoCache;
    that->symLink = FALSE;
    struct stat *b = &that->fic->st;
#if defined(Q_OS_UNIX) && defined(S_IFLNK)
    if ( ::lstat( QFile::encodeName(fn), b ) == 0 ) {
	if ( S_ISLNK( b->st_mode ) )
	    that->symLink = TRUE;
	else
	    return;
    }
#endif

    int r = ::stat( QFile::encodeName(fn), b );
    if ( r != 0 && !that->symLink ) {
	delete that->fic;
	that->fic = 0;
    }
}

/*!
    Returns the file's path.

    If \a absPath is TRUE an absolute path is returned.

    \sa dir(), filePath(), fileName(), isRelative()
*/
#ifndef QT_NO_DIR
QString QFileInfo::dirPath( bool absPath ) const
{
    QString s;
    if ( absPath )
	s = absFilePath();
    else
	s = fn;
    int pos = s.findRev( '/' );
    if ( pos == -1 ) {
	return QString::fromLatin1( "." );
    } else {
	if ( pos == 0 )
	    return QString::fromLatin1( "/" );
	return s.left( pos );
    }
}
#endif

/*!
    Returns the name of the file, excluding the path.

    Example:
    \code
	QFileInfo fi( "/tmp/archive.tar.gz" );
	QString name = fi.fileName();		// name = "archive.tar.gz"
    \endcode

    \sa isRelative(), filePath(), baseName(), extension()
*/

QString QFileInfo::fileName() const
{
    int p = fn.findRev( '/' );
    if ( p == -1 ) {
	return fn;
    } else {
	return fn.mid( p + 1 );
    }
}
