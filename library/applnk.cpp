/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "applnk.h"

#include <qpe/qpeapplication.h>
#include <qpe/categories.h>
#include <qpe/categoryselect.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/global.h>
#include <qpe/mimetype.h>
#include <qpe/config.h>
#include <qpe/storage.h>
#include <qpe/resource.h>

#include <qdict.h>
#include <qdir.h>
#include <qregexp.h>

#ifdef Q_WS_QWS
#include <qgfx_qws.h>
#endif

#include <stdlib.h>

int AppLnk::lastId = 5000;

static int smallSize = 14;
static int bigSize = 32;

static QString safeFileName(const QString& n)
{
    QString safename=n;
    safename.replace(QRegExp("[^0-9A-Za-z.]"),"_");
    safename.replace(QRegExp("^[^A-Za-z]*"),"");
    if ( safename.isEmpty() )
	safename = "_";
    return safename;
}


class AppLnkPrivate
{
public:
    QArray<int> mCat;
};

/*!
  \class AppLnk applnk.h
  \brief The AppLnk class represents an application available on the system.

  Information about applications are stored in Qtopia as ".desktop" files.
  When read, these files are stored as AppLnk objects.
*/

/*!
  Sets the size used for small icons to \a small pixels.
  Only affects AppLnk objects created after the call.
*/
void AppLnk::setSmallIconSize(int small)
{
    smallSize = small;
}

/*!
  Returns the size used for small icons.
*/
int AppLnk::smallIconSize()
{
    return smallSize;
}


/*!
  Sets the size used for large icons to \a big pixels.
  Only affects AppLnk objects created after the call.
*/
void AppLnk::setBigIconSize(int big)
{
    bigSize = big;
}

/*!
  Returns the size used for large icons.
*/
int AppLnk::bigIconSize()
{
    return bigSize;
}


/*!
  \fn QString AppLnk::name() const

  Returns the Name property.
*/
/*!
    \fn QString AppLnk::exec() const

  Returns the Exec property. This is the executable program associated
  with the AppLnk.
*/
/*!
    \fn QString AppLnk::rotation() const

  Returns the Rotation property.
*/
/*!
    \fn QString AppLnk::comment() const

  Returns the Comment property.
*/
/*!
    \fn QStringList AppLnk::mimeTypes() const

  Returns the MimeTypes property. This is the list of MIME types
  that the application can view or edit.
*/
/*!
    \fn const QArray<int>& AppLnk::categories() const

  Returns the Categories property.
*/

const QArray<int>& AppLnk::categories() const
{
    return d->mCat;
}

/*!
    \fn int AppLnk::id() const

  Returns the id of the AppLnk. If the AppLnk is not in an AppLnkSet,
  this value is 0, otherwise it is a value that is unique for the
  duration of the current process.
*/

/*!
  \fn bool AppLnk::isValid() const

  Returns whether this AppLnk is valid.
*/

/*!
  Creates an invalid AppLnk.

  \sa isValid()
*/
AppLnk::AppLnk()
{
    mId = 0;
    d = new AppLnkPrivate();
}

/*!
  Loads \a file as an AppLnk.
*/
AppLnk::AppLnk( const QString &file )
{
    QStringList sl;
    d = new AppLnkPrivate();
    if ( !file.isNull() ) {
	Config config( file, Config::File );

	if ( config.isValid() ) {
	    config.setGroup( "Desktop Entry" );

	    mName = config.readEntry( "Name", file );
	    mExec = config.readEntry( "Exec" );
	    mType = config.readEntry( "Type", QString::null );
	    mIconFile = config.readEntry( "Icon", QString::null );
	    mRotation = config.readEntry( "Rotation", "" );
	    mComment = config.readEntry( "Comment", QString::null );
	    mMimeTypes = config.readListEntry( "MimeType", ';' );
	    mMimeTypeIcons = config.readListEntry( "MimeTypeIcons", ';' );
	    mLinkFile = file;
	    mFile = config.readEntry("File", QString::null);
	    sl = config.readListEntry("Categories", ';');
	}
    }
    // let's try our darndest to create categories...
    Categories cat( 0 );
    cat.load( categoryFileName() );
    d->mCat.resize( sl.count() );
    int i;
    QStringList::Iterator it;
    for ( i = 0, it = sl.begin(); it != sl.end();
	  ++it, i++ ) {

	bool number;
	int id = (*it).toInt( &number );
	if ( !number ) {
	    // convert from old school...
	    id = cat.id( "Document Viewer", *it );
	    if ( id == 0 )
		id = cat.addCategory( "Document Viewer", *it );
	}
	d->mCat[i] = id;
    }
    mId = 0;
}

/*!
  Returns a small pixmap associated with the application.
*/
const QPixmap& AppLnk::pixmap() const
{
    if ( mPixmap.isNull() ) {
	AppLnk* that = (AppLnk*)this;
	if ( mIconFile.isEmpty() ) {
	    MimeType mt(type());
	    that->mPixmap = mt.pixmap();
	    if ( that->mPixmap.isNull() )
		that->mPixmap = Resource::loadPixmap("UnknownDocument-14");
	    return that->mPixmap;
	}
	QImage unscaledIcon = Resource::loadImage( that->mIconFile );
	that->mPixmap.convertFromImage( unscaledIcon.smoothScale( smallSize, smallSize ) );
	that->mBigPixmap.convertFromImage( unscaledIcon.smoothScale( bigSize, bigSize ) );
	return that->mPixmap;
    }
    return mPixmap;
}

/*!
  Returns a large pixmap associated with the application.
*/
const QPixmap& AppLnk::bigPixmap() const
{
    if ( mBigPixmap.isNull() ) {
	AppLnk* that = (AppLnk*)this;
	if ( mIconFile.isEmpty() ) {
	    MimeType mt(type());
	    that->mBigPixmap = mt.bigPixmap();
	    if ( that->mBigPixmap.isNull() )
		that->mBigPixmap = Resource::loadPixmap("UnknownDocument");
	    return that->mBigPixmap;
	}
	QImage unscaledIcon = Resource::loadImage( that->mIconFile );
	that->mPixmap.convertFromImage( unscaledIcon.smoothScale( smallSize, smallSize ) );
	that->mBigPixmap.convertFromImage( unscaledIcon.smoothScale( bigSize, bigSize ) );
	return that->mBigPixmap;
    }
    return mBigPixmap;
}

/*!
  Returns the type of the application.
*/
QString AppLnk::type() const
{
    if ( mType.isNull() ) {
	AppLnk* that = (AppLnk*)this;
	MimeType mt(file());
	that->mType = mt.id();
	return that->mType;
    }
    return mType;
}

/*!
  Returns the file associated with the AppLnk.

  \sa exec()
*/
QString AppLnk::file() const
{
    if ( mFile.isNull() ) {
	AppLnk* that = (AppLnk*)this;
	if ( !mLinkFile.isEmpty() ) {
	    that->mFile =
		mLinkFile.right(8)==".desktop" // 8 = strlen(".desktop")
		    ? mLinkFile.left(mLinkFile.length()-8) : mLinkFile;
	} else if ( mType.contains('/') ) {
	    that->mFile =
		QString(getenv("HOME"))+"/Documents/"+mType+"/"+safeFileName(that->mName);
	    if ( QFile::exists(that->mFile) || QFile::exists(that->mFile+".desktop") ) {
		int n=1;
		QString nn;
		while (QFile::exists((nn=(that->mFile+"_"+QString::number(n))))
			|| QFile::exists(nn+".desktop"))
		    n++;
		that->mFile = nn;
	    }
	    that->mLinkFile = that->mFile+".desktop";
	}
	return that->mFile;
    }
    return mFile;
}

/*!
  Returns the desktop file coresponding to this AppLnk.

  \sa file(), exec()
*/
QString AppLnk::linkFile() const
{
    if ( mLinkFile.isNull() ) {
	AppLnk* that = (AppLnk*)this;
	if ( type().contains('/') ) {
	    StorageInfo storage;
	    const FileSystem *fs = storage.fileSystemOf( that->mFile );
// 	    qDebug("creating lnkFile for %s", mFile.latin1() );
// 	    if ( fs )
// 		qDebug("fs is %s", fs->path().latin1() );
	    if ( fs && fs->isRemovable() ) {
//		qDebug("isRemovable");
		that->mLinkFile = fs->path();
	    } else
		that->mLinkFile = getenv( "HOME" );
	    that->mLinkFile += "/Documents/"+type()+"/"+safeFileName(that->mName);
	    if ( QFile::exists(that->mLinkFile+".desktop") ) {
		int n=1;
		QString nn;
		while (QFile::exists((nn=that->mLinkFile+"_"+QString::number(n))+".desktop"))
		    n++;
		that->mLinkFile = nn;
	    }
	    that->mLinkFile += ".desktop";
//	    qDebug("file is %s", mLinkFile.latin1() );
	}
	return that->mLinkFile;
    }
    return mLinkFile;
}

/*!
  Copies \a copy.
*/
AppLnk::AppLnk( const AppLnk &copy )
{
    mName = copy.mName;
    mPixmap = copy.mPixmap;
    mBigPixmap = copy.mBigPixmap;
    mExec = copy.mExec;
    mType = copy.mType;
    mRotation = copy.mRotation;
    mComment = copy.mComment;
    mFile = copy.mFile;
    mLinkFile = copy.mLinkFile;
    mIconFile = copy.mIconFile;
    mMimeTypes = copy.mMimeTypes;
    mMimeTypeIcons = copy.mMimeTypeIcons;
    mId = 0;
    d = new AppLnkPrivate();
    d->mCat = copy.d->mCat;
}

/*!
  Destroys the AppLnk. Note that if the AppLnk is current a member of
  an AppLnkSet, this will produce a run-time warning.

  \sa AppLnkSet::add(), AppLnkSet::remove()
*/
AppLnk::~AppLnk()
{
    if ( mId )
	qWarning("Deleting AppLnk that is in an AppLnkSet");
    if ( d )
	delete d;
}

/*!
  Executes the application associated with this AppLnk.
*/
void AppLnk::execute() const
{
    execute(QStringList());
}

/*!
  Executes the application associated with this AppLnk, with
  \a args as arguments.
*/
void AppLnk::execute(const QStringList& args) const
{
#ifdef Q_WS_QWS
    if ( !mRotation.isEmpty() ) {
	// ######## this will only work in the server
	int rot = QPEApplication::defaultRotation();
	rot = (rot+mRotation.toInt())%360;
	QCString old = getenv("QWS_DISPLAY");
	setenv("QWS_DISPLAY", QString("Transformed:Rot%1:0").arg(rot), 1);
	invoke(args);
	setenv("QWS_DISPLAY", old.data(), 1);
    } else
#endif
	invoke(args);
}

/*!
  Invokes the application associated with this AppLnk, with
  \a args as arguments. Rotation is not taken into account by
  this function, you should not call it directly.

  \sa execute()
*/
void AppLnk::invoke(const QStringList& args) const
{
    Global::execute( exec(), args[0] );
}

/*!
  Sets the Exec property to \a exec.

  \sa exec()
*/
void AppLnk::setExec( const QString& exec )
{
    mExec = exec;
}

/*!
  Sets the Name property to \a docname.

  \sa name()
*/
void AppLnk::setName( const QString& docname )
{
    mName = docname;
}

/*!
  Sets the File property to \a filename.

  \sa file()
*/
void AppLnk::setFile( const QString& filename )
{
    mFile = filename;
}

/*!
  Sets the LinkFile property to \a filename.

  \sa linkFile()
*/
void AppLnk::setLinkFile( const QString& filename )
{
    mLinkFile = filename;
}

/*!
  Sets the Comment property to \a comment.

  \sa comment()
*/
void AppLnk::setComment( const QString& comment )
{
    mComment = comment;
}

/*!
  Sets the Type property to \a type.

  \sa type()
*/
void AppLnk::setType( const QString& type )
{
    mType = type;
}

/*!
  Sets the Icon property to \a iconname.

  \sa pixmap(), bigPixmap()
*/
void AppLnk::setIcon( const QString& iconname )
{
    mIconFile = iconname;
    QImage unscaledIcon = Resource::loadImage( mIconFile );
    mPixmap.convertFromImage( unscaledIcon.smoothScale( smallSize, smallSize ) );
    mBigPixmap.convertFromImage( unscaledIcon.smoothScale( bigSize, bigSize ) );
}

/*!
  Sets the Categories property to \a c.

  \sa categories()
*/
void AppLnk::setCategories( const QArray<int>& c )
{
    d->mCat = c;
}

/*!
  \fn QStringList AppLnk::mimeTypeIcons() const

  Returns the MimeTypeIcons property of the AppLnk.
*/

/*!
  Attempts to ensure that the link file for this AppLnk exists, including
  creating any required directories. Returns TRUE if successful.
*/
bool AppLnk::ensureLinkExists() const
{
    QString lf = linkFile();
    if ( !QFile::exists(lf) ) {
	// May need to create directories
	QFileInfo fi(lf);
	if ( system(("mkdir -p "+fi.dirPath(TRUE))) )
	    return FALSE;
    }
    return TRUE;
}

/*!
  Commits the AppLnk to disk. Returns whether the operation succeeded.

  The "linkChanged(QString)" message is sent to the
  "QPE/System" QCop channel as a result.
*/
bool AppLnk::writeLink() const
{
    // Only re-writes settable parts
    QString lf = linkFile();
    if ( !ensureLinkExists() )
	return FALSE;
    Config config( lf, Config::File );
    config.setGroup("Desktop Entry");
    config.writeEntry("Name",mName);
    if ( !mIconFile.isNull() ) config.writeEntry("Icon",mIconFile);
    config.writeEntry("Type",type());
    if ( !mComment.isNull() ) config.writeEntry("Comment",mComment);
    config.writeEntry("File",file());
    // write out the id...
    int i;
    QStringList sl;
    for ( i = 0; i < int(d->mCat.count()); i++ ) {
	sl.append( QString::number( d->mCat[i] ) );
    }
    config.writeEntry( "Categories", sl, ';' );

    QCopEnvelope e("QPE/System", "linkChanged(QString)");
    e << lf;

    return TRUE;
}

/*!
  Sets the property named \a key to \a value.
*/
void AppLnk::setProperty(const QString& key, const QString& value)
{
    if ( ensureLinkExists() ) {
	Config cfg(linkFile(), Config::File);
	cfg.writeEntry(key,value);
    }
}

/*!
  Returns the property named \a key.
*/
QString AppLnk::property(const QString& key) const
{
    QString lf = linkFile();
    if ( !QFile::exists(lf) )
	return QString::null;
    Config cfg(lf, Config::File);
    return cfg.readEntry(key);
}


/*!
  Deletes both the linkFile() and file() associated with this AppLnk.
*/
void AppLnk::removeFiles()
{
    bool valid = isValid();
    if ( !valid || QFile::remove(linkFile()) ) {
	if ( QFile::remove(file()) ) {
	    QCopEnvelope e("QPE/System", "linkChanged(QString)");
	    e << linkFile();
	} else if ( valid ) {
	    // restore link
	    writeLink();
	}
    }
}

/*!
  Delete the linkFile(), leaving any file() untouched.
*/
void AppLnk::removeLinkFile()
{
    if ( isValid() && QFile::remove(linkFile()) ) {
	QCopEnvelope e("QPE/System", "linkChanged(QString)");
	e << linkFile();
    }
}

class AppLnkSetPrivate {
public:
    AppLnkSetPrivate()
    {
	typPix.setAutoDelete(TRUE);
	typPixBig.setAutoDelete(TRUE);
	typName.setAutoDelete(TRUE);
    }

    QDict<QPixmap> typPix;
    QDict<QPixmap> typPixBig;
    QDict<QString> typName;
};

/*!
  \class AppLnkSet applnk.h
  \brief The AppLnkSet class is a set of AppLnk objects.
*/

/*!
  \fn QStringList AppLnkSet::types() const

  Returns the list of types in the set.

  \sa AppLnk::type(), typeName(), typePixmap(), typeBigPixmap()
*/

/*!
  \fn const QList<AppLnk>& AppLnkSet::children() const

  Returns the members of the set.
*/

/*!
  Constructs an empty AppLnkSet.
*/
AppLnkSet::AppLnkSet() :
    d(new AppLnkSetPrivate)
{
}

/*!
  Constructs an AppLnkSet that contains AppLnk objects representing
  all the files in a \a directory (recursively).

  The directories may contain ".directory" files which overrides
  any AppLnk::type() value of AppLnk objects found in the directory.
  This allows simple localization of application types.
*/
AppLnkSet::AppLnkSet( const QString &directory ) :
    d(new AppLnkSetPrivate)
{
    QDir dir( directory );
    mFile = directory;
    findChildren(directory,QString::null,QString::null);
}

/*!
  Detaches all AppLnk objects from the set. The set become empty
  and the call becomes responsible for deleting the AppLnk objects.
*/
void AppLnkSet::detachChildren()
{
    QListIterator<AppLnk> it( mApps );
    for ( ; it.current(); ) {
	AppLnk* a = *it;
	++it;
	a->mId = 0;
    }
    mApps.clear();
}

/*!
  Destroys the set, deleting all AppLnk objects it contains.

  \sa detachChildren()
*/
AppLnkSet::~AppLnkSet()
{
    QListIterator<AppLnk> it( mApps );
    for ( ; it.current(); ) {
	AppLnk* a = *it;
	++it;
	a->mId = 0;
	delete a;
    }
    delete d;
}

void AppLnkSet::findChildren(const QString &dr, const QString& typ, const QString& typName, int depth)
{
    depth++;
    if ( depth > 10 )
	return;

    QDir dir( dr );
    QString typNameLocal = typName;

    if ( dir.exists( ".directory" ) ) {
	Config config( dr + "/.directory", Config::File );
	config.setGroup( "Desktop Entry" );
	typNameLocal = config.readEntry( "Name", typNameLocal );
	if ( !typ.isEmpty() ) {
	    QString iconFile = config.readEntry( "Icon", "AppsIcon" );
	    QImage unscaledIcon = Resource::loadImage( iconFile );
	    QPixmap pm, bpm;
	    pm.convertFromImage( unscaledIcon.smoothScale( smallSize, smallSize ) );
	    bpm.convertFromImage( unscaledIcon.smoothScale( bigSize, bigSize ) );
	    d->typPix.insert(typ, new QPixmap(pm));
	    d->typPixBig.insert(typ, new QPixmap(bpm));
	    d->typName.insert(typ, new QString(typNameLocal));
	}
    }

    const QFileInfoList *list = dir.entryInfoList();
    if ( list ) {
	QFileInfo* fi;
	bool cadded=FALSE;
	for ( QFileInfoListIterator it(*list); (fi=*it); ++it ) {
	    QString bn = fi->fileName();
	    if ( bn[0] != '.' && bn != "CVS" ) {
		if ( fi->isDir() ) {
		    QString c = typ.isNull() ? bn : typ+"/"+bn;
		    QString d = typNameLocal.isNull() ? bn : typNameLocal+"/"+bn;
		    findChildren(fi->filePath(), c, d, depth );
		} else {
		    if ( fi->extension(FALSE) == "desktop" ) {
			AppLnk* app = new AppLnk( fi->filePath() );
#ifdef QT_NO_QWS_MULTIPROCESS
			if ( !Global::isBuiltinCommand( app->exec() ) )
			    delete app;
			else
#endif
			{
			    if ( !typ.isEmpty() ) {
				if ( !cadded ) {
				    typs.append(typ);
				    cadded = TRUE;
				}
				app->setType(typ);
			    }
			    add(app);
			}
		    }
		}
	    }
	}
    }
}

/*!
  Adds \a f to the set. The set takes over responsibility for deleting \a f.

  \sa remove()
*/
void AppLnkSet::add( AppLnk *f )
{
    if ( f->mId == 0 ) {
	AppLnk::lastId++;
	f->mId = AppLnk::lastId;
	mApps.append( f );
    } else {
	qWarning("Attempt to add an AppLnk twice");
    }
}

/*!
  Removes \a f to the set, returning whether \a f was in the set.
  The caller becomes responsible for deleting \a f.

  \sa add()
*/
bool AppLnkSet::remove( AppLnk *f )
{
    if ( mApps.remove( f ) ) {
	f->mId = 0;
	return TRUE;
    }
    return FALSE;
}

/*!
  Returns the localized name for type \a t.
*/
QString AppLnkSet::typeName( const QString& t ) const
{
    QString *st = d->typName.find(t);
    return st ? *st : QString::null;
}

/*!
  Returns the small pixmap associated with type \a t.
*/
QPixmap AppLnkSet::typePixmap( const QString& t ) const
{
    QPixmap *pm = d->typPix.find(t);
    return pm ? *pm : QPixmap();
}

/*!
  Returns the large pixmap associated with type \a t.
*/
QPixmap AppLnkSet::typeBigPixmap( const QString& t ) const
{
    QPixmap *pm = d->typPixBig.find(t);
    return pm ? *pm : QPixmap();
}

/*!
  Returns the AppLnk with the given \a id.
*/
const AppLnk *AppLnkSet::find( int id ) const
{
    QListIterator<AppLnk> it( children() );

    for ( ; it.current(); ++it ) {
	const AppLnk *app = it.current();
	if ( app->id() == id )
	    return app;
    }

    return 0;
}

/*!
  Returns the AppLnk with the given \a exec attribute.
*/
const AppLnk *AppLnkSet::findExec( const QString& exec ) const
{
    QListIterator<AppLnk> it( children() );

    for ( ; it.current(); ++it ) {
	const AppLnk *app = it.current();
	if ( app->exec() == exec )
	    return app;
    }

    return 0;
}

/*!
  \class DocLnkSet applnk.h
  \brief The DocLnkSet class is a set of DocLnk objects.
*/

/*!
  \fn const QList<DocLnk>& DocLnkSet::children() const

  Returns the members of the set.
*/

/*!
  Constructs an empty DocLnkSet.

  \sa appendFrom()
*/
DocLnkSet::DocLnkSet()
{
}

/*!
  Constructs an DocLnkSet that contains DocLnk objects representing
  all the files in a \a directory (recursively).

  If \a mimefilter is not null,
  only documents with a MIME type matching \a mimefilter are selected.
  The value may contain multiple wild-card patterns separated by ";",
  such as "*o/mpeg;audio/x-wav".
*/
DocLnkSet::DocLnkSet( const QString &directory, const QString& mimefilter ) :
    AppLnkSet()
{
    QDir dir( directory );
    mFile = dir.dirName();
    QDict<void> reference;

    QStringList subFilter = QStringList::split(";", mimefilter);
    QValueList<QRegExp> mimeFilters;
    for( QStringList::Iterator it = subFilter.begin(); it != subFilter.end(); ++ it )
	mimeFilters.append( QRegExp(*it, FALSE, TRUE) );

    findChildren(directory, mimeFilters, reference);

    const QList<DocLnk> &list = children();
    for ( QListIterator<DocLnk> it( list ); it.current(); ++it ) {
	reference.remove( (*it)->file() );
    }

    for ( QDictIterator<void> dit(reference); dit.current(); ++dit ) {
	if ( dit.current() == (void*)2 ) {
	    // Unreferenced, make an unwritten link
	    DocLnk* dl = new DocLnk;
	    QFileInfo fi( dit.currentKey() );
	    dl->setFile(fi.filePath());
	    dl->setName(fi.baseName());
	    // #### default to current path?
	    // dl->setCategories( ... );
	    bool match = mimefilter.isNull();
	    if ( !match )
		for( QValueList<QRegExp>::Iterator it = mimeFilters.begin(); it != mimeFilters.end() && !match; ++ it )
		    if ( (*it).match(dl->type()) >= 0 )
			match = TRUE;
	    if ( match /* && dl->type() != "application/octet-stream" */
		    && !!dl->exec() )
		add(dl);
	    else
		delete dl;
	}
    }
}

// other becomes empty
/*!
  Transfers all DocLnk objects from \a other to this set. \a other becomes
  empty.
*/
void DocLnkSet::appendFrom( DocLnkSet& other )
{
    if ( &other == this )
	return;
    QListIterator<AppLnk> it( other.mApps );
    for ( ; it.current(); ) {
	mApps.append(*it);
	++it;
    }
    other.mApps.clear();
}

void DocLnkSet::findChildren(const QString &dr, const QValueList<QRegExp> &mimeFilters, QDict<void> &reference, int depth)
{
    depth++;
    if ( depth > 10 )
	return;

    QDir dir( dr );

    const QFileInfoList *list = dir.entryInfoList();
    if ( list ) {
	QFileInfo* fi;
	for ( QFileInfoListIterator it(*list); (fi=*it); ++it ) {
	    QString bn = fi->fileName();
	    if ( bn[0] != '.' && bn != "CVS" ) {
		if ( fi->isDir() ) {
		    findChildren(fi->filePath(), mimeFilters, reference, depth);
		} else {
		    if ( fi->extension(FALSE) == "desktop" ) {
			DocLnk* dl = new DocLnk( fi->filePath() );
			QFileInfo fi2(dl->file());
			bool match = FALSE;
			if ( !fi2.exists() ) {
			    dir.remove( dl->file() );
			}
			if ( mimeFilters.count() == 0 ) {
			    add( dl );
			    match = TRUE;
			} else {
			    for( QValueList<QRegExp>::ConstIterator it = mimeFilters.begin(); it != mimeFilters.end(); ++ it ) {
				if ( (*it).match(dl->type()) >= 0 ) {
				    add(dl);
				    match = TRUE;
				}
			    }
			}
			if ( !match )
			    delete dl;
		    } else {
			if ( !reference.find(fi->fileName()) )
			    reference.insert(fi->filePath(), (void*)2);
		    }
		}
	    }
	}
    }
}

/*!
  \class DocLnk applnk.h
  \brief The DocLnk class represents loaded document references.
*/

/*!
  \fn DocLnk::DocLnk( const DocLnk &o )

  Copies \a o.
*/

/*!
  Constructs a DocLnk from a valid .desktop \a file or a new .desktop
  \a file for other files.
*/
DocLnk::DocLnk( const QString &file ) :
    AppLnk(file)
{
    init(file);
}

/*!
  Constructs a DocLnk from a valid .desktop \a file or a new .desktop
  \a file for other files. If \a may_be_desktopfile is TRUE, then \a file
  is first attempted to be read as a .desktop file.
*/
DocLnk::DocLnk( const QString &file, bool may_be_desktopfile ) :
    AppLnk(may_be_desktopfile ? file : QString::null)
{
    init(file);
}

void DocLnk::init(const QString &file)
{
    if ( isValid() ) {
#ifndef FORCED_DIR_STRUCTURE_WAY
	if ( mType.isNull() )
	    // try to infer it
#endif
	{
	    int s0 = file.findRev('/');
	    if ( s0 > 0 ) {
		int s1 = file.findRev('/',s0-1);
		if ( s1 > 0 ) {
		    int s2 = file.findRev('/',s1-1);
		    if ( s2 > 0 ) {
			mType = file.mid(s2+1,s0-s2-1);
		    }
		}
	    }
	}
    } else if ( QFile::exists(file) ) {
	QString n = file;
	n.replace(QRegExp("\\..*"),"");
	n.replace(QRegExp(".*/"),"");
	setName( n );
	setFile( file );
    }
    MimeType mt(mType);
    if( mt.application() )
	mExec = mt.application()->exec();
}

/*!
  Constructs an invalid DocLnk.
*/
DocLnk::DocLnk()
{
}

/*!
  Destroys the DocLnk. As with AppLnk objects, a run-time error
  occurs if the DocLnk is a member of a DocLnkSet (or AppLnkSet).
*/
DocLnk::~DocLnk()
{
}

/*!
  \reimp
*/
QString DocLnk::exec() const
{
    MimeType mt(type());
    const AppLnk* app = mt.application();
    if ( app )
	return app->exec();
    else
	return QString::null;
}

/*!
  \reimp
*/
void DocLnk::invoke(const QStringList& args) const
{
    MimeType mt(type());
    const AppLnk* app = mt.application();
    if ( app ) {
	QStringList a = args;
	if ( QFile::exists( linkFile() ) )
	    a.append(linkFile());
	else
	    a.append(file());
	app->execute(a);
    }
}

