/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
** Copyright (C) 2003 zecke introduce a needed symbol
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
#include <qpe/palmtoprecord.h>

#include <qmessagebox.h>
#include <qlayout.h>
#include <qtoolbutton.h>
#include <qfile.h>

#include "categorywidget.h"
#include "categoryselect.h"

#include <stdlib.h>


/*! \enum CategorySelect::SelectorWidget
  Chooses a type of widget to use as the selection widget.

  \value ComboBox
  \value ListView
*/

/*!
  \class CategorySelect
  \brief The CategorySelect widget allows users to select Categories with a
  combobox interface.

  CategorySelect is useful to provide a QComboBox of Categories for
  filtering (such as in the Contacts table view) or to allow the user
  to select multiple Categories. The allCategories variable sets
  whether the CategorySelect is in filtering or selecting mode.

  In filtering mode, the All and Unfiled categories are added. The

  In selecting mode, the CategorySelect may either be a QComboBox and
  a QToolButton or a QListView with checkable items depending on the
  screen size.

  CategorySelect automatically updates itself if Categories has been
  changed elsewhere in the environment.

  Signals and slots are provided to notify the application of the users
  selections.  A QToolButton is also provided so that users can edit the
  Categories manually.

  \ingroup qtopiaemb
*/


static QString categoryEdittingFileName()
{
    QString str = getenv("HOME");
    str +="/.cateditting";
    return str;
}

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

CategoryCombo::CategoryCombo( QWidget *parent, const char *name , int width)
    : QComboBox( parent, name )
{
    d = new CategoryComboPrivate(this);
    if (width)
      setFixedWidth(width);
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

    QObject::disconnect( this, SIGNAL(activated(int)),
		      this, SLOT(slotValueChanged(int)) );

    QObject::connect( this, SIGNAL(activated(int)),
		      this, SLOT(slotValueChanged(int)) );
    bool loadOk = d->mCat.load( categoryFileName() );
    slApp = d->mCat.labels( d->mStrAppName, TRUE, Categories::UnfiledLabel );

    d->mAppCats = d->mCat.ids( d->mStrAppName, slApp);

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
	for ( j = 0; j< (int)(slApp.count()-1); ++it, j++ ) {
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
	insertItem( *it );
    } else
	insertStringList( slApp );

    if ( recCount > 0 && loadOk ) {
	for ( i = 0; i < int(d->mAppCats.size()); i++ ) {
	    if ( d->mAppCats[i] == recCats[0] ) {
		setCurrentItem( i );
		break;
	    }
	}
    } else
    {
	setCurrentItem( slApp.count()-1 );  // unfiled
    }
}

// this is a new function by SHARP instead of initCombo()
QArray<int> CategoryCombo::initComboWithRefind( const QArray<int> &recCats,
			       const QString &appName)
{
    QString visibleName = appName;
    d->mStrAppName = appName;
    d->mStrVisibleName = visibleName;
    clear();
    QStringList slApp;
    QArray<int> results;

    QObject::disconnect( this, SIGNAL(activated(int)),
		      this, SLOT(slotValueChanged(int)) );

    QObject::connect( this, SIGNAL(activated(int)),
		      this, SLOT(slotValueChanged(int)) );
    bool loadOk = d->mCat.load( categoryFileName() );
    slApp = d->mCat.labels( d->mStrAppName, TRUE, Categories::UnfiledLabel );

    d->mAppCats = d->mCat.ids( d->mStrAppName, slApp);

    // addition part
    // make new recCats
    if (loadOk){
      int i,j;
      int value;
      int rCount = recCats.count();
      int mCount = d->mAppCats.count();

      for (i=0; i<rCount; i++){
	value = 0;
	for (j=0; j<mCount; j++){
	  if (recCats[i] == d->mAppCats[j]){
	    value = recCats[i];
	    break;
	  }
	}
	if (value != 0){
	  int tmp = results.size();
	  results.resize( tmp + 1 );
	  results[ tmp ] = value;
	}
      }
    }
    else{
      results = recCats;
    }
    // addition end

    int i,
	j,
	saveMe,
	recCount;
    QStringList::Iterator it;
    // now add in all the items...
    recCount = results.count();
    saveMe = -1;
    if ( recCount > 1 && loadOk ) {
	it = slApp.begin();
	for ( j = 0; j< (int)(slApp.count()-1); ++it, j++ ) {

	    // grr... we have to go through and compare...
	    if ( j < int(d->mAppCats.size()) ) {
		for ( i = 0; i < recCount; i++ ) {
		    if ( results[i] == d->mAppCats[j] ) {
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
	insertItem( *it );
    } else
	insertStringList( slApp );

    if ( recCount > 0 && loadOk ) {
	for ( i = 0; i < int(d->mAppCats.size()); i++ ) {
	    if ( d->mAppCats[i] == results[0] ) {
		setCurrentItem( i );
		break;
	    }
	}
    } else
    {
	setCurrentItem( slApp.count()-1 );  // unfiled
    }
/*
    QObject::connect( this, SIGNAL(activated(int)),
		      this, SLOT(slotValueChanged(int)) );
*/
    return results;
}


CategoryCombo::~CategoryCombo()
{
    delete d;
}

int CategoryCombo::currentCategory() const
{
    int returnMe;
    returnMe = currentItem();

    if ( returnMe == (int)d->mAppCats.count() )
	returnMe = -1;
    else if ( returnMe > (int)d->mAppCats.count() )  // only happen on "All"
	returnMe = -2;
    else
	returnMe =  d->mAppCats[returnMe];
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

/*!
  Constructs a category selector with parent \a parent, name \a name.

  This constructor is provided to make Opie compatible with Sharp ROM.
*/
CategorySelect::CategorySelect( QWidget* parent,  const char* name )
    : QHBox( parent, name ),
      cmbCat( 0 ),
      cmdCat( 0 ),
      d( 0 )
{
    d = new CategorySelectPrivate();
    init(0); // default argument
}
/*!
  Constructs a category selector with parent \a parent, name \a name and
  fixed width \a width.

  This constructor is provided to make integration with Qt Designer easier.
*/
CategorySelect::CategorySelect( QWidget *parent, const char *name,int width)
    : QHBox( parent, name ),
      cmbCat( 0 ),
      cmdCat( 0 ),
      d( 0 )
{
    d = new CategorySelectPrivate();
    init(width);
}

/*!
  \overload
  This constructor accepts an array \a vl of integers representing Categories.
  \a appName is used as the visible name string.
*/
CategorySelect::CategorySelect( const QArray<int> &vl,
				const QString &appName, QWidget *parent,
				const char *name ,int width)
    : QHBox( parent, name )
{
    d = new CategorySelectPrivate( vl );

    init(width);

    setCategories( vl, appName, appName );
}

/*!
  \overload

  This constructor accepts an array \a vl of integers representing Categories.
  \a visibleName is the string used when the name of this widget is required
  to be displayed. \a width is an integer used as the fixed width of the widget.
*/
CategorySelect::CategorySelect( const QArray<int> &vl,
				const QString &appName,
				const QString &visibleName,
				QWidget *parent, const char *name , int width)
    : QHBox( parent, name )
{
    d = new CategorySelectPrivate( vl );
    init(width);
    setCategories( vl, appName, visibleName );
}

/*!
  Destructs a CategorySelect widget.
*/
CategorySelect::~CategorySelect()
{
    delete d;
}

/*!
  This slot is called when the user pushes the button to edit Categories.
*/
void CategorySelect::slotDialog()
{
    if (QFile::exists( categoryEdittingFileName() )){
        QMessageBox::warning(this,tr("Error"),
	   tr("Sorry, another application is\nediting categories.") );
        return;
    }

    QFile f( categoryEdittingFileName() );
    if ( !f.open( IO_WriteOnly) ){
        return;
    }

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

    f.close();
    QFile::remove( categoryEdittingFileName() );
}


/*!
  This slot is called when a new Category is available.
*/
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

/*!
  \overload

  Resets the CategorySelect to select the \a vlCats for
  the Categories assoicated with \a appName.

  This function should only be called if <i>filtering</i>
  on Categories and not selecting and therefore possibly
  allowing the user to edit Categories.
*/
QString CategorySelect::setCategories( const QArray<int> &rec,
				    const QString &appName )
{
    return setCategories( rec, appName, appName );
}

/*!
  Resets the CategorySelect to select the \a vlCats for
  the Categories assoicated with \a appName and displays
  the \a visibleName if the user is selecting and therefore editing
  new Categories.
 */
QString CategorySelect::setCategories( const QArray<int> &rec,
				    const QString &appName,
				    const QString &visibleName )
{
    d->mVisibleName = visibleName;
    mStrAppName = appName;
    d->mRec = cmbCat->initComboWithRefind( rec, appName );
    return Qtopia::Record::idsToString(d->mRec);
}

void CategorySelect::init(int width)
{
    cmbCat = new CategoryCombo( this, 0, width);

    QObject::connect( cmbCat, SIGNAL(sigCatChanged(int)),
		      this, SLOT(slotNewCat(int)) );
    cmdCat = new QToolButton( this );
    QObject::connect( cmdCat, SIGNAL(clicked()), this, SLOT(slotDialog()) );
    cmdCat->setTextLabel( "...", FALSE );
    cmdCat->setUsesTextLabel( true );
    cmdCat->setMaximumSize( cmdCat->sizeHint() );
    cmdCat->setFocusPolicy( TabFocus );
}

/*!
  Return the value of the currently selected category.
 */
int CategorySelect::currentCategory() const
{
    return cmbCat->currentCategory();
}


void CategorySelect::setCurrentCategory( int newCatUid )
{
    cmbCat->setCurrentCategory( newCatUid );
}

/*!
  Returns a shallow copy of the categories in this CategorySelect.
 */
const QArray<int> &CategorySelect::currentCategories() const
{
    return d->mRec;
}

/*!
  Hides the edit section of the CategorySelect widget if \a remove is TRUE.
 */
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

/*!
  Changes this CategorySelect to the All category if \a all is TRUE.
 */
void CategorySelect::setAllCategories( bool add )
{
    d->usingAll = add;
    if ( add ) {
	cmbCat->insertItem( tr( "All" ), cmbCat->count() );
	cmbCat->setCurrentItem( cmbCat->count() - 1 );
    } else
	cmbCat->removeItem( cmbCat->count() - 1 );
}

// 01.12.21 added
/*!
  Sets the fixed width of the widget to \a width.
  */
void CategorySelect::setFixedWidth(int width)
{
  width -= cmdCat->width();
  cmbCat->setFixedWidth(width);
}
