/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included
** in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
** THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A
** PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#include "categories.h"
#include <qfile.h>
#include <qcstring.h>
#include <qtextstream.h>
#include "stringutil.h"

using namespace Qtopia;

/***********************************************************
 *
 * CategoryGroup
 *
 **********************************************************/

#ifdef PALMTOPCENTER
UidGen CategoryGroup::sUidGen( UidGen::PalmtopCenter );
#else
UidGen CategoryGroup::sUidGen( UidGen::Qtopia );
#endif

int CategoryGroup::add( const QString &label )
{
    if ( label == QObject::tr("All") || label == QObject::tr("Unfiled") )
	return 0;

    QMap<QString,int>::Iterator findIt = mLabelIdMap.find( label );
    if ( findIt != mLabelIdMap.end() )
	return 0;
    int newUid = uidGen().generate();
    insert( newUid, label );
    return newUid;
}

void CategoryGroup::insert( int uid, const QString &label )
{
    uidGen().store( uid );
    mIdLabelMap[uid] = label;
    mLabelIdMap[label] = uid;
}

bool CategoryGroup::add( int uid, const QString &label )
{
    if ( label == QObject::tr("All") || label == QObject::tr("Unfiled") )
	return FALSE;

    QMap<QString,int>::ConstIterator labelIt = mLabelIdMap.find( label );
    if ( labelIt != mLabelIdMap.end() )
	return FALSE;
    QMap<int,QString>::ConstIterator idIt = mIdLabelMap.find( uid );
    if ( idIt != mIdLabelMap.end() )
	return FALSE;
    insert( uid, label );
    return TRUE;
}

bool CategoryGroup::remove( const QString &label )
{
    QMap<QString,int>::Iterator findIt = mLabelIdMap.find( label );
    if ( findIt == mLabelIdMap.end() )
	return FALSE;

    mIdLabelMap.remove( *findIt );
    mLabelIdMap.remove( findIt );

    return TRUE;
}

bool CategoryGroup::remove( int uid )
{
    QMap<int,QString>::Iterator idIt = mIdLabelMap.find( uid );
    if ( idIt == mIdLabelMap.end() )
	return FALSE;

    mLabelIdMap.remove( *idIt );
    mIdLabelMap.remove( idIt );

    return TRUE;
}

bool CategoryGroup::rename( int uid, const QString &newLabel )
{
    if ( newLabel == QObject::tr("All") || newLabel == QObject::tr("Unfiled") )
	return FALSE;

    QMap<int, QString>::Iterator idIt = mIdLabelMap.find( uid );
    if ( idIt == mIdLabelMap.end() )
	return FALSE;

    mLabelIdMap.remove( *idIt );
    mLabelIdMap[newLabel] = uid;
    *idIt = newLabel;

    return TRUE;
}

bool CategoryGroup::rename( const QString &oldLabel, const QString &newLabel )
{
    return rename( id(oldLabel), newLabel );
}

bool CategoryGroup::contains(int uid) const
{
    return ( mIdLabelMap.find( uid ) != mIdLabelMap.end() );
}

bool CategoryGroup::contains(const QString &label) const
{
    return ( mLabelIdMap.find( label ) != mLabelIdMap.end() );
}

/** Returns label associated with the uid or QString::null if
 *   not found
 */
const QString &CategoryGroup::label(int uid) const
{
    QMap<int,QString>::ConstIterator idIt = mIdLabelMap.find( uid );
    if ( idIt == mIdLabelMap.end() )
	return QString::null;
    return *idIt;
}

/** Returns the uid associated with label or 0 if not found */
int CategoryGroup::id(const QString &label) const
{
    QMap<QString,int>::ConstIterator labelIt = mLabelIdMap.find( label );
    if ( labelIt == mLabelIdMap.end() )
	return 0;
    return *labelIt;
}

QStringList CategoryGroup::labels() const
{
    QStringList labels;
    for ( QMap<int, QString>::ConstIterator it = mIdLabelMap.begin();
	  it != mIdLabelMap.end(); ++it )
	labels += *it;
    // ### I don't think this is the place for this...
//    labels.sort();
    return labels;
}

QStringList CategoryGroup::labels(const QArray<int> &catids ) const
{
    QStringList labels;
    if ( catids.count() == 0 )
	return labels;
    for ( QMap<int, QString>::ConstIterator it = mIdLabelMap.begin();
	  it != mIdLabelMap.end(); ++it )
	if ( catids.find( it.key() ) != -1 )
	    labels += *it;
    return labels;
}

/***********************************************************
 *
 * Categories
 *
 **********************************************************/

/** Add the category name as long as it doesn't already exist locally
 *  or globally.  Return TRUE if added, FALSE if conflicts.
 */
int Categories::addCategory( const QString &appname,
			     const QString &catname,
			     int uid )
{
    if ( mGlobalCats.contains(catname) )
	return 0;

    QMap< QString, CategoryGroup >::Iterator
	appIt = mAppCats.find( appname );

    if ( appIt == mAppCats.end() ) {
	CategoryGroup newgroup;
	newgroup.add( uid, catname );
	mAppCats.insert( appname, newgroup );
	emit categoryAdded( *this, appname, uid );
	return uid;
    }

    CategoryGroup &cats = *appIt;
    cats.add( uid, catname );
    emit categoryAdded( *this, appname, uid );
    return uid;
}

int Categories::addCategory( const QString &appname,
			     const QString &catname )
{
     if ( mGlobalCats.contains(catname) )
	return 0;

    QMap< QString, CategoryGroup >::Iterator
	appIt = mAppCats.find( appname );

    if ( appIt == mAppCats.end() ) {
	CategoryGroup newgroup;
	int uid = newgroup.add( catname );
	mAppCats.insert( appname, newgroup );
	emit categoryAdded( *this, appname, uid );
	return uid;
    }

    CategoryGroup &cats = *appIt;
    int uid = cats.add( catname );
    if ( !uid )
	return 0;
    emit categoryAdded( *this, appname, uid );
    return uid;
}

int Categories::addGlobalCategory( const QString &catname, int uid )
{
    mGlobalCats.add( uid, catname );
    emit categoryAdded( *this, QString::null, uid );
    return uid;
}

int Categories::addGlobalCategory( const QString &catname )
{
    int uid = mGlobalCats.add( catname );
    if ( !uid )
	return 0;
    emit categoryAdded( *this, QString::null, uid );
    return uid;
}

/** Removes the category from the application; if it is not found
 *  in the application, then it attempts to remove it from
 *  the global list
 */
bool Categories::removeCategory( const QString &appname,
				 const QString &catname,
				 bool checkGlobal )
{
    QMap< QString, CategoryGroup >::Iterator
	appIt = mAppCats.find( appname );
    if ( appIt != mAppCats.end() ) {
	CategoryGroup &cats = *appIt;
	int uid = cats.id( catname );
	if ( cats.remove( uid ) ) {
	    emit categoryRemoved( *this, appname, uid );
	    return TRUE;
	}
    }
    if ( !checkGlobal )
	return FALSE;
    return removeGlobalCategory( catname );
}

bool Categories::removeCategory( const QString &appname, int uid )
{
    QMap< QString, CategoryGroup >::Iterator
	appIt = mAppCats.find( appname );
    if ( appIt != mAppCats.end() ) {
	CategoryGroup &cats = *appIt;
	if ( cats.remove( uid ) ) {
	    emit categoryRemoved( *this, appname, uid );
	    return TRUE;
	}
    }
    return FALSE;
}

bool Categories::removeGlobalCategory( const QString &catname )
{
    int uid = mGlobalCats.id( catname );
    if ( mGlobalCats.remove( uid ) ) {
	emit categoryRemoved( *this, QString::null, uid );
	return TRUE;
    }
    return FALSE;
}


bool Categories::removeGlobalCategory( int uid )
{
     if ( mGlobalCats.remove( uid ) ) {
	emit categoryRemoved( *this, QString::null, uid );
	return TRUE;
    }
    return FALSE;
}

/** Returns the sorted list of all categories that are associated with
 *   the app.  If includeGlobal parameter is TRUE then the returned
 *   categories will include the global category items.
 */
QStringList Categories::labels( const QString &app,
				bool includeGlobal,
				ExtraLabels extra ) const
{
    QMap< QString, CategoryGroup >::ConstIterator
	appIt = mAppCats.find( app );
    QStringList cats;

    if ( appIt != mAppCats.end() )
	cats += (*appIt).labels();
    else qDebug("Categories::labels didn't find app %s", app.latin1() );
    if ( includeGlobal )
	cats += mGlobalCats.labels();

    cats.sort();
    switch ( extra ) {
    case NoExtra: break;
    case AllUnfiled:
	cats.append( tr("All") );
	cats.append( tr("Unfiled") );
	break;
    case AllLabel:
	cats.append( tr("All") );
	break;
    case UnfiledLabel:
	cats.append( tr("Unfiled") );
	break;
    }

    return cats;
}

QString Categories::label( const QString &app, int id ) const
{
    if ( mGlobalCats.contains( id ) )
	return mGlobalCats.label( id );
    QMap< QString, CategoryGroup >::ConstIterator
	appIt = mAppCats.find( app );
    if ( appIt == mAppCats.end() )
	return QString::null;
    return (*appIt).label( id );
}

/** Returns a single string associated with the cat ids for display in
    *   a combobox or any area that requires one string.  If catids are empty
    *   then "Unfiled" will be returned.  If multiple categories are assigned
    *   the first cat id is shown with " (multi)" appended to the string.
    */
QString Categories::displaySingle( const QString &app,
				   const QArray<int> &catids,
				   DisplaySingle display ) const
{
    QStringList strs = labels( app, catids );
    if ( !strs.count() )
	return tr("Unfiled");
    strs.sort();
    QString r;
    if ( strs.count() > 1 ) {
	switch ( display ) {
	case ShowFirst:
	    r = strs.first();
	    break;
	case ShowMulti:
	    r = strs.first() + tr(" (multi.)");
	    break;
	case ShowAll:
	    r = strs.join(" ");
	    break;
	}
    }
    else r = strs.first(); 	
    return r;
}

QArray<int> Categories::ids( const QString &app, const QStringList &labels) const
{
  QArray<int> results;
  QStringList::ConstIterator it;
  int i;
  
  for ( i=0, it=labels.begin(); it!=labels.end(); i++, ++it ) {
    int value = id( app, *it );
    if ( value != 0 ) {
      int tmp = results.size();
      results.resize( tmp + 1 );
      results[ tmp ] = value;
    }
  }
  return results;
}

int Categories::id( const QString &app, const QString &cat ) const
{
    if ( cat == tr("Unfiled") || cat.contains( tr(" (multi.)") ) )
	return 0;
    int uid = mGlobalCats.id( cat );
    if ( uid != 0 )
	return uid;
    return mAppCats[app].id( cat );
}


/** Return TRUE if renaming succeeded; FALSE if app name not found,
 *   or if there was a name conflict
 */
bool Categories::renameCategory( const QString &appname,
				 const QString &oldName,
				 const QString &newName )
{
    QMap< QString, CategoryGroup >::Iterator
	appIt = mAppCats.find( appname );

    if ( appIt != mAppCats.end() ) {
	CategoryGroup &cats = *appIt;
	int id = cats.id( oldName );
	if ( id != 0 && cats.rename( id, newName ) ) {
	    emit categoryRenamed( *this, appname, id );
	    return TRUE;
	}
    }
    return renameGlobalCategory( oldName, newName );
}

bool Categories::renameGlobalCategory( const QString &oldName,
				       const QString &newName )
{
    int uid = mGlobalCats.id( oldName );
    if ( uid != 0 && mGlobalCats.rename( uid, newName ) ) {
	emit categoryRenamed( *this, QString::null, uid );
	return TRUE;
    }
    return FALSE;
}

void Categories::setGlobal( const QString &appname,
			    const QString &catname,
			    bool global )
{
    // if in global and should be in app; then move it
    if ( mGlobalCats.contains( catname ) && !global ) {
	mGlobalCats.remove( catname );
	addCategory( appname, catname );
	return ;
    }

    // if in app and should be in global, then move it
    if ( !global )
	return;
    if ( removeCategory( appname, catname, FALSE ) )
	addGlobalCategory( catname );
}

bool Categories::isGlobal( const QString &catname ) const
{
    return mGlobalCats.contains( catname );
}


/** Returns true if the catname is associated with any application
 */
bool Categories::exists( const QString &catname ) const
{
    if ( isGlobal(catname) )
	return TRUE;

    for ( QMap<QString, CategoryGroup>::ConstIterator appsIt = mAppCats.begin(); appsIt != mAppCats.end(); ++appsIt )
	if ( exists( appsIt.key(), catname ) )
	    return TRUE;

    return FALSE;
}

bool Categories::exists( const QString &appname,
			 const QString &catname) const
{
    QMap< QString, CategoryGroup >::ConstIterator
	appIt = mAppCats.find( appname );

    if ( appIt == mAppCats.end() )
	return FALSE;

    return (*appIt).contains( catname );
}


bool Categories::save( const QString &fname ) const
{
    QString strNewFile = fname + ".new";
    QFile f( strNewFile );
    QString out;
    int total_written;

    if ( !f.open( IO_WriteOnly|IO_Raw ) ) {
	qWarning("Unable to write to %s", fname.latin1());
	return FALSE;
    }

    out = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out += "<!DOCTYPE CategoryList>\n";

    out += "<Categories>\n";

    for ( QMap<int, QString>::ConstIterator git = mGlobalCats.idMap().begin();
	  git != mGlobalCats.idMap().end(); ++git )
	out += "<Category id=\"" + QString::number(git.key()) + "\"" +
	  " name=\"" + escapeString(*git) + "\" />\n";

    for ( QMap<QString, CategoryGroup>::ConstIterator appsIt=mAppCats.begin();
	  appsIt != mAppCats.end(); ++appsIt ) {
	const QString &app = appsIt.key();
	const QMap<int, QString> &appcats = (*appsIt).idMap();
	for ( QMap<int, QString>::ConstIterator appcatit = appcats.begin();
	      appcatit != appcats.end(); ++appcatit )
	    out += "<Category id=\"" + QString::number(appcatit.key()) + "\"" +
	      " app=\"" + escapeString(app) + "\"" +
	      " name=\"" + escapeString(*appcatit) + "\" />\n";
    }
    out += "</Categories>\n";

    QCString cstr = out.utf8();
    total_written = f.writeBlock( cstr.data(), cstr.length() );
    if ( total_written != int(cstr.length()) ) {
	f.close();
 	QFile::remove( strNewFile );
 	return FALSE;
    }
    f.close();

    if ( ::rename( strNewFile.latin1(), fname.latin1() ) < 0 ) {
	qWarning( "problem renaming file %s to %s",
		  strNewFile.latin1(), fname.latin1());
	// remove the tmp file...
	QFile::remove( strNewFile );
    }

    return TRUE;
}

bool Categories::load( const QString &fname )
{
    QFile file( fname );
    if ( !file.open( IO_ReadOnly ) ) {
	qWarning("Unable to open %s", fname.latin1());

	addGlobalCategory(tr("Business"));
	addGlobalCategory(tr("Personal"));
	save(fname);

	return FALSE;
    }

    clear();
    QByteArray ba = file.readAll();
    QString data = QString::fromUtf8( ba.data(), ba.size() );
    QChar *uc = (QChar *)data.unicode();
    int len = data.length();

    //     QTime t;
    //     t.start();
    QString name;
    QString id;
    QString app;
    int i = 0;
    while ( (i = data.find( "<Category ",  i)) != -1 ) {

	i += 10;
	name = QString::null;
	app = QString::null;
	while ( 1 ) {
	    // skip white space
	    while ( i < len &&
		    (uc[i] == ' ' || uc[i] == '\n' || uc[i] == '\r') )
		i++;
	    // if at the end, then done
	    if ( i >= len-2 || (uc[i] == '/' && uc[i+1] == '>') )
		break;
	    // we have another attribute read it.
	    int j = i;
	    while ( j < len && uc[j] != '=' )
		j++;
	    QString attr = QConstString( uc+i, j-i ).string();
	    i = ++j; // skip =
	    while ( i < len && uc[i] != '"' )
		i++;
	    j = ++i;
	    while ( j < len && uc[j] != '"' )
		j++;
	    QString value = Qtopia::plainString( QConstString( uc+i, j-i ).string() );
	    i = j + 1;

//  	    qDebug("attr='%s' value='%s'", attr.latin1(), value.latin1() );
	    if ( attr == "id" )
		id = value;
	    else if ( attr == "app" )
		app = value;

	    else if ( attr == "name" )
		name = value;
	}

	if ( name.isNull() || id.isNull() ) {
	    qWarning("No name or id in the category");
	    continue;
	}
	if ( app.isNull() )
	    mGlobalCats.add( id.toInt(), name );
	else
	    mAppCats[ app ].add( id.toInt(), name );
    }

    return TRUE;
}

void Categories::clear()
{
    mGlobalCats.clear();
    mAppCats.clear();
}

void Categories::dump() const
{
    qDebug("\tglobal categories = %s", mGlobalCats.labels().join(", ").latin1() );
    for ( QMap<QString, CategoryGroup>::ConstIterator appsIt = mAppCats.begin(); appsIt != mAppCats.end(); ++appsIt ) {
	const QString &app = appsIt.key();
	QStringList appcats = (*appsIt).labels();
	qDebug("\tapp = %s\tcategories = %s", app.latin1(),
	       appcats.join(", ").latin1() );

    }
}

QStringList CheckedListView::checked() const
{
    QStringList strs;
    for ( QCheckListItem *i = (QCheckListItem *) firstChild();
	  i; i = (QCheckListItem *)i->nextSibling() )
	if ( i->isOn() )
	    strs += i->text( 0 );
    return strs;
}

void CheckedListView::addCheckableList( const QStringList &options )
{
    for ( QStringList::ConstIterator it = options.begin();
	  it != options.end(); ++it ) {
	(void) new QCheckListItem( this, *it,
				   QCheckListItem::CheckBox );
    }
}

void CheckedListView::setChecked( const QStringList &checked )
{
    // iterate over all items
    bool showingChecked = FALSE;
    for ( QCheckListItem *i = (QCheckListItem *) firstChild();
	  i; i = (QCheckListItem *)i->nextSibling() )
	// see if the item should be checked by searching the
	// checked list
	if ( checked.find( i->text( 0 ) ) != checked.end() ) {
	   i->setOn( TRUE );
	   // make sure it is showing at least one checked item
	   if ( !showingChecked ) {
	       ensureItemVisible( i );
	       showingChecked = TRUE;
	   }
	}
       else
	   i->setOn( FALSE );
}
