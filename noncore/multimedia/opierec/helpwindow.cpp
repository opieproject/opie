/****************************************************************************
** $Id: helpwindow.cpp,v 1.3 2003-08-09 16:51:16 kergoth Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "helpwindow.h"
#include <qlayout.h>

#include <qpe/qpetoolbar.h>
#include <qpe/resource.h>

#include <qaction.h>
#include <qmenubar.h>

#include <ctype.h>

HelpWindow::HelpWindow( const QString& home_, const QString& _path, QWidget* parent, const char *name )
    : QMainWindow( parent, name, WDestructiveClose ), pathCombo( 0 ), selectedURL()
{
    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing( 2);
    layout->setMargin( 2);
    qDebug(_path);
    browser = new QTextBrowser( this );
    QStringList Strlist;
    Strlist.append( home_);
    browser->mimeSourceFactory()->setFilePath( Strlist );
    browser->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    connect( browser, SIGNAL( textChanged() ), this, SLOT( textChanged() ) );

    if ( !home_.isEmpty() )
        browser->setSource( home_ );
    QPEToolBar *toolbar = new QPEToolBar( this );

    QAction *a = new QAction( tr( "Backward" ), Resource::loadPixmap( "back" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), browser, SLOT( backward() ) );
    a->addTo( toolbar );

    a = new QAction( tr( "Forward" ), Resource::loadPixmap( "forward" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), browser, SLOT( forward() ) );
    a->addTo( toolbar );


    layout->addMultiCellWidget( toolbar, 0, 0, 0, 0);

    layout->addMultiCellWidget( browser, 1, 2, 0, 2);
    
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
  setCaption( "QpeRec - Helpviewer - " + browser->context() );
  selectedURL = browser->context();
    }
    else {
  setCaption( "QpeRec - Helpviewer - " + browser->documentTitle() ) ;
  selectedURL = browser->documentTitle();
    }

//     if ( !selectedURL.isEmpty() && pathCombo ) {
//   bool exists = FALSE;
//   int i;
//   for ( i = 0; i < pathCombo->count(); ++i ) {
//       if ( pathCombo->text( i ) == selectedURL ) {
//     exists = TRUE;
//     break;
//       }
//   }
//   if ( !exists ) {
//       pathCombo->insertItem( selectedURL, 0 );
//       pathCombo->setCurrentItem( 0 );
//       mHistory[ hist->insertItem( selectedURL ) ] = selectedURL;
//   } else
//       pathCombo->setCurrentItem( i );
//   selectedURL = QString::null;
//     }
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
#endif
}

void HelpWindow::newWindow()
{
    ( new HelpWindow(browser->source(), "qbrowser") )->show();
}

void HelpWindow::print()
{
#ifndef QT_NO_PRINTER
#endif
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

