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

#ifndef QTPALMTOP_CATEGORIES_H
#define QTPALMTOP_CATEGORIES_H

#include <qstring.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qlistview.h>
#include <qarray.h>
#include "qpcglobal.h"
#include "palmtopuidgen.h"

class CategoryGroup;

#if defined(QPC_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class QPC_EXPORT QMap<int, QString>;
template class QPC_EXPORT QMap<QString, int>;
template class QPC_EXPORT QMap< QString, CategoryGroup >;
// MOC_SKIP_END
#endif

class QPC_EXPORT CategoryGroup
{
    friend class Categories;
public:
    CategoryGroup(): mIdLabelMap(), mLabelIdMap() { }
    CategoryGroup( const CategoryGroup &c ) :
	mIdLabelMap( c.mIdLabelMap), mLabelIdMap( c.mLabelIdMap ) { }

    void clear() { mIdLabelMap.clear(); mLabelIdMap.clear(); }

    int add( const QString &label );
    bool add( int uid, const QString &label );

    bool remove( const QString &label );
    bool remove( int uid );

    bool rename( int uid, const QString &newLabel );
    bool rename( const QString &oldLabel, const QString &newLabel );

    bool contains(int id) const;
    bool contains(const QString &label) const;

    /** Returns label associated with the uid or QString::null if
    *   not found
    */
    const QString &label(int id) const;
    /** Returns the uid associated with label or 0 if not found */
    int id(const QString &label) const;

    /** Returns a sorted list of labels */
    QStringList labels() const;
    QArray<int> ids( const QStringList &cats ) const;
    QArray<int> ids() const;
    QStringList labels( const QArray<int> &catids ) const;

    const QMap<int, QString> &idMap() const { return mIdLabelMap; }

private:
    void insert( int uid, const QString &label );
    QMap<int, QString> mIdLabelMap;
    QMap<QString, int> mLabelIdMap;

    static Qtopia::UidGen &uidGen() { return sUidGen; }
    static Qtopia::UidGen sUidGen;
};

/** Map from application name to categories */
class QPC_EXPORT Categories : public QObject
{
    Q_OBJECT
public:
    Categories( QObject *parent=0, const char *name = 0 )
	: QObject( parent, name ),  mGlobalCats(), mAppCats() { }
    Categories( const Categories &copyFrom ) : QObject( copyFrom.parent() ),
	mGlobalCats( copyFrom.mGlobalCats ),
    mAppCats( copyFrom.mAppCats ) { }
    virtual ~Categories() { }

    Categories &operator= ( const Categories &c )
{ mAppCats = c.mAppCats; mGlobalCats = c.mGlobalCats; return *this; }

    void clear();

    /** Add the category name as long as it doesn't already exist
    *  locally or globally.  Return UID if added, 0 if conflicts
    *  (error).
    */
    int addCategory( const QString &appname, const QString &catname);
    /** Add the category name as long as it doesn't already exist
    *  locally or globally.  Return UID if added, 0 if conflicts
    *  (error).
    */
    int addCategory( const QString &appname, const QString &catname, int uid);
    /** Add the global category just checking that it doesn't
    *  already exist globally. Return UID if added, 0 if conflicts.
    */
    int addGlobalCategory( const QString &catname );
    /** Add the global category just checking that it doesn't
    *  already exist globally. Return UID if added, 0 if conflicts.
    */
    int addGlobalCategory( const QString &catname, int uid );
    /** Removes the category from the application; if it is not found
    *   in the application, then it removes it from the global list
    */
    bool removeCategory( const QString &appName, const QString &catName,
			 bool checkGlobal = TRUE);
    bool removeCategory( const QString &appName, int uid );
    bool removeGlobalCategory( const QString &catName );
    bool removeGlobalCategory( int uid );

    QArray<int> ids( const QString &app ) const;
    QArray<int> ids( const QString &app,
		     const QStringList &cats ) const;
    /** Returns the id associated with the app */
    int id( const QString &app, const QString &cat ) const;
    /** Returns the label associated with the id */
    QString label( const QString &app, int id ) const;

    enum ExtraLabels { NoExtra, AllUnfiled, AllLabel, UnfiledLabel };
    /** Returns the sorted list of all categories that are
    *   associated with the app.
    *   If includeGlobal parameter is TRUE then the returned
    *   categories will include the global category items.
    *   If extra = NoExtra, then
    *   If extra = AllUnfiled, then All and Unfiled will be prepended to
    *   the list
    *   If extra = AllLabel, then All is prepended
    *   If extra = UnfiledLabel, then Unfiled is prepended
    */
    QStringList labels( const QString &app,
			bool includeGlobal = TRUE,
			ExtraLabels extra = NoExtra ) const;

    /** Returns the labels of the categories associated with the uids */
    QStringList labels( const QString & app,
			const QArray<int> &catids ) const;

    enum DisplaySingle { ShowMulti, ShowAll, ShowFirst };

    /** Returns a single string associated with the cat ids for display in
    *   a combobox or any area that requires one string.  If catids are empty
    *   then "Unfiled" will be returned.  If multiple categories are assigned
    *   then the behavior depends on the DisplaySingle type.
    *   If /a display is set to ShowMulti then  " (multi)" appended to the
    *   first string. If /a display is set to ShowAll, then a space seperated
    *   string is returned with all categories.  If ShowFirst is returned,
    *   the just the first string is returned.
    */
    QString displaySingle( const QString &app,
			   const QArray<int> &catids,
			   DisplaySingle display ) const;

    QStringList globalCategories() const { return mGlobalCats.labels();}

    bool renameCategory( const QString &appname,
			 const QString &oldName,
			 const QString &newName );
    bool renameGlobalCategory( const QString &oldName,
			       const QString &newName );

    void setGlobal( const QString &appname, const QString &catname,
		    bool value );
    bool isGlobal( const QString &catname ) const;


    /** Returns true if the catname is associated with any application
    */
    bool exists( const QString &catname ) const;
    bool exists( const QString &appname, const QString &catname) const;

    bool save( const QString &fname ) const;
    bool load( const QString &fname );

    // for debugging
    void dump() const;

    const QMap<QString, CategoryGroup> &appGroupMap() const{ return mAppCats; }
    const CategoryGroup &globalGroup() const { return mGlobalCats; }

signals:
    /** emitted if added a category;
    *  the second param is the application the category was added to
    *  or null if global
    *  the third param is the uid of the newly added category
    */
    void categoryAdded( const Categories &, const QString &, int );
    /** emitted if removed a category
    *  the second param is the application the category was removed from
    *  or null if global
    *  the third param is the uid of the removed category
    */
    void categoryRemoved( const Categories &, const QString &, int );
    /** emitted if a category is renamed; the second param is the uid of
    *   the removed category */
    void categoryRenamed( const Categories &, const QString &, int );

private:
    CategoryGroup mGlobalCats;
    QMap< QString, CategoryGroup > mAppCats;
};

class QPC_EXPORT CheckedListView : public QListView
{
public:
    void addCheckableList( const QStringList &options );
    void setChecked( const QStringList &checked );
    QStringList checked() const;
};

#endif
