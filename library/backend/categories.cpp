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

/*! \class CategoryGroup categories.h
  \brief Helper class that is used by Categories

  CategoryGroup is a group of categories that is associated with an
  application or global set. Mainly it defines a map of ids to
  category labels and category labels to ids. Lookups can be done with
  labels or unique idenifiers.

  \ingroup qtopiaemb
  \ingroup qtopiadesktop
  \warning Categories API will likely change between Qtopia 1.5 and Qtopia 3
  \sa Categories::appGroupMap(), Categories::globalGroup()
 */

/*! Add \a label and return the UID. If failure, then 0 is returned. Note
  that All and Unfiled are reserved labels.
  \internal
*/
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

/*! \internal
 */
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

/*! Returns TRUE if \a label was removed from the group, FALSE if not.
  \internal
 */
bool CategoryGroup::remove( const QString &label )
{
    QMap<QString,int>::Iterator findIt = mLabelIdMap.find( label );
    if ( findIt == mLabelIdMap.end() )
	return FALSE;

    mIdLabelMap.remove( *findIt );
    mLabelIdMap.remove( findIt );

    return TRUE;
}

/*! Returns TRUE if \a uid was removed from the group, FALSE if not.
  \internal
 */
bool CategoryGroup::remove( int uid )
{
    QMap<int,QString>::Iterator idIt = mIdLabelMap.find( uid );
    if ( idIt == mIdLabelMap.end() )
	return FALSE;

    mLabelIdMap.remove( *idIt );
    mIdLabelMap.remove( idIt );

    return TRUE;
}

/*! \internal
 */
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

/*! \internal
 */
bool CategoryGroup::rename( const QString &oldLabel, const QString &newLabel )
{
    return rename( id(oldLabel), newLabel );
}

/*! Returns TRUE if \a uid is stored in this group, FALSE if not. */
bool CategoryGroup::contains(int uid) const
{
    return ( mIdLabelMap.find( uid ) != mIdLabelMap.end() );
}

/*! Returns TRUE if \a label is stored in this group, FALSE if not. */
bool CategoryGroup::contains(const QString &label) const
{
    return ( mLabelIdMap.find( label ) != mLabelIdMap.end() );
}

/*! Returns label associated with the \a uid or QString::null if
  not found
 */
const QString &CategoryGroup::label(int uid) const
{
    QMap<int,QString>::ConstIterator idIt = mIdLabelMap.find( uid );
    if ( idIt == mIdLabelMap.end() )
	return QString::null;
    return *idIt;
}

/*! Returns the uid associated with \a label or 0 if not found */
int CategoryGroup::id(const QString &label) const
{
    QMap<QString,int>::ConstIterator labelIt = mLabelIdMap.find( label );
    if ( labelIt == mLabelIdMap.end() )
	return 0;
    return *labelIt;
}

/*! Returns a list of all labels stored in this group. */
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

/*! Returns a list of all labels associated with the \a catids */
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

/*!
  \class Categories categories.h
  \brief Category database that groups categories and maps ids to names.

  The Categories class is the low level Categories accessor class. To
  add a category menu and filter for your application, see CategoryMenu.

  The Categories class allows the developer to add, remove, and rename
  categories. Categories can be created for an individual application
  such as Todo List or to be used for all applications. Categories
  that can be used by all applications are called global
  categories. Each PalmtopRecord subclass stores categories as an
  QArray<int> using PalmtopRecord::setCategories() and
  PalmtopRecord::categories(). This allows each record to be assigned
  to multiple categories. This also allows the user to rename a
  category and for it to update automatically in all records.

  This class provides several methods to convert between a category id
  and it's associated string such as id(), ids(), label() and labels(). A
  helper class called CategoryGroup is used to access categories of a
  single application group, such as Todo List. Global categories can
  also be accessed via CategoryGroup objects.  See appGroupMap() and
  globalGroup() for the appropriate accessor methods.

  Categories are stored in an xml file in the Settings directory
  (Categories.xml). A global function called categoryFileName() will
  return to appropriate QString file location to be passed to load()
  and save() for the master categories database.

  \ingroup qtopiaemb
  \ingroup qtopiadesktop
  \warning Categories API will likely change between Qtopia 1.5 and Qtopia 3
  \sa CategoryGroup, CategoryMenu
*/


/*!
  Add the category name as long as it doesn't already exist locally or
  globally.  The \a uid is assigned to the category if successfully
  added. Return \a uid if added, 0 if conflicts (error).

  \internal
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

/*!
  Add the category name as long as it doesn't already exist locally or
  globally.  Return UID if added, 0 if conflicts (error).
*/

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

/*!
  \internal
*/
int Categories::addGlobalCategory( const QString &catname, int uid )
{
    mGlobalCats.add( uid, catname );
    emit categoryAdded( *this, QString::null, uid );
    return uid;
}

/*!
  Add the global category \a catname while checking that it doesn't
  already exist globally. Return UID if added, 0 if conflicts.

  \sa addCategory()
 */
int Categories::addGlobalCategory( const QString &catname )
{
    int uid = mGlobalCats.add( catname );
    if ( !uid )
	return 0;
    emit categoryAdded( *this, QString::null, uid );
    return uid;
}

/*!

  Removes the \a catname from the application group. If it is not
  found in the application group and \a checkGlobal is TRUE, then it
  attempts to remove it from the global list
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


/*!
  Removes the \a uid from the application group \a appname. Returns TRUE
  if success, FALSE if not found.
*/
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

/*!
  Removes the global category \a catname. Returns TRUE
  if success, FALSE if not found.
*/
bool Categories::removeGlobalCategory( const QString &catname )
{
    int uid = mGlobalCats.id( catname );
    if ( mGlobalCats.remove( uid ) ) {
	emit categoryRemoved( *this, QString::null, uid );
	return TRUE;
    }
    return FALSE;
}

/*!
  Removes the global category \a uid. Returns TRUE
  if success, FALSE if not found.
*/
bool Categories::removeGlobalCategory( int uid )
{
     if ( mGlobalCats.remove( uid ) ) {
	emit categoryRemoved( *this, QString::null, uid );
	return TRUE;
    }
    return FALSE;
}

/*!
  Returns the sorted list of all categories that are associated with
  the \a app.  If \a includeGlobal is TRUE then the returned
  categories will include the global category items.
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

/*!
  Returns the label associated with the id
*/
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

/*!
  Returns a single string associated with \a catids for display in a
  combobox or any area that requires one string.  If \a catids are empty
  then "Unfiled" will be returned.  If multiple categories are
  assigned then the behavior depends on the DisplaySingle type.

  If \a display is set to ShowMulti then " (multi)" appended to the
  first string. If \a display is set to ShowAll, then a space
  seperated string is returned with all categories.  If ShowFirst is
  set, the just the first string is returned.
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

/*!

  Returns all ids associated with the application CategoryGroup \a app
  and the passed in \a labels in that group.
*/
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

/*!
  Returns the id associated with the app. If the id is not found in the
  application CategoryGroup, then it searches the global CategoryGroup.
  If it is not found it either, 0 is returned.
*/
int Categories::id( const QString &app, const QString &cat ) const
{
    if ( cat == tr("Unfiled") || cat.contains( tr(" (multi.)") ) )
	return 0;
    int uid = mGlobalCats.id( cat );
    if ( uid != 0 )
	return uid;
    return mAppCats[app].id( cat );
}


/*!
  Return TRUE if renaming succeeded; FALSE if \a appname or \a oldName
  is not found, or if \a newName conflicts with an existing category
  in the CategoryGroup.

  It will first search the CategoryGroup associated with \a appname
  and if not found it will try to replace in global CategoryGroup.
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

/*!
  Return TRUE if renaming succeeded; FALSE if \a appname or \a oldName
  is not found, or if \a newName conflicts with an existing category
  in the CategoryGroup. This function will only rename categories found
  in the global CategoryGroup.
 */
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

/*!
  Changes the grouping of a category. If a category was global and \a global
  is set to TRUE, then the \a catname will be moved to the \a appname group.
*/
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

/*!
  Returns TRUE if the \a catname is in the global CategoryGroup, FALSE if not.
*/
bool Categories::isGlobal( const QString &catname ) const
{
    return mGlobalCats.contains( catname );
}


/*!
  Returns true if the \a catname is associated with any CategoryGroup,
  including global.
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

/*!
  Returns TRUE if the \a catname is associated with the \a appname
  CategoryGroup, FALSE if not found.
 */
bool Categories::exists( const QString &appname,
			 const QString &catname) const
{
    QMap< QString, CategoryGroup >::ConstIterator
	appIt = mAppCats.find( appname );

    if ( appIt == mAppCats.end() )
	return FALSE;

    return (*appIt).contains( catname );
}

/*!
  Saves the Categories database to the \a fname. See categoryFileName()
  for the default file name string used for the shared category database.

  Returns FALSE if there is error writing the file or TRUE on success.
 */
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

/*!
  Loads the Categories database using \a fname. See categoryFileName()
  for the default file name string used for the shared category database.

  Returns FALSE if there is error reading the file or TRUE on success.
 */
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

/*!
  Clear the categories in memory. Equivelent to creating an empty Categories
  object.
*/
void Categories::clear()
{
    mGlobalCats.clear();
    mAppCats.clear();
}

/*!
  Dump the contents to standard out. Used for debugging only.
*/
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

/*! \fn Categories &Categories::operator= ( const Categories &c )

  Performs deep copy.
 */


/*! \fn  QStringList Categories::labels( const QString & app, const QArray<int> &catids ) const

  Returns list of labels associated with the application and catids
*/

/*! \fn QStringList Categories::globalCategories() const

  Returns list of all global category labels
*/

/*! \fn const QMap<QString, CategoryGroup> &Categories::appGroupMap() const

  Returns a map of application names to CategoryGroup. The CategoryGroup
  class defines a map of ids to category labels and category labels to ids.
*/

/*! \fn const CategoryGroup &Categories::globalGroup() const

  Returns the global CategoryGroup. The CategoryGroup
  class defines a map of ids to category labels and category labels to ids.
*/

/*! \fn void Categories::categoryAdded( const Categories &cats, const QString &appname, int uid)

  Emitted if a category is added.

  \a cats is a const reference to this object
  \a appname is the CategoryGroup application name that the category was added to or QString::null if it was global
  \a uid is the unique identifier associated with the added category
*/	

/*! \fn void Categories::categoryRemoved( const Categories &cats, const QString &appname,
  int uid)

  Emitted if removed category is removed.

  \a cats is a const reference to this object
  \a appname is the CategoryGroup application name that the category was removed from or QString::null if it was the global CategoryGroup
  \a uid is the unique identifier associated with the removed category
*/


/*! \fn void Categories::categoryRenamed( const Categories &cats, const QString &appname,
  int uid)

  Emitted if \a uid in the \a appname CategoryGroup is renamed in \a cats
  object.

  \a cats is a const reference to this object
  \a appname is the CategoryGroup application name that the category was renamed in or QString::null if it was the global CategoryGroup
  \a uid is the unique identifier associated with the renamed category
*/

/*! \fn Categories::Categories( QObject *parent=0, const char *name = 0 )

  Constructor for an empty Categories object.
*/

/*! \fn Categories::Categories( const Categories &copyFrom )

  Deep copy constructor
*/

/*! \fn Categories::~Categories()

  Empty destructor. Call save() before destruction if there are changes
  that need to be saved.
*/

/*! \fn CategoryGroup::clear()
  \internal
*/

/*! \fn const QMap<int, QString> &CategoryGroup::idMap() const

  Returns a const reference to the id to label QMap
*/

/*! \fn CategoryGroup::CategoryGroup()
  \internal
*/

/*! \fn CategoryGroup::CategoryGroup(const CategoryGroup &c)
  \internal
*/

