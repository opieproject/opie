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

// WARNING: Do *NOT* define this yourself. The SL5xxx from SHARP does NOT
//      have this class.
#define QTOPIA_INTERNAL_FSLP

#include "fileselector.h"
#include "fileselector_p.h"
#include "global.h"
#include "resource.h"
#include "config.h"
#include "applnk.h"
#include "storage.h"
#include "qpemenubar.h"
#include "qcopchannel_qws.h"
#include "lnkproperties.h"
#include "applnk.h"
#include "qpeapplication.h"
#include "categorymenu.h"

#include <stdlib.h>

#include <qdir.h>
#include <qwidget.h>
#include <qpopupmenu.h>
#include <qtoolbutton.h>
#include <qpushbutton.h>
#include <qheader.h>
#include <qtooltip.h>


FileSelectorItem::FileSelectorItem( QListView *parent, const DocLnk &f )
    : QListViewItem( parent ), fl( f )
{
    setText( 0, f.name() );
    setPixmap( 0, f.pixmap() );
}

FileSelectorItem::~FileSelectorItem()
{
}

class FileSelectorViewPrivate
{
public:
    CategoryMenu *cm;
    bool m_noItems:1;
};

FileSelectorView::FileSelectorView( const QString &f, QWidget *parent, const char *name )
    : QListView( parent, name ), filter( f ), count( 0 )
{
    d = new FileSelectorViewPrivate();
    d->cm = 0;
    d->m_noItems = false;
    setAllColumnsShowFocus( TRUE );
    addColumn( tr( "Name" ) );
    header()->hide();

    fileManager = new FileManager;
    reread();
    QCopChannel *channel = new QCopChannel( "QPE/Card", this );
    connect( channel, SIGNAL(received(const QCString &, const QByteArray &)),
	     this, SLOT(cardMessage( const QCString &, const QByteArray &)) );
}

FileSelectorView::~FileSelectorView()
{
}

void FileSelectorView::reread()
{
    QString oldFile;
    FileSelectorItem *item;
    if( !d->m_noItems ) { // there are items
      item = (FileSelectorItem *)selectedItem();
      if ( item )
	  oldFile = item->file().file();
    }
    clear();
    DocLnkSet files;
    Global::findDocuments(&files, filter);
    count = files.children().count();
    if(count == 0 ){ // No Documents
      d->m_noItems = true;
      QListViewItem *it = new QListViewItem(this, tr("There are no files in this directory." ), "empty" );
      it->setSelectable(FALSE );
      return;
    }
    QListIterator<DocLnk> dit( files.children() );
    for ( ; dit.current(); ++dit ) {
	if (d->cm)
	    if (!d->cm->isSelected((**dit).categories()))
		continue;
        item = new FileSelectorItem( this, **dit );
	if ( item->file().file() == oldFile )
	    setCurrentItem( item );
    }
    if ( !selectedItem() )
	setCurrentItem( firstChild() );
}

void FileSelectorView::setCategoryFilter(CategoryMenu *cm)
{
    d->cm = cm;
    connect(cm, SIGNAL(categoryChange()), this, SLOT(categoryChanged()) );
}

void FileSelectorView::categoryChanged() { reread(); }

void FileSelectorView::cardMessage( const QCString &msg, const QByteArray &)
{
    if ( msg == "mtabChanged()" )
	reread();
}

void FileSelectorView::keyPressEvent( QKeyEvent *e )
{
    QString txt = e->text();
    if (e->key() == Key_Space)
	emit returnPressed( currentItem() );
    else if ( !txt.isNull() && txt[0] > ' ' && e->key() < 0x1000 )
	e->ignore();
    else
	QListView::keyPressEvent(e);
}

class FileSelectorPrivate
{
public:
    CategoryMenu *cm;
    QMenuBar *mb;
};

/*!
  \class FileSelector fileselector.h
  \brief The FileSelector widget allows the user to select DocLnk objects.
*/

/*!
  Constructs a FileSelector with mime filter \a f.
  The standard Qt \a parent and \a name parameters are passed to the
  parent.

  If \a newVisible is TRUE, the widget has an button allowing the user
  the create "new" documents - editor applications will have this while
  viewer applications will not.

  If \a closeVisible is TRUE, the widget has an button allowinf the user
  to select "no document".

  \sa DocLnkSet::DocLnkSet()
*/
FileSelector::FileSelector( const QString &f, QWidget *parent, const char *name, bool newVisible, bool closeVisible )
    : QVBox( parent, name ), filter( f )
{
    setMargin( 0 );
    setSpacing( 0 );
    QHBox *top = new QHBox( this );
    top->setBackgroundMode( PaletteButton );	// same colour as toolbars
    top->setSpacing( 0 );

    QWidget *spacer = new QWidget( top );
    spacer->setBackgroundMode( PaletteButton );

    d = new FileSelectorPrivate();
    d->mb = new QMenuBar(spacer);
    d->cm = new CategoryMenu("Document View", this);
    QPEMenuToolFocusManager::manager()->addWidget( d->mb );
    d->mb->insertItem(tr("View"), d->cm);


    QToolButton *tb = new QToolButton( top );
    tb->setPixmap( Resource::loadPixmap( "new" ) );
    connect( tb, SIGNAL( clicked() ), this, SLOT( createNew() ) );
    buttonNew = tb;
    tb->setFixedSize( 18, 20 ); // tb->sizeHint() );
    tb->setAutoRaise( TRUE );
    QToolTip::add( tb, tr( "Create a new Document" ) );
    QPEMenuToolFocusManager::manager()->addWidget( tb );

    tb = new QToolButton( top );
    tb->setPixmap( Resource::loadPixmap( "close" ) );
    connect( tb, SIGNAL( clicked() ), this, SIGNAL( closeMe() ) );
    buttonClose = tb;
    tb->setFixedSize( 18, 20 ); // tb->sizeHint() );
    tb->setAutoRaise( TRUE );
    QToolTip::add( tb, tr( "Close the File Selector" ) );
    QPEMenuToolFocusManager::manager()->addWidget( tb );

    view = new FileSelectorView( filter, this, "fileview" );
    view->setCategoryFilter(d->cm);
    QPEApplication::setStylusOperation( view->viewport(), QPEApplication::RightOnHold );
    connect( view, SIGNAL( mouseButtonClicked( int, QListViewItem *, const QPoint &, int ) ),
	     this, SLOT( fileClicked( int, QListViewItem *, const QPoint &, int ) ) );
    connect( view, SIGNAL( mouseButtonPressed( int, QListViewItem *, const QPoint &, int ) ),
	     this, SLOT( filePressed( int, QListViewItem *, const QPoint &, int ) ) );
    connect( view, SIGNAL( returnPressed( QListViewItem * ) ),
	     this, SLOT( fileClicked( QListViewItem * ) ) );

    setNewVisible( newVisible );
    setCloseVisible( closeVisible );
}

/*!
  Destroys the widget.
*/
FileSelector::~FileSelector()
{

}

/*!
  Returns the number of files in the view. If this is zero, and editor
  application might avoid using the selector and immediately start with
  a "new" document.
*/
int FileSelector::fileCount()
{
    return view->fileCount();
}

/*!
  Causes the file selector to act as if the "new" button was chosen.

  \sa newSelected(), closeMe()
*/
void FileSelector::createNew()
{
    DocLnk f;
    emit newSelected( f );
    emit closeMe();
}

void FileSelector::fileClicked( int button, QListViewItem *i, const QPoint &, int )
{
    if ( !i )
	return;
    if(i->text(1) == QString::fromLatin1("empty" ) )
      return; 

    if ( button == Qt::LeftButton ) {
	fileClicked( i );
    }
}

void FileSelector::filePressed( int button, QListViewItem *i, const QPoint &, int )
{
    if ( !i )
	return;
    if(i->text(1) == QString::fromLatin1("empty" ) )
      return; 

    if ( button == Qt::RightButton ) {
	DocLnk l = ((FileSelectorItem *)i)->file();
	LnkProperties prop( &l );
	prop.showMaximized();
	prop.exec();
	d->cm->reload();
	reread();
    }
}

void FileSelector::fileClicked( QListViewItem *i )
{
    if ( !i )
	return;
    emit fileSelected( ( (FileSelectorItem*)i )->file() );
    emit closeMe();
}

/*!
  Returns the selected DocLnk. The caller is responsible for deleting
  the returned value.
*/
const DocLnk *FileSelector::selected()
{
    FileSelectorItem *item = (FileSelectorItem *)view->selectedItem();
    if ( item )
	return new DocLnk( item->file() );
    return NULL;
}

/*!
  \fn void FileSelector::fileSelected( const DocLnk &f )

  This signal is emitted when the user selects a file.
  \a f is the file.
*/

/*!
  \fn void FileSelector::newSelected( const DocLnk &f )

  This signal is emitted when the user selects "new" file.
  \a f is a DocLnk for the file. You will need to set the type
  of the value after copying it.
*/

/*!
  \fn void FileSelector::closeMe()

  This signal is emitted when the user no longer needs to view the widget.
*/


/*!
  Sets whether a "new document" button is visible, according to \a b.
*/
void FileSelector::setNewVisible( bool b )
{
    if ( b )
	buttonNew->show();
    else
	buttonNew->hide();
}

/*!
  Sets whether a "no document" button is visible, according to \a b.
*/
void FileSelector::setCloseVisible( bool b )
{
    if ( b )
	buttonClose->show();
    else
	buttonClose->hide();
}

/*!
  Rereads the list of files.
*/
void FileSelector::reread()
{
    view->reread();
}

