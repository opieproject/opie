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
#ifdef QWS
#include "qcopchannel_qws.h"
#endif
#include "lnkproperties.h"
#include "applnk.h"
#include "qpeapplication.h"
#include "categorymenu.h"
#include "categoryselect.h"
#include "mimetype.h"
#include "categories.h"

#include <stdlib.h>

#include <qdir.h>
#include <qwidget.h>
#include <qpopupmenu.h>
#include <qtoolbutton.h>
#include <qpushbutton.h>
#include <qheader.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

class TypeCombo : public QComboBox
{
    Q_OBJECT
public:
    TypeCombo( QWidget *parent, const char *name=0 )
	: QComboBox( parent, name )
    {
	connect( this, SIGNAL(activated(int)), this, SLOT(selectType(int)) );
    }

    void reread( DocLnkSet &files, const QString &filter );

signals:
    void selected( const QString & );

protected slots:
    void selectType( int idx ) {
	emit selected( typelist[idx] );
    }

protected:
    QStringList typelist;
    QString prev;
};

void TypeCombo::reread( DocLnkSet &files, const QString &filter )
{
    typelist.clear();
    QStringList filters = QStringList::split( ';', filter );
    int pos = filter.find( '/' );
    //### do for each filter
    if ( filters.count() == 1 && pos >= 0 && filter[pos+1] != '*' ) {
	typelist.append( filter );
	clear();
	QString minor = filter.mid( pos+1 );
	minor[0] = minor[0].upper();
	insertItem( tr("%1 files").arg(minor) );
	setCurrentItem(0);
	setEnabled( FALSE );
	return;
    }

    QListIterator<DocLnk> dit( files.children() );
    for ( ; dit.current(); ++dit ) {
	if ( !typelist.contains( (*dit)->type() ) )
	    typelist.append(  (*dit)->type() );
    }

    QStringList types;
    QStringList::ConstIterator it;
    for (it = typelist.begin(); it!=typelist.end(); ++it) {
	QString t = *it;
	if ( t.left(12) == "application/" ) {
	    MimeType mt(t);
	    const AppLnk* app = mt.application();
	    if ( app )
		t = app->name();
	    else
		t = t.mid(12);
	} else {
	    QString major, minor;
	    int pos = t.find( '/' );
	    if ( pos >= 0 ) {
		major = t.left( pos );
		minor = t.mid( pos+1 );
	    }
	    if ( minor.find( "x-" ) == 0 )
		minor = minor.mid( 2 );
	    minor[0] = minor[0].upper();
	    major[0] = major[0].upper();
	    if ( filters.count() > 1 )
		t = tr("%1 %2", "minor mimetype / major mimetype").arg(minor).arg(major);
	    else
		t = minor;
	}
	types += tr("%1 files").arg(t);
    }
    for (it = filters.begin(); it!=filters.end(); ++it) {
	typelist.append( *it );
	int pos = (*it).find( '/' );
	if ( pos >= 0 ) {
	    QString maj = (*it).left( pos );
	    maj[0] = maj[0].upper();
	    types << tr("All %1 files").arg(maj);
	}
    }
    if ( filters.count() > 1 ) {
	typelist.append( filter );
	types << tr("All files");
    }
    prev = currentText();
    clear();
    insertStringList(types);
    for (int i=0; i<count(); i++) {
	if ( text(i) == prev ) {
	    setCurrentItem(i);
	    break;
	}
    }
    if ( prev.isNull() )
	setCurrentItem(count()-1);
    setEnabled( TRUE );
}


//===========================================================================

FileSelectorItem::FileSelectorItem( QListView *parent, const DocLnk &f )
    : QListViewItem( parent ), fl( f )
{
    setText( 0, f.name() );
    setPixmap( 0, f.pixmap() );
}

FileSelectorItem::~FileSelectorItem()
{
}

FileSelectorView::FileSelectorView( QWidget *parent, const char *name )
    : QListView( parent, name )
{
    setAllColumnsShowFocus( TRUE );
    addColumn( tr( "Name" ) );
    header()->hide();
}

FileSelectorView::~FileSelectorView()
{
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

class NewDocItem : public FileSelectorItem
{
public:
    NewDocItem( QListView *parent, const DocLnk &f )
	: FileSelectorItem( parent, f ) {
	setText( 0, QObject::tr("New Document") );
	QImage img( Resource::loadImage( "new" ) );
	QPixmap pm;
	pm = img.smoothScale( AppLnk::smallIconSize(), AppLnk::smallIconSize() );
	setPixmap( 0, pm );
    }
    QString key ( int, bool ) const {
	return QString("\n");
    }

    void paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int alignment ) {
	QFont oldFont = p->font();
	QFont newFont = p->font();
	newFont.setWeight( QFont::Bold );
	p->setFont( newFont );
	FileSelectorItem::paintCell( p, cg, column, width, alignment );
	p->setFont( oldFont );
    }

    int width( const QFontMetrics &fm, const QListView *v, int c ) const {
	return FileSelectorItem::width( fm, v, c )*4/3; // allow for bold font
    }
};

//===========================================================================

class FileSelectorPrivate
{
public:
    TypeCombo *typeCombo;
    CategorySelect *catSelect;
    QValueList<QRegExp> mimeFilters;
    int catId;
    bool showNew;
    NewDocItem *newDocItem;
    DocLnkSet files;
    QHBox *toolbar;
};

/*!
  \class FileSelector fileselector.h
  \brief The FileSelector widget allows the user to select DocLnk objects.

  This class presents a file selection dialog to the user. This widget
  is usually the first widget seen in a \link docwidget.html
  document-oriented application\endlink. The developer will most often
  create this widget in combination with a <a
  href="../qt/qwidgetstack.html"> QWidgetStack</a> and the appropriate
  editor and/or viewer widget for their application. This widget
  should be shown first and the user can the select which document
  they wish to operate on. Please refer to the implementation of
  texteditor for an example of how to tie these classes together.

  Use setNewVisible() depending on whether the application can be used
  to create new files or not. Use setCloseVisible() depending on
  whether the user may leave the dialog without creating or selecting
  a document or not. The number of files in the view is available from
  fileCount(). To force the view to be updated call reread().

  If the user presses the 'New Document' button the newSelected()
  signal is emitted. If the user selects an existing file the
  fileSelected() signal is emitted. The selected file's \link
  doclnk.html DocLnk\endlink is available from the selected()
  function. If the file selector is no longer necessary the closeMe()
  signal is emitted.

  \ingroup qtopiaemb
  \sa FileManager
*/

/*!
  Constructs a FileSelector with mime filter \a f.
  The standard Qt \a parent and \a name parameters are passed to the
  parent widget.

  If \a newVisible is TRUE, the widget has a button to allow the user
  the create "new" documents; this is useful for applications that can
  create and edit documents but not suitable for applications that
  only provide viewing.

  \a closeVisible is deprecated

  \sa DocLnkSet::DocLnkSet()
*/
FileSelector::FileSelector( const QString &f, QWidget *parent, const char *name, bool newVisible, bool closeVisible )
    : QVBox( parent, name ), filter( f )
{
    setMargin( 0 );
    setSpacing( 0 );

    d = new FileSelectorPrivate();
    d->newDocItem = 0;
    d->showNew = newVisible;
    d->catId = -2; // All files

    d->toolbar = new QHBox( this );
    d->toolbar->setBackgroundMode( PaletteButton );   // same colour as toolbars
    d->toolbar->setSpacing( 0 );
    d->toolbar->hide();

    QWidget *spacer = new QWidget( d->toolbar );
    spacer->setBackgroundMode( PaletteButton );

    QToolButton *tb = new QToolButton( d->toolbar );
    tb->setPixmap( Resource::loadPixmap( "close" ) );
    connect( tb, SIGNAL( clicked() ), this, SIGNAL( closeMe() ) );
    buttonClose = tb;
    tb->setFixedSize( 18, 20 ); // tb->sizeHint() );
    tb->setAutoRaise( TRUE );
    QToolTip::add( tb, tr( "Close the File Selector" ) );
    QPEMenuToolFocusManager::manager()->addWidget( tb );

    view = new FileSelectorView( this, "fileview" );
    QPEApplication::setStylusOperation( view->viewport(), QPEApplication::RightOnHold );
    connect( view, SIGNAL( mouseButtonClicked( int, QListViewItem *, const QPoint &, int ) ),
	     this, SLOT( fileClicked( int, QListViewItem *, const QPoint &, int ) ) );
    connect( view, SIGNAL( mouseButtonPressed( int, QListViewItem *, const QPoint &, int ) ),
	     this, SLOT( filePressed( int, QListViewItem *, const QPoint &, int ) ) );
    connect( view, SIGNAL( returnPressed( QListViewItem * ) ),
	     this, SLOT( fileClicked( QListViewItem * ) ) );

    QHBox *hb = new QHBox( this );

    d->typeCombo = new TypeCombo( hb );
    connect( d->typeCombo, SIGNAL(selected(const QString&)),
	    this, SLOT(typeSelected(const QString&)) );
    QWhatsThis::add( d->typeCombo, tr("Show documents of this type") );

    Categories c;
    c.load(categoryFileName());
    QArray<int> vl( 0 );
    d->catSelect = new CategorySelect( hb );
    d->catSelect->setRemoveCategoryEdit( TRUE );
    d->catSelect->setCategories( vl, "Document View", tr("Document View") );
    d->catSelect->setAllCategories( TRUE );
    connect( d->catSelect, SIGNAL(signalSelected(int)), this, SLOT(catSelected(int)) );
    QWhatsThis::add( d->catSelect, tr("Show documents in this category") );

    setCloseVisible( closeVisible );

    QCopChannel *channel = new QCopChannel( "QPE/Card", this );
    connect( channel, SIGNAL(received(const QCString &, const QByteArray &)),
	    this, SLOT(cardMessage( const QCString &, const QByteArray &)) );

    reread();
    updateWhatsThis();
}

/*!
  Destroys the widget.
*/
FileSelector::~FileSelector()
{
    delete d;
}

/*!
  Returns the number of files in the view. If this is zero, an editor
  application might bypass the selector and immediately start with
  a "new" document.
*/
int FileSelector::fileCount()
{
    return d->files.children().count();;
}

/*!
  Calling this function is the programmatic equivalent of the user
  pressing the "new" button.

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
    if ( button == Qt::LeftButton ) {
	fileClicked( i );
    }
}

void FileSelector::filePressed( int button, QListViewItem *i, const QPoint &, int )
{
    if ( !i || i == d->newDocItem )
	return;
    if ( button == Qt::RightButton ) {
	DocLnk l = ((FileSelectorItem *)i)->file();
	LnkProperties prop( &l );
	prop.showMaximized();
	prop.exec();
	reread();
    }
}

void FileSelector::fileClicked( QListViewItem *i )
{
    if ( !i )
	return;
    if ( i == d->newDocItem ) {
	createNew();
    } else {
	emit fileSelected( ( (FileSelectorItem*)i )->file() );
	emit closeMe();
    }
}

void FileSelector::typeSelected( const QString &type )
{
    d->mimeFilters.clear();
    QStringList subFilter = QStringList::split(";", type);
    for( QStringList::Iterator it = subFilter.begin(); it != subFilter.end(); ++it )
	d->mimeFilters.append( QRegExp(*it, FALSE, TRUE) );
    updateView();
}

void FileSelector::catSelected( int c )
{
    d->catId = c;
    updateView();
}

void FileSelector::cardMessage( const QCString &msg, const QByteArray &)
{
    if ( msg == "mtabChanged()" )
	reread();
}


/*!
  Returns the selected \link doclnk.html DocLnk\endlink. The caller is
  responsible for deleting the returned value.
*/
const DocLnk *FileSelector::selected()
{
    FileSelectorItem *item = (FileSelectorItem *)view->selectedItem();
    if ( item && item != d->newDocItem )
	return new DocLnk( item->file() );
    return NULL;
}

/*!
  \fn void FileSelector::fileSelected( const DocLnk &f )

  This signal is emitted when the user selects a document.
  \a f is the document.
*/

/*!
  \fn void FileSelector::newSelected( const DocLnk &f )

  This signal is emitted when the user selects a "new" document.
  \a f is a DocLnk for the document. You will need to set the type
  of the document after copying it.
*/

/*!
  \fn void FileSelector::closeMe()

  This signal is emitted when the user no longer needs to view the widget.
*/


/*!
  If \a b is TRUE a "new document" entry is visible; if \a b is FALSE
  this entry is not visible and the user is unable to create new
  documents from the dialog.
*/
void FileSelector::setNewVisible( bool b )
{
    if ( d->showNew != b ) {
	d->showNew = b;
	updateView();
	updateWhatsThis();
    }
}

/*!
  If \a b is TRUE a "close" or "no document" button is visible; if \a
  b is FALSE this button is not visible and the user is unable to
  leave the dialog without creating or selecting a document.

  This function is deprecated.
*/
void FileSelector::setCloseVisible( bool b )
{
    if (  b )
	d->toolbar->show();
    else
	d->toolbar->hide();
}

/*!

*/
void FileSelector::setTypeComboVisible( bool b ) {
    if ( b )
        d->typeCombo->show();
    else
        d->typeCombo->hide();
}
/*!

*/
void FileSelector::setCategorySelectVisible( bool b ) {
    if ( b )
        d->catSelect->show();
    else
        d->catSelect->hide();
}

/*!
  Rereads the list of documents.
*/
void FileSelector::reread()
{
    d->files.clear();
    Global::findDocuments(&d->files, filter);
    d->typeCombo->reread( d->files, filter );
    updateView();
}

void FileSelector::updateView()
{
    FileSelectorItem *item = (FileSelectorItem *)view->selectedItem();
    if ( item == d->newDocItem )
	item = 0;
    QString oldFile;
    if ( item )
	oldFile = item->file().file();
    view->clear();
    QListIterator<DocLnk> dit( d->files.children() );
    for ( ; dit.current(); ++dit ) {
	bool mimeMatch = FALSE;
	if ( d->mimeFilters.count() ) {
	    QValueList<QRegExp>::Iterator it;
	    for ( it = d->mimeFilters.begin(); it != d->mimeFilters.end(); ++it ) {
		if ( (*it).match((*dit)->type()) >= 0 ) {
		    mimeMatch = TRUE;
		    break;
		}
	    }
	} else {
	    mimeMatch = TRUE;
	}
	if ( mimeMatch &&
		(d->catId == -2 || (*dit)->categories().contains(d->catId) ||
		 (d->catId == -1 && (*dit)->categories().isEmpty())) ) {
	    item = new FileSelectorItem( view, **dit );
	    if ( item->file().file() == oldFile )
		view->setCurrentItem( item );
	}
    }

    if ( d->showNew )
	d->newDocItem = new NewDocItem( view, DocLnk() );
    else
	d->newDocItem = 0;

    if ( !view->selectedItem() || view->childCount() == 1 ) {
	view->setCurrentItem( view->firstChild() );
	view->setSelected( view->firstChild(), TRUE );
    }
}

void FileSelector::updateWhatsThis()
{
    QWhatsThis::remove( this );
    QString text = tr("Click to select a document from the list");
    if ( d->showNew )
	text += tr(", or select <b>New Document</b> to create a new document.");
    text += tr("<br><br>Click and hold for document properties.");
    QWhatsThis::add( this, text );
}

#include "fileselector.moc"

