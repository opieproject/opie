/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// (c) 2002 Patrick S. Vogt <tille@handhelds.org>


#include "mainwindow.h"

#include <qpe/qpemenubar.h>
#include <qpe/qpemessagebox.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/qpetoolbar.h>
#include <qpe/qpeapplication.h>
#include <qaction.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qtoolbutton.h>
#include <qstring.h>
#include <qlabel.h>
#include <qfile.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qtextbrowser.h>

#include "olistview.h"
#include "olistviewitem.h"
#include "resultitem.h"
#include "adresssearch.h"
#include "todosearch.h"
#include "datebooksearch.h"
#include "applnksearch.h"

MainWindow::MainWindow( QWidget *parent, const char *name, WFlags f ) :
  QMainWindow( parent, name, f ), _currentItem(0)
{
  setCaption( tr("OSearch") );

  setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

  QFrame *mainFrame = new QFrame( this, "mainFrame" );

  mainLayout = new QVBoxLayout( mainFrame );
  mainLayout->setSpacing( 0 );
  mainLayout->setMargin( 0 );

  resultsList = new OListView( mainFrame );
  resultsList->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
  mainLayout->addWidget( resultsList );

  detailsFrame = new QFrame( mainFrame, "detailsFrame" );
  QVBoxLayout *detailsLayout = new QVBoxLayout( detailsFrame );
  richEdit = new QTextView( detailsFrame );
  richEdit->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
  detailsLayout->addWidget( richEdit, 0 );
  QHBoxLayout *buttonLayout = new QHBoxLayout( detailsFrame );
  detailsLayout->addLayout( buttonLayout, 0 );
   QPushButton *buttonShow = new QPushButton( detailsFrame, "Show" );
   buttonShow->setText( tr("show") );
//   buttonShow->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
   QPushButton *buttonEdit = new QPushButton( detailsFrame, "Edit" );
   buttonEdit->setText( tr("edit") );
 //  buttonEdit->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
   buttonLayout->addWidget( buttonShow, 0 );
   buttonLayout->addWidget( buttonEdit, 0 );

  mainLayout->addWidget( detailsFrame, 0 );
  detailsFrame->hide();

  adrSearch = new AdressSearch( resultsList, tr("adressbook") );
  todoSearch = new TodoSearch( resultsList, tr("todo") );
  datebookSearch = new DatebookSearch( resultsList, tr("datebook") );
  applnkSearch = new AppLnkSearch( resultsList, tr("applications") );

  makeMenu();
  setCentralWidget( mainFrame );

  popupTimer = new QTimer();

  connect(popupTimer, SIGNAL(timeout()), SLOT(showPopup()));
  connect(resultsList, SIGNAL(pressed(QListViewItem*)), SLOT(setCurrent(QListViewItem*)));
  connect(resultsList, SIGNAL(clicked(QListViewItem*)), SLOT(stopTimer(QListViewItem*)));
  connect(buttonShow, SIGNAL(clicked()), SLOT( showItem() ) );
  connect(buttonEdit, SIGNAL(clicked()), SLOT( editItem() ) );


}

void MainWindow::makeMenu()
{
//   QPEToolBar *toolBar = new QPEToolBar( this );
//   QPEMenuBar *menuBar = new QPEMenuBar( toolBar );
//   QPopupMenu *searchMenu = new QPopupMenu( menuBar );
//   QPopupMenu *viewMenu = new QPopupMenu( menuBar );
//   QPopupMenu *cfgMenu = new QPopupMenu( menuBar );
//
   setToolBarsMovable( false );
//   toolBar->setHorizontalStretchable( true );
//   menuBar->insertItem( tr( "Search" ), searchMenu );
//   menuBar->insertItem( tr( "View" ), viewMenu );
//   menuBar->insertItem( tr( "Settings" ), cfgMenu );

  //SEARCH
  QPEToolBar *searchBar = new QPEToolBar(this);
  addToolBar( searchBar,  "Search", QMainWindow::Top, TRUE );
  QLabel *label = new QLabel( tr("Search: "), searchBar );
//  label->setBackgroundMode( PaletteForeground );
  searchBar->setHorizontalStretchable( TRUE );
  QLineEdit *searchEdit = new QLineEdit( searchBar, "seachEdit" );
  searchBar->setStretchableWidget( searchEdit );
  connect( searchEdit, SIGNAL( textChanged( const QString & ) ),
       this, SLOT( setSearch( const QString & ) ) );


}

MainWindow::~MainWindow()
{
}

void MainWindow::setCurrent(QListViewItem *item)
{
	if (!item) return;
	_currentItem = (OListViewItem*)item;
//	_currentItem = dynamic_cast<OListViewItem*>(item);
	if (_currentItem->rtti() == OListViewItem::Result){
		ResultItem *res = (ResultItem*)item;
//		ResultItem *res = dynamic_cast<ResultItem*>(item);
		richEdit->setText( res->toRichText() );
		detailsFrame->show();
	}else detailsFrame->hide();
	//_currentItem = (OListViewItem*)item;
	popupTimer->start( 300 );
}

void MainWindow::stopTimer(QListViewItem*)
{
        popupTimer->stop();
}

void MainWindow::showPopup()
{
	qDebug("showPopup");
        if (!_item) return;
}

void MainWindow::setSearch( const QString &key )
{
  adrSearch->setSearch(key);
  todoSearch->setSearch(key);
  datebookSearch->setSearch(key);
  applnkSearch->setSearch(key);
}


void MainWindow::showItem()
{
	if (_currentItem->rtti() == OListViewItem::Result){
		ResultItem *res = (ResultItem*)_currentItem;
//		ResultItem *res = dynamic_cast<ResultItem*>(item);
		res->showItem();
	}
}

void MainWindow::editItem()
{
	if (_currentItem->rtti() == OListViewItem::Result){
		ResultItem *res = (ResultItem*)_currentItem;
//		ResultItem *res = dynamic_cast<ResultItem*>(item);
		res->editItem();
	}
}
