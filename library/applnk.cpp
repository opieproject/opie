/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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

#define QTOPIA_INTERNAL_MIMEEXT
#define QTOPIA_INTERNAL_PRELOADACCESS
#define QTOPIA_INTERNAL_APPLNKASSIGN

#include "applnk.h"

#include <qpe/qpeapplication.h>
#include <qpe/categories.h>
#include <qpe/categoryselect.h>
#ifdef QWS
#include <qpe/qcopenvelope_qws.h>
#endif
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

static bool prepareDirectories(const QString& lf)
{
    if ( !QFile::exists(lf) ) {
	// May need to create directories
	QFileInfo fi(lf);
	if ( system(("mkdir -p "+fi.dirPath(TRUE))) )
	    return FALSE;
    }
    return TRUE;
}

class AppLnkPrivate
{
public:
    /* the size of the Pixmap */
    enum Size {Normal = 0, Big };
    AppLnkPrivate() {
        /* we want one normal and one big item */

        QPixmap pix;
        mPixmaps.insert(0, pix );
        mPixmaps.insert(1,  pix);
    }

    QStringList mCatList; // always correct
    QArray<int> mCat; // cached value; correct if not empty
    QMap<int,  QPixmap> mPixmaps;

    void updateCatListFromArray()
    {
	Categories cat( 0 );
	cat.load( categoryFileName() );
	mCatList = cat.labels("Document View",mCat);
    }

    void setCatArrayDirty()
    {
	mCat.resize(0);
    }

    void ensureCatArray()
    {
	if ( mCat.count() > 0 || mCatList.count()==0 )
	    return;

	Categories cat( 0 );
	cat.load( categoryFileName() );
	mCat.resize( mCatList.count() );
	int i;
	QStringList::ConstIterator it;
	for ( i = 0, it = mCatList.begin(); it != mCatList.end();
	      ++it, i++ ) {

	    bool number;
	    int id = (*it).toInt( &number );
	    if ( !number ) {
		id = cat.id( "Document View", *it );
		if ( id == 0 )
		    id = cat.addCategory( "Document View", *it );
	    }
	    mCat[i] = id;
	}
    }
};

/*!
  \class AppLnk applnk.h
  \brief The AppLnk class represents an application available on the system.

    Every Qtopia application \e app has a corresponding \e app.desktop
    file. When one of these files is read its data is stored as an
    AppLnk object.

    The AppLnk class introduces some Qtopia-specific concepts, and
    provides a variety of functions, as described in the following
    sections.
    \tableofcontents

    \target Types
    \section1 Types

    Every AppLnk object has a \e type. For applications, games and
    settings the type is \c Application; for documents the
    type is the document's MIME type.

    \target files-and-links
    \section1 Files and Links

    When you create an AppLnk (or more likely, a \link doclnk.html
    DocLnk\endlink), you don't deal directly with filenames in the
    filesystem. Instead you do this:
    \code
    DocLnk d;
    d.setType("text/plain");
    d.setName("My Nicely Named Document / Whatever"); // Yes, "/" is legal.
    \endcode
    At this point, the file() and linkFile() are unknown. Normally
    this is uninteresting, and the names become automatically known,
    and more importantly, becomes reserved, when you ask what they are:

    \code
    QString fn = d.file();
    \endcode
    This invents a filename, and creates the file on disk (an empty
    reservation file) to prevent the name being used by another
    application.

    In some circumstances, you don't want to create the file if it
    doesn't already exist (e.g. in the Document tab, some of the \link
    doclnk.html DocLnk\endlink objects represented by icons are
    DocLnk's created just for that view - they don't have
    corresponding \c .desktop files. To avoid littering empty
    reservation files around, we check in a few places to see whether
    the file really needs to exist).

    \section1 Functionality

    AppLnk objects are created by calling the constructor with the
    name of a \e .desktop file. The object can be checked for validity
    using isValid().

    The following functions are used to set or retrieve information
    about the application:
    \table
    \header \i Get Function \i Set Function \i Short Description
    \row \i \l name()	    \i \l setName()    \i application's name
    \row \i \l pixmap()     \i \e none	    \i application's icon
    \row \i \l bigPixmap()  \i \e none	    \i application's large icon
    \row \i \e none	    \i setIcon()    \i sets the icon's filename
    \row \i \l type() 	    \i \l setType()    \i see \link #Types Types\endlink above
    \row \i \l rotation()   \i \e none	    \i 0, 90, 180 or 270 degrees
    \row \i \l comment()    \i \l setComment() \i text for the Details dialog
    \row \i \l exec()	    \i \l setExec()    \i executable's filename
    \row \i \l file()	    \i \e none	    \i document's filename
    \row \i \l linkFile()   \i \l setLinkFile()	\i \e .desktop filename
    \row \i \l mimeTypes()  \i \e none	    \i the mime types the application can view or edit
    \row \i \l categories() \i \l setCategories()	\i \e{see the function descriptions}
    \row \i \l fileKnown()  \i \e none	    \i see \link
#files-and-links Files and Links\endlink above
    \row \i \l linkFileKnown() \i \e none	    \i see \link
#files-and-links Files and Links\endlink above
    \row \i \l property()   \i \l setProperty()	\i any AppLnk property
    can be retrieved or set (if writeable) using these
    \endtable

    To save an AppLnk to disk use writeLink(). To execute the
    application that the AppLnk object refers to, use execute().

    AppLnk's can be deleted from disk using removeLinkFile(). To
    remove both the link and the application's executable use
    removeFiles().

    Icon sizes can be globally changed (but only for AppLnk objects
    created after the calls) with setSmallIconSize() and
    setBigIconSize().

    \ingroup qtopiaemb
*/

/*!
  Sets the size used for small icons to \a small pixels.
  Only affects AppLnk objects created after the call.

  \sa smallIconSize() setIcon()
*/
void AppLnk::setSmallIconSize(int small)
{
    smallSize = small;
}

/*!
  Returns the size used for small icons.

  \sa setSmallIconSize() setIcon()
*/
int AppLnk::smallIconSize()
{
    return smallSize;
}


/*!
  Sets the size used for large icons to \a big pixels.
  Only affects AppLnk objects created after the call.

  \sa bigIconSize() setIcon()
*/
void AppLnk::setBigIconSize(int big)
{
    bigSize = big;
}

/*!
  Returns the size used for large icons.

  \sa setBigIconSize() setIcon()
*/
int AppLnk::bigIconSize()
{
    return bigSize;
}


/*!
  \fn QString AppLnk::name() const

  Returns the Name property. This is the user-visible name for the
  document or application, not the filename.

    See \link #files-and-links Files and Links\endlink.

  \sa setName()
*/
/*!
    \fn QString AppLnk::exec() const

  Returns the Exec property. This is the name of the executable
  program associated with the AppLnk.

  \sa setExec()
*/
/*!
    \fn QString AppLnk::rotation() const

  Returns the Rotation property. The value is 0, 90, 180 or 270
  degrees.
*/
/*!
    \fn QString AppLnk::comment() const

  Returns the Comment property.

  \sa setComment()
*/
/*!
    \fn QStringList AppLnk::mimeTypes() const

  Returns the MimeTypes property. This is the list of MIME types
  that the application can view or edit.
*/
/*!
    \fn const QArray<int>& AppLnk::categories() const

  Returns the Categories property.

  See the CategoryWidget for more details.

  \sa setCategories()
*/

const QArray<int>& AppLnk::categories() const
{
    d->ensureCatArray();
    return d->mCat;
}

/*!
    \fn int AppLnk::id() const

  Returns the id of the AppLnk. If the AppLnk is not in an AppLnkSet,
  this value is 0, otherwise it is a value that is unique for the
  duration of the current process.

  \sa AppLnkSet::find()
*/

/*!
  \fn bool AppLnk::isValid() const

  Returns TRUE if this AppLnk is valid; otherwise returns FALSE.
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
  Loads \a file (e.g. \e app.desktop) as an AppLnk.

  \sa writeLink()
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
	    // MIME types are case-insensitive.
	    mMimeTypes = config.readListEntry( "MimeType", ';' );
	    for (QStringList::Iterator it=mMimeTypes.begin(); it!=mMimeTypes.end(); ++it)
		*it = (*it).lower();
	    mMimeTypeIcons = config.readListEntry( "MimeTypeIcons", ';' );
	    mLinkFile = file;
	    mFile = config.readEntry("File", QString::null);
	    if ( mFile[0] != '/' ) {
		int slash = file.findRev('/');
		if ( slash >= 0 ) {
		    mFile = file.left(slash) + '/' + mFile;
		}
	    }
	    d->mCatList = config.readListEntry("Categories", ';');
	    if ( d->mCatList[0].toInt() < -1 ) {
		// numeric cats in file! convert to text
		Categories cat( 0 );
		cat.load( categoryFileName() );
		d->mCat.resize( d->mCatList.count() );
		int i;
		QStringList::ConstIterator it;
		for ( i = 0, it = d->mCatList.begin(); it != d->mCatList.end();
		      ++it, i++ ) {
		    bool number;
		    int id = (*it).toInt( &number );
		    if ( !number ) {
			// convert from text
			id = cat.id( "Document View", *it );
			if ( id == 0 )
			    id = cat.addCategory( "Document View", *it );
		    }
		    d->mCat[i] = id;
		}
		d->updateCatListFromArray();
	    }
	}
    }
    mId = 0;
}

AppLnk& AppLnk::operator=(const AppLnk &copy)
{
    if ( mId )
	qWarning("Deleting AppLnk that is in an AppLnkSet");
    if ( d )
	delete d;


    mName = copy.mName;

    /* remove for Qtopia 3.0 -zecke */
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
    d->mCatList = copy.d->mCatList;
    d->mPixmaps = copy.d->mPixmaps;

    return *this;
}
/*!
  protected internally to share code
  should I document that at all?
  I don't know the TT style for that
*/
const QPixmap& AppLnk::pixmap( int pos, int size ) const {
    if ( d->mPixmaps[pos].isNull() ) {
	AppLnk* that = (AppLnk*)this;
	if ( mIconFile.isEmpty() ) {
	    MimeType mt(type());
	    that->d->mPixmaps[pos] = mt.pixmap();
	    if ( that->d->mPixmaps[pos].isNull() )
		that->d->mPixmaps[pos].convertFromImage(
		    Resource::loadImage("UnknownDocument")
			.smoothScale( size, size ) );
	    return that->d->mPixmaps[pos];
	}
	QImage unscaledIcon = Resource::loadImage( that->mIconFile );
	if ( unscaledIcon.isNull() ) {
	    qDebug( "Cannot find icon: %s", that->mIconFile.latin1() );
	    that->d->mPixmaps[pos].convertFromImage(
		    Resource::loadImage("UnknownDocument")
			.smoothScale( size, size ) );
	} else {
	    that->d->mPixmaps[0].convertFromImage( unscaledIcon.smoothScale( smallSize, smallSize ) );
	    that->d->mPixmaps[1].convertFromImage( unscaledIcon.smoothScale( bigSize, bigSize ) );
	}
	return that->d->mPixmaps[pos];
    }
    return d->mPixmaps[pos];
}

/*!
  Returns a small pixmap associated with the application.

  \sa bigPixmap() setIcon()
*/
const QPixmap& AppLnk::pixmap() const
{
    if ( d->mPixmaps[0].isNull() ) {
        return pixmap(AppLnkPrivate::Normal, smallSize );
    }
    return d->mPixmaps[0];
}

/*!
  Returns a large pixmap associated with the application.

  \sa pixmap() setIcon()
*/
const QPixmap& AppLnk::bigPixmap() const
{
    if ( d->mPixmaps[1].isNull() ) {
        return pixmap( AppLnkPrivate::Big, bigSize );
    }
    return mBigPixmap;
}

/*!
  Returns the type of the AppLnk. For applications, games and
  settings the type is \c Application; for documents the type is the
  document's MIME type.
*/
QString AppLnk::type() const
{
    if ( mType.isNull() ) {
	AppLnk* that = (AppLnk*)this;
	QString f = file();
	if ( !f.isNull() ) {
	    MimeType mt(f);
	    that->mType = mt.id();
	    return that->mType;
	}
    }
    return mType;
}

/*!
  Returns the file associated with the AppLnk.

  \sa exec() name()
*/
QString AppLnk::file() const
{
    if ( mFile.isNull() ) {
	AppLnk* that = (AppLnk*)this;
	QString ext = MimeType(mType).extension();
	if ( !ext.isEmpty() )
	    ext = "." + ext;
	if ( !mLinkFile.isEmpty() ) {
	    that->mFile =
		mLinkFile.right(8)==".desktop" // 8 = strlen(".desktop")
		    ? mLinkFile.left(mLinkFile.length()-8) : mLinkFile;
	} else if ( mType.contains('/') ) {
	    that->mFile =
		QString(getenv("HOME"))+"/Documents/"+mType+"/"+safeFileName(that->mName);
            /*
             * A file with the same name or a .desktop file already exists
             */
	    if ( QFile::exists(that->mFile+ext) || QFile::exists(that->mFile+".desktop") ) {
		int n=1;
		QString nn;
		while (QFile::exists((nn=(that->mFile+"_"+QString::number(n)))+ext)
			|| QFile::exists(nn+".desktop"))
		    n++;
		that->mFile = nn;
	    }
	    that->mLinkFile = that->mFile+".desktop";
	    that->mFile += ext;
	}
	prepareDirectories(that->mFile);
	if ( !that->mFile.isEmpty() ) {
	    QFile f(that->mFile);
	    if ( !f.open(IO_WriteOnly) )
		that->mFile = QString::null;
	    return that->mFile;
	}
    }
    return mFile;
}

/*!
  Returns the desktop file corresponding to this AppLnk.

  \sa file() exec() name()
*/
QString AppLnk::linkFile() const
{
    if ( mLinkFile.isNull() ) {
	AppLnk* that = (AppLnk*)this;
	if ( type().contains('/') ) {
	    StorageInfo storage;
	    const FileSystem *fs = storage.fileSystemOf( that->mFile );
            /* tmpfs + and ramfs are available too but not removable
             * either we fix storage or add this
             */
	    if ( fs && ( fs->isRemovable() || fs->disk() == "/dev/mtdblock6" || fs->disk() == "tmpfs") ) {
		that->mLinkFile = fs->path();
	    } else
		that->mLinkFile = getenv( "HOME" );
	    that->mLinkFile += "/Documents/"+type()+"/"+safeFileName(that->mName);

            /* the desktop file exists make sure we don't point to the same file */
	    if ( QFile::exists(that->mLinkFile+".desktop") ) {
                AppLnk lnk( that->mLinkFile + ".desktop" );

                /* the linked is different */
                if(that->file() != lnk.file() ) {
                    int n = 1;
                    QString nn;
                    while (QFile::exists((nn=that->mLinkFile+"_"+QString::number(n))+".desktop")) {
                        n++;
                        /* just to be sure */
                        AppLnk lnk(nn );
                        if (lnk.file() == that->file() )
                            break;
                    }
                    that->mLinkFile = nn;
                }
	    }
	    that->mLinkFile += ".desktop";
	    storeLink();
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
    d->mCatList = copy.d->mCatList;
    d->mPixmaps = copy.d->mPixmaps;
}

/*!
  Destroys the AppLnk. Note that if the AppLnk is currently a member
  of an AppLnkSet, this will produce a run-time warning.

  \sa AppLnkSet::add() AppLnkSet::remove()
*/
AppLnk::~AppLnk()
{
    if ( mId )
	qWarning("Deleting AppLnk that is in an AppLnkSet");
    if ( d )
	delete d;
}

/*!
  \overload
  Executes the application associated with this AppLnk.

  \sa exec()
*/
void AppLnk::execute() const
{
    execute(QStringList());
}

/*!
  Executes the application associated with this AppLnk, with
  \a args as arguments.

  \sa exec()
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
  this function, so you should not call it directly.

  \sa execute()
*/
void AppLnk::invoke(const QStringList& args) const
{
    Global::execute( exec(), args[0] );
}

/*!
  Sets the Exec property to \a exec.

  \sa exec() name()
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

  \sa file() name()
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

  This text is displayed in the 'Details Dialog', for example if the
  user uses the 'press-and-hold' gesture.

  \sa comment()
*/
void AppLnk::setComment( const QString& comment )
{
    mComment = comment;
}

/*!
  Sets the Type property to \a type.

  For applications, games and settings the type should be \c
  Application; for documents the type should be the document's MIME
  type.

  \sa type()
*/
void AppLnk::setType( const QString& type )
{
    mType = type;
}

/*!
  \fn QString AppLnk::icon() const

  Returns the Icon property.

  \sa setIcon()
*/

/*!
  Sets the Icon property to \a iconname. This is the filename from
  which the pixmap() and bigPixmap() are obtained.

  \sa icon() setSmallIconSize() setBigIconSize()
*/
void AppLnk::setIcon( const QString& iconname )
{
    mIconFile = iconname;
    QImage unscaledIcon = Resource::loadImage( mIconFile );
    d->mPixmaps[0].convertFromImage( unscaledIcon.smoothScale( smallSize, smallSize ) );
    d->mPixmaps[1].convertFromImage( unscaledIcon.smoothScale( bigSize, bigSize ) );
}

/*!
  Sets the Categories property to \a c.

  See the CategoryWidget for more details.

  \sa categories()
*/
void AppLnk::setCategories( const QArray<int>& c )
{
    d->mCat = c;
    d->updateCatListFromArray();
}

/*!
  \fn QStringList AppLnk::mimeTypeIcons() const

  Returns the MimeTypeIcons property of the AppLnk.
*/

/*!
  Attempts to ensure that the link file for this AppLnk exists,
  including creating any required directories. Returns TRUE if
  successful; otherwise returns FALSE.

  You should not need to use this function.
*/
bool AppLnk::ensureLinkExists() const
{
    QString lf = linkFile();
    return prepareDirectories(lf);
}

/*!
  Commits the AppLnk to disk. Returns TRUE if the operation succeeded;
  otherwise returns FALSE.

  In addition, the "linkChanged(QString)" message is sent to the
  "QPE/System" \link qcop.html QCop\endlink channel.
*/
bool AppLnk::writeLink() const
{
    // Only re-writes settable parts
    QString lf = linkFile();
    if ( !ensureLinkExists() )
	return FALSE;
    storeLink();
    return TRUE;
}

/*!
  \internal
*/
void AppLnk::storeLink() const
{
    Config config( mLinkFile, Config::File );
    config.setGroup("Desktop Entry");
    config.writeEntry("Name",mName);
    if ( !mIconFile.isNull() ) config.writeEntry("Icon",mIconFile);
    config.writeEntry("Type",type());
    if ( !mComment.isNull() ) config.writeEntry("Comment",mComment);
    QString f = file();
    int i = 0;
    while ( i < (int)f.length() && i < (int)mLinkFile.length() && f[i] == mLinkFile[i] )
	i++;
    while ( i && f[i] != '/' )
	i--;
    // simple case where in the same directory
    if ( mLinkFile.find( '/', i + 1 ) < 0 )
	f = f.mid(i+1);
    // ### could do relative ie ../../otherDocs/file.doc
    config.writeEntry("File",f);
    config.writeEntry( "Categories", d->mCatList, ';' );

#ifndef QT_NO_COP
    QCopEnvelope e("QPE/System", "linkChanged(QString)");
    e << mLinkFile;
#endif
}

/*!
  Sets the property named \a key to \a value.

  \sa property()
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

  \sa setProperty()
*/
QString AppLnk::property(const QString& key) const
{
    QString lf = linkFile();
    if ( !QFile::exists(lf) )
	return QString::null;
    Config cfg(lf, Config::File);
    return cfg.readEntry(key);
}

bool AppLnk::isPreloaded() const {
  // Preload information is stored in the Launcher config in v1.5.
  Config cfg("Launcher");
  cfg.setGroup("Preload");
  QStringList apps = cfg.readListEntry("Apps",',');
  if (apps.contains(exec()))
    return true;
  return false;
}

void AppLnk::setPreloaded(bool yesNo) {
  // Preload information is stored in the Launcher config in v1.5.
  Config cfg("Launcher");
  cfg.setGroup("Preload");
  QStringList apps = cfg.readListEntry("Apps", ',');
  if (apps.contains(exec()) && !yesNo)
    apps.remove(exec());
  else if (yesNo && !apps.contains(exec()))
    apps.append(exec());
  cfg.writeEntry("Apps", apps, ',');
}


/*!
  Deletes both the linkFile() and the file() associated with this AppLnk.

  \sa removeLinkFile()
*/
void AppLnk::removeFiles()
{
    bool valid = isValid();
    if ( !valid || !linkFileKnown() || QFile::remove(linkFile()) ) {
	if ( QFile::remove(file()) ) {
#ifndef QT_NO_COP
	    QCopEnvelope e("QPE/System", "linkChanged(QString)");
	    if ( linkFileKnown() )
		e << linkFile();
	    else
		e << file();
#endif
	} else if ( valid ) {
	    // restore link
	    writeLink();
	}
    }
}

/*!
  Deletes the linkFile(), leaving any file() untouched.

    \sa removeFiles()
*/
void AppLnk::removeLinkFile()
{
    if ( isValid() && linkFileKnown() && QFile::remove(linkFile()) ) {
#ifndef QT_NO_COP
	QCopEnvelope e("QPE/System", "linkChanged(QString)");
	e << linkFile();
#endif
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

  Returns the list of \link applnk.html#Types types\endlink in the set.

    For applications, games and settings the type is \c Application;
    for documents the type is the document's MIME type.

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
  all the files in the given \a directory (and any subdirectories
  recursively).

  \omit
  The directories may contain ".directory" files which override
  any AppLnk::type() values for AppLnk objects found in the directory.
  This allows simple localization of application types.
  \endomit
*/
AppLnkSet::AppLnkSet( const QString &directory ) :
    d(new AppLnkSetPrivate)
{
    QDir dir( directory );
    mFile = directory;
    findChildren(directory,QString::null,QString::null);
}

/*!
  Detaches all AppLnk objects from the set. The set become empty and
  the caller becomes responsible for deleting the AppLnk objects.
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
  Destroys the set, deleting all the AppLnk objects it contains.

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
  Adds AppLnk \a f to the set. The set takes responsibility for
  deleting \a f.

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
  Removes AppLnk \a f to the set. The caller becomes responsible for
  deleting \a f. Returns TRUE if \a f was in the set; otherwise
  returns FALSE.

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

    For applications, games and settings the type is \c Application;
    for documents the type is the document's MIME type.
*/
QString AppLnkSet::typeName( const QString& t ) const
{
    QString *st = d->typName.find(t);
    return st ? *st : QString::null;
}

/*!
  Returns the small pixmap associated with type \a t.

    For applications, games and settings the type is \c Application;
    for documents the type is the document's MIME type.
*/
QPixmap AppLnkSet::typePixmap( const QString& t ) const
{
    QPixmap *pm = d->typPix.find(t);
    return pm ? *pm : QPixmap();
}

/*!
  Returns the large pixmap associated with type \a t.

    For applications, games and settings the type is \c Application;
    for documents the type is the document's MIME type.
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
  Constructs a DocLnkSet that contains DocLnk objects representing all
  the files in the \a directory (and any subdirectories, recursively).

  If \a mimefilter is not null,
  only documents with a MIME type matching \a mimefilter are selected.
  The value may contain multiple wild-card patterns separated by ";",
  such as \c{*o/mpeg;audio/x-wav}.

  See also \link applnk.html#files-and-links Files and Links\endlink.

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

    /* Opie got a different approach
     * I guess it's geek vs. consumer
     * in this case to be discussed
     */
    if ( dir.exists( ".Qtopia-ignore" ) )
	return;

    const QFileInfoList *list = dir.entryInfoList();
    if ( list ) {
	QFileInfo* fi;
	for ( QFileInfoListIterator it(*list); (fi=*it); ++it ) {
	    QString bn = fi->fileName();
	    if ( bn[0] != '.' ) {
		if ( fi->isDir()  ) {
		    if ( bn != "CVS" && bn != "Qtopia" && bn != "QtPalmtop" )
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
  \a file for other files. If \a may_be_desktopfile is TRUE, then an
  attempt is made to read \a file as a .desktop file; if that fails it
  is read as a normal file.
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
	n.replace(QRegExp(".*/"),"");
	n.replace(QRegExp("\\..*"),"");
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
  Destroys the DocLnk. Just like AppLnk objects, a run-time error
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
	if ( linkFileKnown() && QFile::exists( linkFile() ) )
	    a.append(linkFile());
	else
	    a.append(file());
	app->execute(a);
    }
}


