/****************************************************************************
** $Id: qdir.cpp,v 1.1 2002-11-01 00:10:44 kergoth Exp $
**
** Implementation of QDir class
**
** Created : 950427
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
#include "qdir.h"

#ifndef QT_NO_DIR
#include <private/qdir_p.h>
#include "qfileinfo.h"
#include "qregexp.h"
#include "qstringlist.h"
#include <stdlib.h>
#include <ctype.h>



/*!
    \class QDir
    \brief The QDir class provides access to directory structures and their contents in a platform-independent way.

    \ingroup io
    \mainclass

    A QDir is used to manipulate path names, access information
    regarding paths and files, and manipulate the underlying file
    system.

    A QDir can point to a file using either a relative or an absolute
    path. Absolute paths begin with the directory separator "/"
    (optionally preceded by a drive specification under Windows). If
    you always use "/" as a directory separator, Qt will translate
    your paths to conform to the underlying operating system. Relative
    file names begin with a directory name or a file name and specify
    a path relative to the current directory.

    The "current" path refers to the application's working directory.
    A QDir's own path is set and retrieved with setPath() and path().

    An example of an absolute path is the string "/tmp/quartz", a
    relative path might look like "src/fatlib". You can use the
    function isRelative() to check if a QDir is using a relative or an
    absolute file path. Call convertToAbs() to convert a relative QDir
    to an absolute one. For a simplified path use cleanDirPath(). To
    obtain a path which has no symbolic links or redundant ".."
    elements use canonicalPath(). The path can be set with setPath(),
    and changed with cd() and cdUp().

    QDir provides several static functions, for example, setCurrent()
    to set the application's working directory and currentDirPath() to
    retrieve the application's working directory. Access to some
    common paths is provided with the static functions, current(),
    home() and root() which return QDir objects or currentDirPath(),
    homeDirPath() and rootDirPath() which return the path as a string.

    The number of entries in a directory is returned by count().
    Obtain a string list of the names of all the files and directories
    in a directory with entryList(). If you prefer a list of QFileInfo
    pointers use entryInfoList(). Both these functions can apply a
    name filter, an attributes filter (e.g. read-only, files not
    directories, etc.), and a sort order. The filters and sort may be
    set with calls to setNameFilter(), setFilter() and setSorting().
    They may also be specified in the entryList() and
    entryInfoList()'s arguments.

    Create a new directory with mkdir(), rename a directory with
    rename() and remove an existing directory with rmdir(). Remove a
    file with remove(). You can interrogate a directory with exists(),
    isReadable() and isRoot().

    To get a path with a filename use filePath(), and to get a
    directory name use dirName(); neither of these functions checks
    for the existence of the file or directory.

    The list of root directories is provided by drives(); on Unix
    systems this returns a list containing one root directory, "/"; on
    Windows the list will usually contain "C:/", and possibly "D:/",
    etc.

    If you need the path in a form suitable for the underlying
    operating system use convertSeparators().

    Examples:

    See if a directory exists.
    \code
    QDir d( "example" );			// "./example"
    if ( !d.exists() )
	qWarning( "Cannot find the example directory" );
    \endcode

    Traversing directories and reading a file.
    \code
    QDir d = QDir::root();			// "/"
    if ( !d.cd("tmp") ) {			// "/tmp"
	qWarning( "Cannot find the \"/tmp\" directory" );
    } else {
	QFile f( d.filePath("ex1.txt") );	// "/tmp/ex1.txt"
	if ( !f.open(IO_ReadWrite) )
	    qWarning( "Cannot create the file %s", f.name() );
    }
    \endcode

    A program that lists all the files in the current directory
    (excluding symbolic links), sorted by size, smallest first:
    \code
    #include <stdio.h>
    #include <qdir.h>

    int main( int argc, char **argv )
    {
	QDir d;
	d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
	d.setSorting( QDir::Size | QDir::Reversed );

	const QFileInfoList *list = d.entryInfoList();
	QFileInfoListIterator it( *list );
	QFileInfo *fi;

	printf( "     Bytes Filename\n" );
	while ( (fi = it.current()) != 0 ) {
	    printf( "%10li %s\n", fi->size(), fi->fileName().latin1() );
	    ++it;
	}
	return 0;
    }
    \endcode
*/

/*!
    Constructs a QDir pointing to the current directory.

    \sa currentDirPath()
*/

QDir::QDir()
{
    dPath = QString::fromLatin1(".");
    init();
}

/*!
    Constructs a QDir with path \a path, that filters its entries by
    name using \a nameFilter and by attributes using \a filterSpec. It
    also sorts the names using \a sortSpec.

    The default \a nameFilter is an empty string, which excludes
    nothing; the default \a filterSpec is \c All, which also means
    exclude nothing. The default \a sortSpec is \c Name|IgnoreCase,
    i.e. sort by name case-insensitively.

    Example that lists all the files in "/tmp":
    \code
    QDir d( "/tmp" );
    for ( int i = 0; i < d.count(); i++ )
	printf( "%s\n", d[i] );
    \endcode

    If \a path is "" or QString::null, QDir uses "." (the current
    directory). If \a nameFilter is "" or QString::null, QDir uses the
    name filter "*" (all files).

    Note that \a path need not exist.

    \sa exists(), setPath(), setNameFilter(), setFilter(), setSorting()
*/

QDir::QDir( const QString &path, const QString &nameFilter,
	    int sortSpec, int filterSpec )
{
    init();
    dPath = cleanDirPath( path );
    if ( dPath.isEmpty() )
	dPath = QString::fromLatin1(".");
    nameFilt = nameFilter;
    if ( nameFilt.isEmpty() )
	nameFilt = QString::fromLatin1("*");
    filtS = (FilterSpec)filterSpec;
    sortS = (SortSpec)sortSpec;
}

/*!
    Constructs a QDir that is a copy of the directory \a d.

    \sa operator=()
*/

QDir::QDir( const QDir &d )
{
    dPath = d.dPath;
    fList = 0;
    fiList = 0;
    nameFilt = d.nameFilt;
    dirty = TRUE;
    allDirs = d.allDirs;
    filtS = d.filtS;
    sortS = d.sortS;
}


void QDir::init()
{
    fList = 0;
    fiList = 0;
    nameFilt = QString::fromLatin1("*");
    dirty = TRUE;
    allDirs = FALSE;
    filtS = All;
    sortS = SortSpec(Name | IgnoreCase);
}

/*!
    Destroys the QDir frees up its resources.
*/

QDir::~QDir()
{
    delete fList;
    delete fiList;
}


/*!
    Sets the path of the directory to \a path. The path is cleaned of
    redundant ".", ".." and of multiple separators. No check is made
    to ensure that a directory with this path exists.

    The path can be either absolute or relative. Absolute paths begin
    with the directory separator "/" (optionally preceded by a drive
    specification under Windows). Relative file names begin with a
    directory name or a file name and specify a path relative to the
    current directory. An example of an absolute path is the string
    "/tmp/quartz", a relative path might look like "src/fatlib".

    \sa path(), absPath(), exists(), cleanDirPath(), dirName(),
      absFilePath(), isRelative(), convertToAbs()
*/

void QDir::setPath( const QString &path )
{
    dPath = cleanDirPath( path );
    if ( dPath.isEmpty() )
	dPath = QString::fromLatin1(".");
    dirty = TRUE;
}

/*!
    \fn  QString QDir::path() const

    Returns the path, this may contain symbolic links, but never
    contains redundant ".", ".." or multiple separators.

    The returned path can be either absolute or relative (see
    setPath()).

    \sa setPath(), absPath(), exists(), cleanDirPath(), dirName(),
    absFilePath(), convertSeparators()
*/

/*!
    Returns the absolute path (a path that starts with "/" or with a
    drive specification), which may contain symbolic links, but never
    contains redundant ".", ".." or multiple separators.

    \sa setPath(), canonicalPath(), exists(),  cleanDirPath(),
    dirName(), absFilePath()
*/

QString QDir::absPath() const
{
    if ( QDir::isRelativePath(dPath) ) {
	QString tmp = currentDirPath();
	if ( tmp.right(1) != QString::fromLatin1("/") )
	    tmp += '/';
	tmp += dPath;
	return cleanDirPath( tmp );
    } else {
	return cleanDirPath( dPath );
    }
}

/*!
    Returns the name of the directory; this is \e not the same as the
    path, e.g. a directory with the name "mail", might have the path
    "/var/spool/mail". If the directory has no name (e.g. it is the
    root directory) QString::null is returned.

    No check is made to ensure that a directory with this name
    actually exists.

    \sa path(), absPath(), absFilePath(), exists(), QString::isNull()
*/

QString QDir::dirName() const
{
    int pos = dPath.findRev( '/' );
    if ( pos == -1  )
	return dPath;
    return dPath.right( dPath.length() - pos - 1 );
}

/*!
    Returns the path name of a file in the directory. Does \e not
    check if the file actually exists in the directory. If the QDir is
    relative the returned path name will also be relative. Redundant
    multiple separators or "." and ".." directories in \a fileName
    will not be removed (see cleanDirPath()).

    If \a acceptAbsPath is TRUE a \a fileName starting with a
    separator "/" will be returned without change. If \a acceptAbsPath
    is FALSE an absolute path will be prepended to the fileName and
    the resultant string returned.

    \sa absFilePath(), isRelative(), canonicalPath()
*/

QString QDir::filePath( const QString &fileName,
			bool acceptAbsPath ) const
{
    if ( acceptAbsPath && !isRelativePath(fileName) )
	return QString(fileName);

    QString tmp = dPath;
    if ( tmp.isEmpty() || (tmp[(int)tmp.length()-1] != '/' && !!fileName &&
			   fileName[0] != '/') )
	tmp += '/';
    tmp += fileName;
    return tmp;
}

/*!
    Returns the absolute path name of a file in the directory. Does \e
    not check if the file actually exists in the directory. Redundant
    multiple separators or "." and ".." directories in \a fileName
    will not be removed (see cleanDirPath()).

    If \a acceptAbsPath is TRUE a \a fileName starting with a
    separator "/" will be returned without change. If \a acceptAbsPath
    is FALSE an absolute path will be prepended to the fileName and
    the resultant string returned.

    \sa filePath()
*/

QString QDir::absFilePath( const QString &fileName,
			   bool acceptAbsPath ) const
{
    if ( acceptAbsPath && !isRelativePath( fileName ) )
	return fileName;

    QString tmp = absPath();
    if ( tmp.isEmpty() || (tmp[(int)tmp.length()-1] != '/' && !!fileName &&
			   fileName[0] != '/') )
	tmp += '/';
    tmp += fileName;
    return tmp;
}


/*!
    Returns \a pathName with the '/' separators converted to
    separators that are appropriate for the underlying operating
    system.

    On Windows, convertSeparators("c:/winnt/system32") returns
    "c:\winnt\system32".

    The returned string may be the same as the argument on some
    operating systems, for example on Unix.
*/

QString QDir::convertSeparators( const QString &pathName )
{
    QString n( pathName );
#if defined(Q_FS_FAT) || defined(Q_OS_OS2EMX)
    for ( int i=0; i<(int)n.length(); i++ ) {
	if ( n[i] == '/' )
	    n[i] = '\\';
    }
#elif defined(Q_OS_MAC9)
	while(n.length() && n[0] == '/' ) n = n.right(n.length()-1);
    for ( int i=0; i<(int)n.length(); i++ ) {
	if ( n[i] == '/' )
	    n[i] = ':';
    }
    if(n.contains(':') && n.left(1) != ':')
	n.prepend(':');
#endif
    return n;
}


/*!
    Changes the QDir's directory to \a dirName.

    If \a acceptAbsPath is TRUE a path starting with separator "/"
    will cause the function to change to the absolute directory. If \a
    acceptAbsPath is FALSE any number of separators at the beginning
    of \a dirName will be removed and the function will descend into
    \a dirName.

    Returns TRUE if the new directory exists and is readable;
    otherwise returns FALSE. Note that the logical cd() operation is
    not performed if the new directory does not exist.

    Calling cd( ".." ) is equivalent to calling cdUp().

    \sa cdUp(), isReadable(), exists(), path()
*/

bool QDir::cd( const QString &dirName, bool acceptAbsPath )
{
    if ( dirName.isEmpty() || dirName==QString::fromLatin1(".") )
	return TRUE;
    QString old = dPath;
    if ( acceptAbsPath && !isRelativePath(dirName) ) {
	dPath = cleanDirPath( dirName );
    } else {
	if ( !isRoot() ) {
	    dPath += '/';
	} else if ( dirName == ".." ) {
	    dPath = old;
	    return FALSE;
	}
	dPath += dirName;
	if ( dirName.find('/') >= 0
		|| old == QString::fromLatin1(".")
		|| dirName == QString::fromLatin1("..") )
	    dPath = cleanDirPath( dPath );
    }
    if ( !exists() ) {
	dPath = old;			// regret
	return FALSE;
    }
    dirty = TRUE;
    return TRUE;
}

/*!
    Changes directory by moving one directory up from the QDir's
    current directory.

    Returns TRUE if the new directory exists and is readable;
    otherwise returns FALSE. Note that the logical cdUp() operation is
    not performed if the new directory does not exist.

    \sa cd(), isReadable(), exists(), path()
*/

bool QDir::cdUp()
{
    return cd( QString::fromLatin1("..") );
}

/*!
    \fn QString QDir::nameFilter() const

    Returns the string set by setNameFilter()
*/

/*!
    Sets the name filter used by entryList() and entryInfoList() to \a
    nameFilter.

    The \a nameFilter is a wildcard (globbing) filter that understands
    "*" and "?" wildcards. (See \link qregexp.html#wildcard-matching
    QRegExp wildcard matching\endlink.) You may specify several filter
    entries all separated by a single space " " or by a semi-colon
    ";".

    For example, if you want entryList() and entryInfoList() to list
    all files ending with either ".cpp" or ".h", you would use either
    dir.setNameFilter("*.cpp *.h") or dir.setNameFilter("*.cpp;*.h").

    \sa nameFilter(), setFilter()
*/

void QDir::setNameFilter( const QString &nameFilter )
{
    nameFilt = nameFilter;
    if ( nameFilt.isEmpty() )
	nameFilt = QString::fromLatin1("*");
    dirty = TRUE;
}

/*!
    \fn QDir::FilterSpec QDir::filter() const

    Returns the value set by setFilter()
*/

/*!
    \enum QDir::FilterSpec

    This enum describes the filtering options available to QDir, e.g.
    for entryList() and entryInfoList(). The filter value is specified
    by OR-ing together values from the following list:

    \value Dirs  List directories only.
    \value Files  List files only.
    \value  Drives  List disk drives (ignored under Unix).
    \value  NoSymLinks  Do not list symbolic links (ignored by operating
    systems that don't support symbolic links).
    \value All List directories, files, drives and symlinks (this does not list
    broken symlinks unless you specify System).
    \value TypeMask A mask for the the Dirs, Files, Drives and
    NoSymLinks flags.
    \value  Readable  List files for which the application has read access.
    \value  Writable  List files for which the application has write access.
    \value  Executable  List files for which the application has execute
    access. Executables needs to be combined with Dirs or Files.
    \value  RWEMask A mask for the Readable, Writable and Executable flags.
    \value  Modified  Only list files that have been modified (ignored
    under Unix).
    \value  Hidden  List hidden files (on Unix, files starting with a .).
    \value  System  List system files (on Unix, FIFOs, sockets and
    device files)
    \value AccessMask A mask for the Readable, Writable, Executable
    Modified, Hidden and System flags
    \value DefaultFilter Internal flag.

    If you do not set any of \c Readable, \c Writable or \c
    Executable, QDir will set all three of them. This makes the
    default easy to write and at the same time useful.

    Examples: \c Readable|Writable means list all files for which the
    application has read access, write access or both. \c Dirs|Drives
    means list drives, directories, all files that the application can
    read, write or execute, and also symlinks to such
    files/directories.
*/


/*!
    Sets the filter used by entryList() and entryInfoList() to \a
    filterSpec. The filter is used to specify the kind of files that
    should be returned by entryList() and entryInfoList(). See
    \l{QDir::FilterSpec}.

    \sa filter(), setNameFilter()
*/

void QDir::setFilter( int filterSpec )
{
    if ( filtS == (FilterSpec) filterSpec )
	return;
    filtS = (FilterSpec) filterSpec;
    dirty = TRUE;
}

/*!
    \fn QDir::SortSpec QDir::sorting() const

    Returns the value set by setSorting()

    \sa setSorting() SortSpec
*/

/*!
    \enum QDir::SortSpec

    This enum describes the sort options available to QDir, e.g. for
    entryList() and entryInfoList(). The sort value is specified by
    OR-ing together values from the following list:

    \value Name  Sort by name.
    \value Time  Sort by time (modification time).
    \value Size  Sort by file size.
    \value Unsorted  Do not sort.
    \value SortByMask A mask for Name, Time and Size.

    \value DirsFirst  Put the directories first, then the files.
    \value Reversed  Reverse the sort order.
    \value IgnoreCase  Sort case-insensitively.
    \value DefaultSort Internal flag.

    You can only specify one of the first four.

    If you specify both \c DirsFirst and \c Reversed, directories are
    still put first, but in reverse order; the files will be listed
    after the directories, again in reverse order.
*/

// ### Unsorted+DirsFirst ? Unsorted+Reversed?

/*!
    Sets the sort order used by entryList() and entryInfoList().

    The \a sortSpec is specified by OR-ing values from the enum
    \l{QDir::SortSpec}.

    \sa sorting() SortSpec
*/

void QDir::setSorting( int sortSpec )
{
    if ( sortS == (SortSpec) sortSpec )
	return;
    sortS = (SortSpec) sortSpec;
    dirty = TRUE;
}

/*!
    \fn bool QDir::matchAllDirs() const

    Returns the value set by setMatchAllDirs()

    \sa setMatchAllDirs()
*/

/*!
    If \a enable is TRUE then all directories are included (e.g. in
    entryList()), and the nameFilter() is only applied to the files.
    If \a enable is FALSE then the nameFilter() is applied to both
    directories and files.

    \sa matchAllDirs()
*/

void QDir::setMatchAllDirs( bool enable )
{
    if ( (bool)allDirs == enable )
	return;
    allDirs = enable;
    dirty = TRUE;
}


/*!
    Returns the total number of directories and files that were found.

    Equivalent to entryList().count().

    \sa operator[](), entryList()
*/

uint QDir::count() const
{
    return (uint)entryList().count();
}

/*!
    Returns the file name at position \a index in the list of file
    names. Equivalent to entryList().at(index).

    Returns a QString::null if the \a index is out of range or if the
    entryList() function failed.

    \sa count(), entryList()
*/

QString QDir::operator[]( int index ) const
{
    entryList();
    return fList && index >= 0 && index < (int)fList->count() ?
	(*fList)[index] : QString::null;
}


/*!
    \obsolete
  This function is included to easy porting from Qt 1.x to Qt 2.0,
  it is the same as entryList(), but encodes the filenames as 8-bit
  strings using QFile::encodedName().

  It is more efficient to use entryList().
*/
QStrList QDir::encodedEntryList( int filterSpec, int sortSpec ) const
{
    QStrList r;
    QStringList l = entryList(filterSpec,sortSpec);
    for ( QStringList::Iterator it = l.begin(); it != l.end(); ++it ) {
	r.append( QFile::encodeName(*it) );
    }
    return r;
}

/*!
    \obsolete
    \overload
  This function is included to easy porting from Qt 1.x to Qt 2.0,
  it is the same as entryList(), but encodes the filenames as 8-bit
  strings using QFile::encodedName().

  It is more efficient to use entryList().
*/
QStrList QDir::encodedEntryList( const QString &nameFilter,
			   int filterSpec,
			   int sortSpec ) const
{
    QStrList r;
    QStringList l = entryList(nameFilter,filterSpec,sortSpec);
    for ( QStringList::Iterator it = l.begin(); it != l.end(); ++it ) {
	r.append( QFile::encodeName(*it) );
    }
    return r;
}



/*!
    \overload

    Returns a list of the names of all the files and directories in
    the directory, ordered in accordance with setSorting() and
    filtered in accordance with setFilter() and setNameFilter().

    The filter and sorting specifications can be overridden using the
    \a filterSpec and \a sortSpec arguments.

    Returns an empty list if the directory is unreadable or does not
    exist.

    \sa entryInfoList(), setNameFilter(), setSorting(), setFilter()
*/

QStringList QDir::entryList( int filterSpec, int sortSpec ) const
{
    if ( !dirty && filterSpec == (int)DefaultFilter &&
		   sortSpec   == (int)DefaultSort )
	return *fList;
    return entryList( nameFilt, filterSpec, sortSpec );
}

/*!
    Returns a list of the names of all the files and directories in
    the directory, ordered in accordance with setSorting() and
    filtered in accordance with setFilter() and setNameFilter().

    The filter and sorting specifications can be overridden using the
    \a nameFilter, \a filterSpec and \a sortSpec arguments.

    Returns an empty list if the directory is unreadable or does not
    exist.

    \sa entryInfoList(), setNameFilter(), setSorting(), setFilter()
*/

QStringList QDir::entryList( const QString &nameFilter,
				 int filterSpec, int sortSpec ) const
{
    if ( filterSpec == (int)DefaultFilter )
	filterSpec = filtS;
    if ( sortSpec == (int)DefaultSort )
	sortSpec = sortS;
    QDir *that = (QDir*)this;			// mutable function
    if ( that->readDirEntries(nameFilter, filterSpec, sortSpec) ) {
	if ( that->fList )
	    return *that->fList;
    }
    return QStringList();
}

/*!
    \overload

    Returns a list of QFileInfo objects for all the files and
    directories in the directory, ordered in accordance with
    setSorting() and filtered in accordance with setFilter() and
    setNameFilter().

    The filter and sorting specifications can be overridden using the
    \a filterSpec and \a sortSpec arguments.

    Returns 0 if the directory is unreadable or does not exist.

    The returned pointer is a const pointer to a QFileInfoList. The
    list is owned by the QDir object and will be reused on the next
    call to entryInfoList() for the same QDir instance. If you want to
    keep the entries of the list after a subsequent call to this
    function you must copy them.

    \sa entryList(), setNameFilter(), setSorting(), setFilter()
*/

const QFileInfoList *QDir::entryInfoList( int filterSpec, int sortSpec ) const
{
    if ( !dirty && filterSpec == (int)DefaultFilter &&
		   sortSpec   == (int)DefaultSort )
	return fiList;
    return entryInfoList( nameFilt, filterSpec, sortSpec );
}

/*!
    Returns a list of QFileInfo objects for all the files and
    directories in the directory, ordered in accordance with
    setSorting() and filtered in accordance with setFilter() and
    setNameFilter().

    The filter and sorting specifications can be overridden using the
    \a nameFilter, \a filterSpec and \a sortSpec arguments.

    Returns 0 if the directory is unreadable or does not exist.

    The returned pointer is a const pointer to a QFileInfoList. The
    list is owned by the QDir object and will be reused on the next
    call to entryInfoList() for the same QDir instance. If you want to
    keep the entries of the list after a subsequent call to this
    function you must copy them.

    \sa entryList(), setNameFilter(), setSorting(), setFilter()
*/

const QFileInfoList *QDir::entryInfoList( const QString &nameFilter,
					  int filterSpec, int sortSpec ) const
{
    if ( filterSpec == (int)DefaultFilter )
	filterSpec = filtS;
    if ( sortSpec == (int)DefaultSort )
	sortSpec = sortS;
    QDir *that = (QDir*)this;			// mutable function
    if ( that->readDirEntries(nameFilter, filterSpec, sortSpec) )
	return that->fiList;
    else
	return 0;
}

/*!
    \overload

    Returns TRUE if the \e directory exists; otherwise returns FALSE.
    (If a file with the same name is found this function will return
    FALSE).

    \sa QFileInfo::exists(), QFile::exists()
*/

bool QDir::exists() const
{
    QFileInfo fi( dPath );
    return fi.exists() && fi.isDir();
}

/*!
    Returns TRUE if the directory path is relative to the current
    directory and returns FALSE if the path is absolute (e.g. under
    UNIX a path is relative if it does not start with a "/").

    \sa convertToAbs()
*/

bool QDir::isRelative() const
{
    return isRelativePath( dPath );
}

/*!
    Converts the directory path to an absolute path. If it is already
    absolute nothing is done.

    \sa isRelative()
*/

void QDir::convertToAbs()
{
    dPath = absPath();
}

/*!
    Makes a copy of QDir \a d and assigns it to this QDir.
*/

QDir &QDir::operator=( const QDir &d )
{
    dPath    = d.dPath;
    delete fList;
    fList    = 0;
    delete fiList;
    fiList   = 0;
    nameFilt = d.nameFilt;
    dirty    = TRUE;
    allDirs  = d.allDirs;
    filtS    = d.filtS;
    sortS    = d.sortS;
    return *this;
}

/*!
    \overload

    Sets the directory path to be the given \a path.
*/

QDir &QDir::operator=( const QString &path )
{
    dPath = cleanDirPath( path );
    dirty = TRUE;
    return *this;
}


/*!
    \fn bool QDir::operator!=( const QDir &d ) const

    Returns TRUE if directory \a d and this directory have different
    paths or different sort or filter settings; otherwise returns
    FALSE.

    Example:
    \code
    // The current directory is "/usr/local"
    QDir d1( "/usr/local/bin" );
    QDir d2( "bin" );
    if ( d1 != d2 ) qDebug( "They differ\n" ); // This is printed
    \endcode
*/

/*!
    Returns TRUE if directory \a d and this directory have the same
    path and their sort and filter settings are the same; otherwise
    returns FALSE.

    Example:
    \code
    // The current directory is "/usr/local"
    QDir d1( "/usr/local/bin" );
    QDir d2( "bin" );
    d2.convertToAbs();
    if ( d1 == d2 ) qDebug( "They're the same\n" ); // This is printed
    \endcode
*/

bool QDir::operator==( const QDir &d ) const
{
    return dPath    == d.dPath &&
	   nameFilt == d.nameFilt &&
	   allDirs  == d.allDirs &&
	   filtS    == d.filtS &&
	   sortS    == d.sortS;
}


/*!
    Removes the file, \a fileName.

    If \a acceptAbsPath is TRUE a path starting with separator "/"
    will remove the file with the absolute path. If \a acceptAbsPath
    is FALSE any number of separators at the beginning of \a fileName
    will be removed and the resultant file name will be removed.

    Returns TRUE if the file is removed successfully; otherwise
    returns FALSE.
*/

bool QDir::remove( const QString &fileName, bool acceptAbsPath )
{
    if ( fileName.isEmpty() ) {
#if defined(QT_CHECK_NULL)
	qWarning( "QDir::remove: Empty or null file name" );
#endif
	return FALSE;
    }
    QString p = filePath( fileName, acceptAbsPath );
    return QFile::remove( p );
}

/*!
    Checks for the existence of the file \a name.

    If \a acceptAbsPath is TRUE a path starting with separator "/"
    will check the file with the absolute path. If \a acceptAbsPath is
    FALSE any number of separators at the beginning of \a name will be
    removed and the resultant file name will be checked.

    Returns TRUE if the file exists; otherwise returns FALSE.

    \sa QFileInfo::exists(), QFile::exists()
*/

bool QDir::exists( const QString &name, bool acceptAbsPath ) //### const in 4.0
{
    if ( name.isEmpty() ) {
#if defined(QT_CHECK_NULL)
	qWarning( "QDir::exists: Empty or null file name" );
#endif
	return FALSE;
    }
    QString tmp = filePath( name, acceptAbsPath );
    return QFile::exists( tmp );
}

/*!
    Returns the native directory separator; "/" under UNIX (including
    Mac OS X) and "\" under Windows.

    You do not need to use this function to build file paths. If you
    always use "/", Qt will translate your paths to conform to the
    underlying operating system.
*/

char QDir::separator()
{
#if defined(Q_OS_UNIX)
    return '/';
#elif defined (Q_FS_FAT) || defined(Q_WS_WIN)
    return '\\';
#elif defined (Q_OS_MAC)
    return ':';
#else
    return '/';
#endif
}

/*!
    Returns the application's current directory.

    Use path() to access a QDir object's path.

    \sa currentDirPath(), QDir::QDir()
*/

QDir QDir::current()
{
    return QDir( currentDirPath() );
}

/*!
    Returns the home directory.

    Under Windows the \c HOME environment variable is used. If this
    does not exist the \c USERPROFILE environment variable is used. If
    that does not exist the path is formed by concatenating the \c
    HOMEDRIVE and \c HOMEPATH environment variables. If they don't
    exist the rootDirPath() is used (this uses the \c SystemDrive
    environment variable). If none of these exist "C:\" is used.

    Under non-Windows operating systems the \c HOME environment
    variable is used if it exists, otherwise rootDirPath() is used.

    \sa homeDirPath()
*/

QDir QDir::home()
{
    return QDir( homeDirPath() );
}

/*!
    Returns the root directory.

    \sa rootDirPath() drives()
*/

QDir QDir::root()
{
    return QDir( rootDirPath() );
}

/*!
    \fn QString QDir::homeDirPath()

    Returns the absolute path of the user's home directory.

    \sa home()
*/

QStringList qt_makeFilterList( const QString &filter )
{
    if ( filter.isEmpty() )
	return QStringList();

    QChar sep( ';' );
    int i = filter.find( sep, 0 );
    if ( i == -1 && filter.find( ' ', 0 ) != -1 )
	sep = QChar( ' ' );

    QStringList list = QStringList::split( sep, filter );
    QStringList::Iterator it = list.begin();
    QStringList list2;

    for ( ; it != list.end(); ++it ) {
	QString s = *it;
	list2 << s.stripWhiteSpace();
    }
    return list2;
}

/*!
    \overload

    Returns TRUE if the \a fileName matches any of the wildcard (glob)
    patterns in the list of \a filters; otherwise returns FALSE.

    (See \link qregexp.html#wildcard-matching QRegExp wildcard
    matching.\endlink)
    \sa QRegExp::match()
*/

bool QDir::match( const QStringList &filters, const QString &fileName )
{
    QStringList::ConstIterator sit = filters.begin();
    while ( sit != filters.end() ) {
#if defined(Q_FS_FAT) && !defined(Q_OS_UNIX)
	QRegExp rx( *sit, FALSE, TRUE ); // The FAT FS is not case sensitive..
#else
	QRegExp rx( *sit, TRUE, TRUE );  // ..while others are.
#endif
	if ( rx.exactMatch(fileName) )
	    return TRUE;
	++sit;
    }
    return FALSE;
}

/*!
    Returns TRUE if the \a fileName matches the wildcard (glob)
    pattern \a filter; otherwise returns FALSE. The \a filter may
    contain multiple patterns separated by spaces or semicolons.

    (See \link qregexp.html#wildcard-matching QRegExp wildcard
    matching.\endlink)
    \sa QRegExp::match()
*/

bool QDir::match( const QString &filter, const QString &fileName )
{
    QStringList lst = qt_makeFilterList( filter );
    return match( lst, fileName );
}


/*!
    Removes all multiple directory separators "/" and resolves any
    "."s or ".."s found in the path, \a filePath.

    Symbolic links are kept. This function does not return the
    canonical path, but rather the simplest version of the input.
    For example, "./local" becomes "local", "local/../bin" becomes
    "bin" and "/local/usr/../bin" becomes "/local/bin".

    \sa absPath() canonicalPath()
*/

QString QDir::cleanDirPath( const QString &filePath )
{
    QString name = filePath;
    QString newPath;

    if ( name.isEmpty() )
	return name;

    slashify( name );

    bool addedSeparator;
    if ( isRelativePath(name) ) {
	addedSeparator = TRUE;
	name.insert( 0, '/' );
    } else {
	addedSeparator = FALSE;
    }

    int ePos, pos, upLevel;

    pos = ePos = name.length();
    upLevel = 0;
    int len;

    while ( pos && (pos = name.findRev('/',--pos)) != -1 ) {
	len = ePos - pos - 1;
	if ( len == 2 && name.at(pos + 1) == '.'
		      && name.at(pos + 2) == '.' ) {
	    upLevel++;
	} else {
	    if ( len != 0 && (len != 1 || name.at(pos + 1) != '.') ) {
		if ( !upLevel )
		    newPath = QString::fromLatin1("/")
			+ name.mid(pos + 1, len) + newPath;
		else
		    upLevel--;
	    }
	}
	ePos = pos;
    }
    if ( addedSeparator ) {
	while ( upLevel-- )
	    newPath.insert( 0, QString::fromLatin1("/..") );
	if ( !newPath.isEmpty() )
	    newPath.remove( 0, 1 );
	else
	    newPath = QString::fromLatin1(".");
    } else {
	if ( newPath.isEmpty() )
	    newPath = QString::fromLatin1("/");
#if defined(Q_FS_FAT) || defined(Q_OS_OS2EMX)
	if ( name[0] == '/' ) {
	    if ( name[1] == '/' )		// "\\machine\x\ ..."
		newPath.insert( 0, '/' );
	} else {
	    newPath = name.left(2) + newPath;
	}
#endif
    }
    return newPath;
}

int qt_cmp_si_sortSpec;

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

#ifdef Q_OS_TEMP
int __cdecl qt_cmp_si( const void *n1, const void *n2 )
#else
int qt_cmp_si( const void *n1, const void *n2 )
#endif
{
    if ( !n1 || !n2 )
	return 0;

    QDirSortItem* f1 = (QDirSortItem*)n1;
    QDirSortItem* f2 = (QDirSortItem*)n2;

    if ( qt_cmp_si_sortSpec & QDir::DirsFirst )
	if ( f1->item->isDir() != f2->item->isDir() )
	    return f1->item->isDir() ? -1 : 1;

    int r = 0;
    int sortBy = qt_cmp_si_sortSpec & QDir::SortByMask;

    switch ( sortBy ) {
      case QDir::Time:
	r = f1->item->lastModified().secsTo(f2->item->lastModified());
	break;
      case QDir::Size:
	r = f2->item->size() - f1->item->size();
	break;
      default:
	;
    }

    if ( r == 0 && sortBy != QDir::Unsorted ) {
	// Still not sorted - sort by name
	bool ic = qt_cmp_si_sortSpec & QDir::IgnoreCase;

	if ( f1->filename_cache.isNull() )
	    f1->filename_cache = ic ? f1->item->fileName().lower()
				    : f1->item->fileName();
	if ( f2->filename_cache.isNull() )
	    f2->filename_cache = ic ? f2->item->fileName().lower()
				    : f2->item->fileName();

	r = f1->filename_cache.compare(f2->filename_cache);
    }

    if ( r == 0 ) {
	// Enforce an order - the order the items appear in the array
	r = (char*)n1 - (char*)n2;
    }

    if ( qt_cmp_si_sortSpec & QDir::Reversed )
	return -r;
    else
	return r;
}

#if defined(Q_C_CALLBACKS)
}
#endif

#endif // QT_NO_DIR
