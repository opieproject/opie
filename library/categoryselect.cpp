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

#include <qpe/categories.h>

#include <qdialog.h>
#include <qlayout.h>
#include <qtoolbutton.h>

#include "categorywidget.h"
#include "categoryselect.h"


class CategoryComboPrivate
{
public:
    CategoryComboPrivate(QObject *o)
	: mCat( o )
    {
    }
    QArray<int> mAppCats;
    QString mStrAppName;
    QString mStrVisibleName;
    Categories mCat;
};

class CategorySelectPrivate
{
public:
    CategorySelectPrivate( const QArray<int> &cats)
	: mRec( cats ),
	  usingAll( false )
    {
    }
    CategorySelectPrivate()
    {
    }
    QArray<int> mRec;
    bool usingAll;
    QString mVisibleName;
};

CategoryCombo::CategoryCombo( QWidget *parent, const char *name )
    : QComboBox( parent, name )
{
    d = new CategoryComboPrivate(this);
}

void CategoryCombo::initCombo( const QArray<int> &recCats,
			       const QString &appName )
{
    initCombo( recCats, appName, appName );
}

void CategoryCombo::initCombo( const QArray<int> &recCats,
			       const QString &appName,
			       const QString &visibleName )
{
    d->mStrAppName = appName;
    d->mStrVisibleName = visibleName;
    clear();
    QStringList slApp;

    QObject::connect( this, SIGNAL(activated(int)),
		      this, SLOT(slotValueChanged(int)) );
    bool loadOk = d->mCat.load( categoryFileName() );
    slApp = d->mCat.labels( d->mStrAppName, TRUE, Categories::UnfiledLabel );
    d->mAppCats = d->mCat.ids( d->mStrAppName );

    int i,
	j,
	saveMe,
	recCount;
    QStringList::Iterator it;
    // now add in all the items...
    recCount = recCats.count();
    saveMe = -1;
    if ( recCount > 1 && loadOk ) {
	it = slApp.begin();
	insertItem( *it );
	++it;
	for ( j = 0; it != slApp.end(); ++it, j++ ) {
	    // grr... we have to go through and compare...
	    if ( j < int(d->mAppCats.size()) ) {
		for ( i = 0; i < recCount; i++ ) {
		    if ( recCats[i] == d->mAppCats[j] ) {
			(*it).append( tr(" (Multi.)") );
			if ( saveMe < 0 )
			    saveMe = j;
			// no need to continue through the list.
			break;
		    }
		}
	    }
	    insertItem( *it );
	}
    } else
	insertStringList( slApp );

    if ( recCount > 0 && loadOk ) {
	for ( i = 0; i < int(d->mAppCats.size()); i++ ) {
	    if ( d->mAppCats[i] == recCats[0] ) {
		setCurrentItem( i + 1 );
		break;
	    }
	}
    } else
	setCurrentItem( 0 );  // unfiled
    QObject::connect( this, SIGNAL(activated(int)),
		      this, SLOT(slotValueChanged(int)) );
}

CategoryCombo::~CategoryCombo()
{
    delete d;
}

int CategoryCombo::currentCategory() const
{
    int returnMe;
    returnMe = currentItem();
    // unfiled is now 0...
    if ( returnMe == 0 )
	returnMe = -1;
    else if ( returnMe > (int)d->mAppCats.count() )  // only happen on "All"
	returnMe = -2;
    else
	returnMe =  d->mAppCats[returnMe - 1];
    return returnMe;
}

void CategoryCombo::setCurrentCategory( int newCatUid )
{
    int i;
    for ( i = 0; i < int(d->mAppCats.size()); i++ ) {
	if ( d->mAppCats[i] == newCatUid )
	    setCurrentItem( i );
    }
}

void CategoryCombo::setCurrentText( const QString &str )
{
    int i;
    int stop;
    stop = count();
    for ( i = 0; i < stop; i++ ) {
	if ( text( i ) == str ) {
	    setCurrentItem( i );
	    break;
	}
    }
}

void CategoryCombo::slotValueChanged( int )
{
    emit sigCatChanged( currentCategory() );
}

CategorySelect::CategorySelect( QWidget *parent, const char *name )
    : QHBox( parent, name ),
      cmbCat( 0 ),
      cmdCat( 0 ),
      d( 0 )
{
    d = new CategorySelectPrivate();
    init();
}

CategorySelect::CategorySelect( const QArray<int> &vl,
				const QString &appName, QWidget *parent,
				const char *name )
    : QHBox( parent, name )
{
    d = new CategorySelectPrivate( vl );
    init();
    setCategories( vl, appName, appName );
}

CategorySelect::CategorySelect( const QArray<int> &vl,
				const QString &appName,
				const QString &visibleName,
				QWidget *parent, const char *name )
    : QHBox( parent, name )
{
    d = new CategorySelectPrivate( vl );
    init();
    setCategories( vl, appName, visibleName );
}

CategorySelect::~CategorySelect()
{
    delete d;
}

void CategorySelect::slotDialog()
{
    QDialog editDlg( this, 0, TRUE );
    editDlg.setCaption( tr("Edit Categories") );
    QVBoxLayout *vb = new QVBoxLayout( &editDlg );
    QScrollView *sv = new QScrollView( &editDlg );
    sv->setResizePolicy( QScrollView::AutoOneFit );
    sv->setHScrollBarMode( QScrollView::AlwaysOff );
    vb->addWidget( sv );
    CategoryWidget ce( d->mRec, mStrAppName, d->mVisibleName, &editDlg );
    sv->addChild( &ce );
    editDlg.showMaximized();

    if ( editDlg.exec() ) {
	d->mRec = ce.newCategories();
	cmbCat->initCombo( d->mRec, mStrAppName );
    }
}

void CategorySelect::slotNewCat( int newUid )
{
    if ( newUid != -1 ) {
	bool alreadyIn = false;
	for ( uint it = 0; it < d->mRec.count(); ++it ) {
	    if ( d->mRec[it] == newUid ) {
		alreadyIn = true;
		break;
	    }
	}
	if ( !alreadyIn ) {
	    d->mRec.resize( 1 );
	    d->mRec[ 0 ] = newUid;
	}
    } else
    d->mRec.resize(0);  // now Unfiled.
    emit signalSelected( currentCategory() );
}

void CategorySelect::setCategories( const QArray<int> &rec,
				    const QString &appName )
{
    setCategories( rec, appName, appName );
}

void CategorySelect::setCategories( const QArray<int> &rec,
				    const QString &appName,
				    const QString &visibleName )
{
    d->mRec = rec;
    d->mVisibleName = visibleName;
    mStrAppName = appName;
    cmbCat->initCombo( rec, appName );
}

void CategorySelect::init()
{
    cmbCat = new CategoryCombo( this );
    QObject::connect( cmbCat, SIGNAL(sigCatChanged(int)),
		      this, SLOT(slotNewCat(int)) );
    cmdCat = new QToolButton( this );
    QObject::connect( cmdCat, SIGNAL(clicked()), this, SLOT(slotDialog()) );
    cmdCat->setTextLabel( "...", FALSE );
    cmdCat->setUsesTextLabel( true );
    cmdCat->setMaximumSize( cmdCat->sizeHint() );
    cmdCat->setFocusPolicy( TabFocus );
}


int CategorySelect::currentCategory() const
{
    return cmbCat->currentCategory();
}

void CategorySelect::setCurrentCategory( int newCatUid )
{
    cmbCat->setCurrentCategory( newCatUid );
}


const QArray<int> &CategorySelect::currentCategories() const
{
    return d->mRec;
}

void CategorySelect::setRemoveCategoryEdit( bool remove )
{
    if ( remove ) {
	cmdCat->setEnabled( FALSE );
	cmdCat->hide();
    } else {
	cmdCat->setEnabled( TRUE );
	cmdCat->show();
    }
}

void CategorySelect::setAllCategories( bool add )
{
    d->usingAll = add;
    if ( add ) {
	cmbCat->insertItem( tr( "All" ), cmbCat->count() );
	cmbCat->setCurrentItem( cmbCat->count() - 1 );
    } else
	cmbCat->removeItem( cmbCat->count() - 1 );
}
