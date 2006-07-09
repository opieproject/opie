/**********************************************************************
** Copyright (C) 2000-2006 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2 of the License, or (at your
** option) any later version.
**
** A copy of the GNU GPL license version 2 is included in this package as
** LICENSE.GPL.
**
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** In addition, as a special exception Trolltech gives permission to link
** the code of this program with Qtopia applications copyrighted, developed
** and distributed by Trolltech under the terms of the Qtopia Personal Use
** License Agreement. You must comply with the GNU General Public License
** in all respects for all of the code used other than the applications
** licensed under the Qtopia Personal Use License Agreement. If you modify
** this file, you may extend this exception to your version of the file,
** but you are not obligated to do so. If you do not wish to do so, delete
** this exception statement from your version.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "locationcombo.h"

#include <qpe/ir.h>
#include <qpe/applnk.h>
#include <qpe/global.h>
#include <qpe/categorywidget.h>
#include <qpe/categoryselect.h>
#ifdef QWS
#include <qpe/qcopenvelope_qws.h>
#endif
#include <qpe/filemanager.h>
#include <qpe/config.h>
#include <qpe/storage.h>
#include <qpe/global.h>
#include <qtopia/qpemessagebox.h>

#include <qlineedit.h>
#include <qtoolbutton.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qfile.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qsize.h>
#include <qcombobox.h>
#include <qregexp.h>

#include <qradiobutton.h>
#include <qlayout.h>

#include <stdlib.h>

/*!
  \class LocationCombo locationcombo.h
  \brief The LocationCombo class displays a list of available storage
         locations.

  First availability: Qtopia 1.6

  \ingroup qtopiaemb
  \sa DocPropertiesDialog
 */


class LocationComboPrivate
{
public:
    LocationComboPrivate() : homeLocation(-1), fileSize(0), listEmpty(TRUE) {}
    QString originalPath;
    int homeLocation;
    int fileSize;
    bool listEmpty;
};

/*!
  Constructs a LocationCombo with parent \a parent and name \a name.
 */
LocationCombo::LocationCombo( QWidget *parent, const char *name )
    : QComboBox( FALSE, parent, name )
{
    storage = new StorageInfo;
    d = new LocationComboPrivate;
    setLocation( 0 );
    connect( this, SIGNAL(activated(int)), this, SIGNAL(newPath()) );
    connect( storage, SIGNAL(disksChanged()), this, SLOT(updatePaths()) );
}

/*!
  Constructs a LocationCombo with parent \a parent and name \a name.
  \a lnk is pointer to an existing AppLnk.
 */
LocationCombo::LocationCombo( const AppLnk * lnk, QWidget *parent, const char *name )
    : QComboBox( FALSE, parent, name )
{
    storage = new StorageInfo;
    d = new LocationComboPrivate;
    setLocation(lnk);
    connect( this, SIGNAL(activated(int)), this, SIGNAL(newPath()) );
    connect( storage, SIGNAL(disksChanged()), this, SLOT(updatePaths()) );
}

/*!
  Destroys the widget.
 */
LocationCombo::~LocationCombo()
{
    delete storage;
    delete d;
}

/*!
  Sets the display of the LocationCombo to the location associated with the
  AppLnk \a lnk.
 */
void LocationCombo::setLocation( const AppLnk * lnk )
{
    // NB: setLocation(const QString) assumes only lnk->file() is used.

    if ( lnk ) {
	QFileInfo fi( lnk->file() );
	d->fileSize = fi.size();
	const FileSystem *fs =  storage->fileSystemOf( lnk->file() );
	d->originalPath = fs ? fs->path() : QString::null;
    } else {
	d->fileSize = 0;
	d->originalPath = QString::null;
    }

    setupCombo();

    int currentLocation = -1;
    if ( lnk ) {
	int n = locations.count();
	for ( int i = 0; i < n; i++ ) {
	    if ( lnk->file().contains( locations[i] ) )
		currentLocation = i;
	}
    }
    if ( currentLocation == -1 )
	currentLocation = 0; //default to the first one

    setCurrentItem( currentLocation );
}

/*!
  \internal
*/
void LocationCombo::setupCombo()
{
    clear();
    locations.clear();

    const QList<FileSystem> &fs = storage->fileSystems();
    QListIterator<FileSystem> it ( fs );
    QString s;
    QString homeDir = QDir::homeDirPath();
    QString homeFs;
    QString homeFsPath;
    int index = 0;
    for ( ; it.current(); ++it ) {
	// we add 10k to the file size so we are sure we can also save the desktop file
	if ( !d->fileSize || (*it)->path() == d->originalPath ||
		(ulong)(*it)->availBlocks() * (ulong)(*it)->blockSize()
		> (ulong)d->fileSize + 10000 )
	{
	    if ( (*it)->isRemovable() ) {
		insertItem( (*it)->name(), index );
		locations.append( (*it)->path() );
		index++;
	    } else if ( homeDir.contains( (*it)->path() ) &&
			(*it)->path().length() > homeFsPath.length() ) {
		homeFs = (*it)->name();
		homeFsPath = (*it)->path();
	    }
	}
    }

    // $HOME is *somewhere*, but not shown in Storage::fileSystems(),
    // eg. because it's mounted in some unexpected way.
    if ( homeFsPath.isEmpty() ) {
	homeFs = StorageInfo::tr("Internal Storage");
	homeFsPath = homeDir;
    }

    if ( !homeFsPath.isEmpty() ) {
	d->homeLocation = 0;
	insertItem( homeFs, d->homeLocation );
	locations.prepend( homeDir );
    } else {
	d->homeLocation = -1;
    }

    d->listEmpty = locations.count() == 0;
    if ( d->listEmpty ) {
	insertItem( tr("No FileSystems Available!"), 0 );
	locations.append( "" );
    }
}

/*!
  \internal
 */
void LocationCombo::updatePaths()
{
    QString oldPath = locations[currentItem()];

    setupCombo();

    int currentLocation = 0;
    int n = locations.count();
    for ( int i = 0; i < n; i++ ) {
	if ( oldPath == locations[i] ) {
	    currentLocation = i;
	}
    }
    setCurrentItem( currentLocation );
    if ( locations[currentItem()] != oldPath )
	emit newPath();
}

/*!
  Returns TRUE to indicate that the user has changed the location displayed
  by the LocationCombo.  Most useful when the LocationCombo is part of a
  dialog; when the dialog is accept()ed, LocationCombo::isChanged() can be
  examined to check for a change of location.
 */
bool LocationCombo::isChanged() const
{
    if ( const FileSystem *fs = storage->fileSystemOf(locations[currentItem()]) )
	return fs->path() != d->originalPath;

    return TRUE;
}

/*!
  Returns the default (home) location for the file associated with this
  LocationCombo.
 */
QString LocationCombo::installationPath() const
{
    return currentItem() == d->homeLocation ?
			  QString("/") : locations[ currentItem() ]+"/";
}

/*!
  Returns the document path associated with this LocationCombo.  This
  will be "\<path\>/Documents".
 */
QString LocationCombo::documentPath() const
{
    return locations[ currentItem() ]+"/Documents/";
}

/*!
  Returns a pointer to FileSystem object, associated with the current
  selection of the LocationCombo.
 */
const FileSystem *LocationCombo::fileSystem() const
{
    if ( d->listEmpty )
	return 0;
    return storage->fileSystemOf( locations[ currentItem() ] );
}

/*!
  \fn LocationCombo::newPath()
  Emitted when the LocationCombo changes to a new location.
 */


#if 0
void LocationCombo::apply()
{
}
#endif

