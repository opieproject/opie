/****************************************************************************
** $Id: helpwindow.cpp,v 1.1 2002-11-01 16:43:34 llornkcor Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "helpwindow.h"
#include <qstatusbar.h>
#include <qstringlist.h>

#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qiconset.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstylesheet.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qapplication.h>
#include <qcombobox.h>
#include <qevent.h>
#include <qlineedit.h>
#include <qobjectlist.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qprinter.h>
#include <qsimplerichtext.h>
#include <qpaintdevicemetrics.h>

#include <ctype.h>

HelpWindow::HelpWindow( const QString& home_, const QString& _path, QWidget* parent, const char *name )
    : QMainWindow( parent, name, WDestructiveClose ), pathCombo( 0 ), selectedURL()
{
    readHistory();
    readBookmarks();

    browser = new QTextBrowser( this );
    QStringList Strlist;
    Strlist.append( home_);
    
    browser->mimeSourceFactory()->setFilePath( Strlist );

    browser->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    connect( browser, SIGNAL( textChanged() ),

             this, SLOT( textChanged() ) );

    setCentralWidget( browser );

    if ( !home_.isEmpty() )

////////////////////////////////
        browser->setSource( home_ );

////////////////////////////////
    connect( browser, SIGNAL( highlighted( const QString&) ),
       statusBar(), SLOT( message( const QString&)) );

    setGeometry( 0,0,236,280);

    QPopupMenu* file = new QPopupMenu( this );
//    file->insertItem( tr("&New Window"), this, SLOT( newWindow() ), ALT | Key_N );
    file->insertItem( tr("&Open File"), this, SLOT( openFile() ), ALT | Key_O );
//    file->insertItem( tr("&Print"), this, SLOT( print() ), ALT | Key_P );
    file->insertSeparator();
    file->insertItem( tr("&Close"), this, SLOT( close() ), ALT | Key_Q );
//    file->insertItem( tr("E&xit"), qApp, SLOT( closeAllWindows() ), ALT | Key_X );

    // The same three icons are used twice each.
////F FIXME
    QString pixs=(QDir::homeDirPath ()) +"/Applications/gutenbrowser/pix/";
    QIconSet icon_back( QPixmap(pixs+"back.png") );
    QIconSet icon_forward( QPixmap(pixs+"forward.png") );
    QIconSet icon_home( QPixmap(pixs+"home.png") );

    QPopupMenu* go = new QPopupMenu( this );
    backwardId = go->insertItem( icon_back, tr("&Backward"), browser, SLOT( backward() ), ALT | Key_Left );
    forwardId = go->insertItem( icon_forward, tr("&Forward"), browser, SLOT( forward() ), ALT | Key_Right );
    go->insertItem( icon_home, tr("&Home"), browser, SLOT( home() ) );


    hist = new QPopupMenu( this );
    QStringList::Iterator it = history.begin();
    for ( ; it != history.end(); ++it )
    mHistory[ hist->insertItem( *it ) ] = *it;
    connect( hist, SIGNAL( activated( int ) ), this, SLOT( histChosen( int ) ) );

    bookm = new QPopupMenu( this );
    bookm->insertItem( tr( "Add Bookmark" ), this, SLOT( addBookmark() ) );
    bookm->insertSeparator();

    QStringList::Iterator it2 = bookmarks.begin();
    for ( ; it2 != bookmarks.end(); ++it2 )
   mBookmarks[ bookm->insertItem( *it2 ) ] = *it2;
    connect( bookm, SIGNAL( activated( int ) ),
       this, SLOT( bookmChosen( int ) ) );

    menuBar()->insertItem( tr("&File"), file );
    menuBar()->insertItem( tr("&Go"), go );
    menuBar()->insertItem( tr( "History" ), hist );
    menuBar()->insertItem( tr( "Bookmarks" ), bookm );
//      menuBar()->insertSeparator();
//      menuBar()->insertItem( tr("&Help"), help );

    menuBar()->setItemEnabled( forwardId, FALSE);
    menuBar()->setItemEnabled( backwardId, FALSE);
    connect( browser, SIGNAL( backwardAvailable( bool ) ), this, SLOT( setBackwardAvailable( bool ) ) );
    connect( browser, SIGNAL( forwardAvailable( bool ) ), this, SLOT( setForwardAvailable( bool ) ) );


    QToolBar* toolbar = new QToolBar( this );
    addToolBar( toolbar, "Toolbar");
    QToolButton* button;

    button = new QToolButton( icon_back, tr("Backward"), "", browser, SLOT(backward()), toolbar );
    connect( browser, SIGNAL( backwardAvailable(bool) ), button, SLOT( setEnabled(bool) ) );
    button->setEnabled( FALSE );
    button = new QToolButton( icon_forward, tr("Forward"), "", browser, SLOT(forward()), toolbar );
    connect( browser, SIGNAL( forwardAvailable(bool) ), button, SLOT( setEnabled(bool) ) );
    button->setEnabled( FALSE );
    button = new QToolButton( icon_home, tr("Home"), "", browser, SLOT(home()), toolbar );

    toolbar->addSeparator();

    pathCombo = new QComboBox( TRUE, toolbar );
    connect( pathCombo, SIGNAL( activated( const QString & ) ), this, SLOT( pathSelected( const QString & ) ) );
    toolbar->setStretchableWidget( pathCombo );

//    pathCombo->setMaximumWidth(190);
//     setRightJustification( TRUE );
//      setDockEnabled( Left, FALSE );
//      setDockEnabled( Right, FALSE );

    pathCombo->insertItem( home_ );

    browser->setFocus();


}


void HelpWindow::setBackwardAvailable( bool b)
{
    menuBar()->setItemEnabled( backwardId, b);
}

void HelpWindow::setForwardAvailable( bool b)
{
    menuBar()->setItemEnabled( forwardId, b);
}


void HelpWindow::textChanged()
{
    if ( browser->documentTitle().isNull() ) {
  setCaption( "Stockticker Lookup - " + browser->context() );
  selectedURL = browser->context();
    }
    else {
  setCaption( "Stockticker Lookup - " + browser->documentTitle() ) ;
  selectedURL = browser->documentTitle();
    }

    if ( !selectedURL.isEmpty() && pathCombo ) {
  bool exists = FALSE;
  int i;
  for ( i = 0; i < pathCombo->count(); ++i ) {
      if ( pathCombo->text( i ) == selectedURL ) {
    exists = TRUE;
    break;
      }
  }
  if ( !exists ) {
      pathCombo->insertItem( selectedURL, 0 );
      pathCombo->setCurrentItem( 0 );
      mHistory[ hist->insertItem( selectedURL ) ] = selectedURL;
  } else
      pathCombo->setCurrentItem( i );
  selectedURL = QString::null;
    }
}

HelpWindow::~HelpWindow()
{
    history.clear();
    QMap<int, QString>::Iterator it = mHistory.begin();
    for ( ; it != mHistory.end(); ++it )
  history.append( *it );

    QFile f( QDir::currentDirPath() + "/.history" );
    f.open( IO_WriteOnly );
    QDataStream s( &f );
    s << history;
    f.close();

    bookmarks.clear();
    QMap<int, QString>::Iterator it2 = mBookmarks.begin();
    for ( ; it2 != mBookmarks.end(); ++it2 )
  bookmarks.append( *it2 );

    QFile f2( QDir::currentDirPath() + "/.bookmarks" );
    f2.open( IO_WriteOnly );
    QDataStream s2( &f2 );
    s2 << bookmarks;
    f2.close();
}

void HelpWindow::openFile()
{
#ifndef QT_NO_FILEDIALOG
    QString fn = QFileDialog::getOpenFileName( QString::null, QString::null, this );
    if ( !fn.isEmpty() )
  browser->setSource( fn );
#endif
}

void HelpWindow::newWindow()
{
    ( new HelpWindow(browser->source(), "qbrowser") )->show();
}

void HelpWindow::pathSelected( const QString &_path )
{
    browser->setSource( _path );
    QMap<int, QString>::Iterator it = mHistory.begin();
    bool exists = FALSE;
    for ( ; it != mHistory.end(); ++it ) {
  if ( *it == _path ) {
      exists = TRUE;
      break;
  }
    }
    if ( !exists )
  mHistory[ hist->insertItem( _path ) ] = _path;
}

void HelpWindow::readHistory()
{
    if ( QFile::exists( QDir::currentDirPath() + "/.history" ) ) {
  QFile f( QDir::currentDirPath() + "/.history" );
  f.open( IO_ReadOnly );
  QDataStream s( &f );
  s >> history;
  f.close();
  while ( history.count() > 20 )
      history.remove( history.begin() );
    }
}

void HelpWindow::readBookmarks()
{
    if ( QFile::exists( QDir::currentDirPath() + "/.bookmarks" ) ) {
  QFile f( QDir::currentDirPath() + "/.bookmarks" );
  f.open( IO_ReadOnly );
  QDataStream s( &f );
  s >> bookmarks;
  f.close();
    }
}

void HelpWindow::histChosen( int i )
{
    if ( mHistory.contains( i ) )
  browser->setSource( mHistory[ i ] );
}

void HelpWindow::bookmChosen( int i )
{
    if ( mBookmarks.contains( i ) )
  browser->setSource( mBookmarks[ i ] );
}

void HelpWindow::addBookmark()
{
    mBookmarks[ bookm->insertItem( caption() ) ] = caption();
}

