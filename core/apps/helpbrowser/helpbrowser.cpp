#/**********************************************************************
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

#include "helpbrowser.h"

#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/global.h>

#include <qstatusbar.h>
#include <qdragobject.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qpe/qpemenubar.h>
#include <qpe/qpetoolbar.h>
#include <qtoolbutton.h>
#include <qiconset.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstylesheet.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qevent.h>
#include <qlineedit.h>
#include <qobjectlist.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qprinter.h>
#include <qsimplerichtext.h>
#include <qpaintdevicemetrics.h>
#include <qaction.h>

#include <ctype.h>


HelpBrowser::HelpBrowser( QWidget* parent, const char *name, WFlags f )
    : QMainWindow( parent, name, f ),
      selectedURL()
{
    init( "index.html" );
}

void HelpBrowser::init( const QString& _home )
{
    setIcon( Resource::loadPixmap( "help_icon" ) );

    browser = new QTextBrowser( this );
    browser->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    connect( browser, SIGNAL( textChanged() ),
	     this, SLOT( textChanged() ) );

    setCentralWidget( browser );
    setToolBarsMovable( FALSE );

    if ( !_home.isEmpty() )
	browser->setSource( _home );

    QPEToolBar* toolbar = new QPEToolBar( this );
    toolbar->setHorizontalStretchable( TRUE );
    QPEMenuBar *menu = new QPEMenuBar( toolbar );

    toolbar = new QPEToolBar( this );
    // addToolBar( toolbar, "Toolbar");

    //QPopupMenu* go = new QPopupMenu( this );
    backAction = new QAction( tr( "Backward" ), Resource::loadIconSet( "back" ), QString::null, 0, this, 0 );
    connect( backAction, SIGNAL( activated() ), browser, SLOT( backward() ) );
    connect( browser, SIGNAL( backwardAvailable( bool ) ),
	     backAction, SLOT( setEnabled( bool ) ) );
    //backAction->addTo( go );
    backAction->addTo( toolbar );
    backAction->setEnabled( FALSE );

    forwardAction = new QAction( tr( "Forward" ), Resource::loadIconSet( "forward" ), QString::null, 0, this, 0 );
    connect( forwardAction, SIGNAL( activated() ), browser, SLOT( forward() ) );
    connect( browser, SIGNAL( forwardAvailable( bool ) ),
	     forwardAction, SLOT( setEnabled( bool ) ) );
    //forwardAction->addTo( go );
    forwardAction->addTo( toolbar );
    forwardAction->setEnabled( FALSE );

    QAction *a = new QAction( tr( "Home" ), Resource::loadPixmap(
"helpbrowser/home" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), browser, SLOT( home() ) );
    //a->addTo( go );
    a->addTo( toolbar );

    bookm = new QPopupMenu( this );
    bookm->insertItem( tr( "Add Bookmark" ), this, SLOT( addBookmark() ) );
    bookm->insertItem( tr( "Remove from Bookmarks" ), this, SLOT( removeBookmark() ) );
    bookm->insertSeparator();
    connect( bookm, SIGNAL( activated( int ) ),
	     this, SLOT( bookmChosen( int ) ) );

    readBookmarks();

    //menu->insertItem( tr("Go"), go );
    menu->insertItem( tr( "Bookmarks" ), bookm );

    resize( 240, 300 );
    browser->setFocus();

    connect( qApp, SIGNAL(appMessage(const QCString&, const QByteArray&)),
	     this, SLOT(appMessage(const QCString&, const QByteArray&)) );
}

void HelpBrowser::appMessage(const QCString& msg, const QByteArray& data)
{
    if ( msg == "showFile(QString)" ) {
	QDataStream ds(data,IO_ReadOnly);
	QString fn;
	ds >> fn;
	setDocument( fn );
    }
}

void HelpBrowser::setDocument( const QString &doc )
{
    if ( !doc.isEmpty() )
	browser->setSource( doc );
    raise();    
}


void HelpBrowser::textChanged()
{
    if ( browser->documentTitle().isNull() )
	setCaption( tr("Help Browser") );
    else
	setCaption( browser->documentTitle() ) ;

    selectedURL = caption();
}

HelpBrowser::~HelpBrowser()
{
    QStringList bookmarks;
    QMap<int, Bookmark>::Iterator it2 = mBookmarks.begin();
    for ( ; it2 != mBookmarks.end(); ++it2 )
	bookmarks.append( (*it2).name + "=" + (*it2).file );

    QFile f2( Global::applicationFileName("helpbrowser", "bookmarks") );
    if ( f2.open( IO_WriteOnly ) ) {
	QDataStream s2( &f2 );
	s2 << bookmarks;
	f2.close();
    }
}

void HelpBrowser::pathSelected( const QString &_path )
{
    browser->setSource( _path );
}

void HelpBrowser::readBookmarks()
{
    QString file = Global::applicationFileName("helpbrowser", "bookmarks");
    if ( QFile::exists( file ) ) {
	QStringList bookmarks;
	QFile f( file );
	if ( f.open( IO_ReadOnly ) ) {
	    QDataStream s( &f );
	    s >> bookmarks;
	    f.close();
	}
	QStringList::Iterator it = bookmarks.begin();
	for ( ; it != bookmarks.end(); ++it ) {
	    Bookmark b;
	    QString current = *it;
	    int equal = current.find( "=" );
	    if ( equal < 1 || equal == (int)current.length() - 1 )
		continue;
	    b.name = current.left( equal );
	    b.file = current.mid( equal + 1 );
	    mBookmarks[ bookm->insertItem( b.name ) ] = b;
	} 
    }
}

void HelpBrowser::bookmChosen( int i )
{
    if ( mBookmarks.contains( i ) )
	browser->setSource( mBookmarks[ i ].file );
}

void HelpBrowser::addBookmark()
{
    Bookmark b;
    b.name = browser->documentTitle();
    b.file = browser->source();
    if (b.name.isEmpty() ) {
	b.name = b.file.left( b.file.length() - 5 ); // remove .html
    }
    QMap<int, Bookmark>::Iterator it;
    for( it = mBookmarks.begin(); it != mBookmarks.end(); ++it )
	if ( (*it).file == b.file ) return;
    mBookmarks[ bookm->insertItem( b.name ) ] = b;
}

void HelpBrowser::removeBookmark()
{
    QString file = browser->source();
    QMap<int, Bookmark>::Iterator it = mBookmarks.begin();
    for( ; it != mBookmarks.end(); ++it )
	if ( (*it).file == file ) {
	    bookm->removeItem( it.key() );
	    mBookmarks.remove( it );
		break;
	}
}
