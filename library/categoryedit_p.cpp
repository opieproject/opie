/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
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

#include "categoryedit_p.h"

#include <qpe/categories.h>

#include <qdir.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qstringlist.h>
#include <qtoolbutton.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>


using namespace Qtopia;

class CategoryEditPrivate
{
public:
    CategoryEditPrivate( QWidget *parent, const QString &appName )
	: mCategories( parent, "" ),
	  mStrApp( appName )
    {
	editItem = 0;
	mCategories.load( categoryFileName() );
    }
    Categories mCategories;
    QListViewItem *editItem;
    QString mStrApp;
    QString mVisible;
};

CategoryEdit::CategoryEdit( QWidget *parent, const char *name )
    : CategoryEditBase( parent, name )
{
    d = 0;
}

CategoryEdit::CategoryEdit( const QArray<int> &recCats,
			    const QString &appName, const QString &visibleName,
			    QWidget *parent, const char *name )
    : CategoryEditBase( parent, name )
{
    d = 0;
    setCategories( recCats, appName, visibleName );
}

void CategoryEdit::setCategories( const QArray<int> &recCats,
				  const QString &appName, const QString &visibleName )
{
    if ( !d )
	d = new CategoryEditPrivate( (QWidget*)parent(), name()  );
    d->mStrApp = appName;
    d->mVisible = visibleName;

    QStringList appCats = d->mCategories.labels( d->mStrApp );
    QArray<int> cats = d->mCategories.ids(d->mStrApp, appCats);
    lvView->clear();

    QStringList::ConstIterator it;
    int i, j;
    for ( i = 0, it = appCats.begin(); it != appCats.end(); i++, ++it ) {
	QCheckListItem *chk;
	chk = new QCheckListItem( lvView, (*it), QCheckListItem::CheckBox );
	if ( !d->mCategories.isGlobal((*it)) )
	    chk->setText( 1, tr(d->mVisible) );
	else
	    chk->setText( 1, tr("All") );
	// Is this record using this category, then we should check it
	for ( j = 0; j < int(recCats.count()); j++ ) {
	    if ( cats[i] == recCats[j] ) {
		chk->setOn( true );
		break;
	    }
	}
    }
    lvView->setSorting( 0, TRUE );
    lvView->sort();
    if ( lvView->childCount() < 1 )
	txtCat->setEnabled( FALSE );
    else {
	lvView->setSelected( lvView->firstChild(), true );
    }
}

CategoryEdit::~CategoryEdit()
{
    if ( d )
	delete d;
}

void CategoryEdit::slotSetText( QListViewItem *selected )
{
    d->editItem = selected;
    if ( !d->editItem )
	return;
    txtCat->setText( d->editItem->text(0) );
    txtCat->setEnabled( true );
    if ( d->editItem->text(1) == tr("All") )
	chkGlobal->setChecked( true );
    else
	chkGlobal->setChecked( false );
}

void CategoryEdit::slotAdd()
{
    QString name = tr( "New Category" );
    bool insertOk = FALSE;
    int num = 0;
    while ( !insertOk ) {
	if ( num++ > 0 )
	    name = tr("New Category ") + QString::number(num);
	insertOk = d->mCategories.addCategory( d->mStrApp, name );
    }
    QCheckListItem *chk;
    chk = new QCheckListItem( lvView, name, QCheckListItem::CheckBox );
    if ( !chkGlobal->isChecked() )
	chk->setText( 1, tr(d->mVisible) );
    else
	chk->setText( 1, tr("All") );

    lvView->setSelected( chk, TRUE );
    txtCat->selectAll();
}

void CategoryEdit::slotRemove()
{
    d->editItem = lvView->selectedItem();
    if ( d->editItem ) {
	QListViewItem *sibling = d->editItem->nextSibling();

	d->mCategories.removeCategory( d->mStrApp, d->editItem->text(0) );

	delete d->editItem;
	d->editItem = 0;

	if ( sibling )
	    lvView->setSelected( sibling, TRUE );
    }
    if ( lvView->childCount() < 1 ) {
	txtCat->clear();
	txtCat->setEnabled( FALSE );
    }
}

void CategoryEdit::slotSetGlobal( bool isChecked )
{
    if ( d->editItem ) {
	if ( isChecked )
	    d->editItem->setText( 1, tr("All") );
	else
	    d->editItem->setText( 1, tr(d->mVisible) );

	d->mCategories.setGlobal( d->mStrApp, d->editItem->text( 0 ), isChecked );
    }
}

void CategoryEdit::slotTextChanged( const QString &strNew )
{
    if ( d->editItem ) {
	if ( chkGlobal->isChecked() )
	    d->mCategories.renameGlobalCategory( d->editItem->text(0), strNew );
	else
	    d->mCategories.renameCategory( d->mStrApp, d->editItem->text(0), strNew );
	d->editItem->setText( 0, strNew );
    }
}

QArray<int> CategoryEdit::newCategories()
{
    QArray<int> a;
    if ( d ) {
	d->mCategories.save( categoryFileName() );
	QListViewItemIterator it( lvView );
	QValueList<int> l;
	for ( ; it.current(); ++it ) {
	    if ( reinterpret_cast<QCheckListItem*>(it.current())->isOn() )
		l.append( d->mCategories.id( d->mStrApp, it.current()->text(0) ) );
	}
	uint i = 0;
	a.resize( l.count() );
	for ( QValueList<int>::Iterator lit = l.begin(); lit != l.end(); ++lit )
	    a[i++] = *lit;
    }
    return a;
}

void CategoryEdit::accept()
{
    // write our categories out...
    d->mCategories.save( categoryFileName() );
    //    QDialog::accept();
}

QString categoryFileName()
{
    QDir dir = (QString(getenv("HOME")) + "/Settings");
    if ( !dir.exists() )
	mkdir( dir.path().local8Bit(), 0700 );
    return dir.path() + "/" + "Categories" + ".xml";
}

void CategoryEdit::kludge()
{
    lvView->setMaximumHeight( 130 );
}
