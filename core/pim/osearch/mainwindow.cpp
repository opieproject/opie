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
#include <qhbuttongroup.h>
#include <qpushbutton.h>
#include <qintdict.h>
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
#include "doclnksearch.h"

MainWindow::MainWindow( QWidget *parent, const char *name, WFlags f ) :
  QMainWindow( parent, name, f ), _currentItem(0)
{
  setCaption( tr("OSearch") );

  setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding));

  QFrame *mainFrame = new QFrame( this, "mainFrame" );

  mainLayout = new QVBoxLayout( mainFrame );
  mainLayout->setSpacing( 0 );
  mainLayout->setMargin( 0 );

  resultsList = new OListView( mainFrame );
  resultsList->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding));
  mainLayout->addWidget( resultsList, 1 );

  detailsFrame = new QFrame( mainFrame, "detailsFrame" );
  QVBoxLayout *detailsLayout = new QVBoxLayout( detailsFrame );
  richEdit = new QTextView( detailsFrame );
  richEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding));
  //richEdit->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ));
  detailsLayout->addWidget( richEdit, 1 );

  buttonGroupActions = new QHButtonGroup( this );
  _buttonCount = 0;
//  buttonGroupActions->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

  buttonLayout = new QHBoxLayout( detailsFrame );
  buttonGroupActions->hide();
  //buttonLayout->addWidget( buttonGroupActions, 0 );

   detailsLayout->addLayout( buttonLayout );
   /*
   QPushButton *buttonShow = new QPushButton( detailsFrame, "Show" );
   buttonShow->setText( tr("show") );
   QPushButton *buttonEdit = new QPushButton( detailsFrame, "Edit" );
   buttonEdit->setText( tr("edit") );
   buttonLayout->addWidget( buttonShow, 0 );
   buttonLayout->addWidget( buttonEdit, 0 );
*/
  mainLayout->addWidget( detailsFrame );
  detailsFrame->hide();

  searches.append( new AdressSearch( resultsList, tr("adressbook") ) );
  searches.append( new TodoSearch( resultsList, tr("todo") ) );
  searches.append( new DatebookSearch( resultsList, tr("datebook") ) );
  searches.append( new AppLnkSearch( resultsList, tr("applications") ) );
  searches.append( new DocLnkSearch( resultsList, tr("documents") ) );

  makeMenu();
  setCentralWidget( mainFrame );

  popupTimer = new QTimer();

  connect(popupTimer, SIGNAL(timeout()), SLOT(showPopup()));
  connect(resultsList, SIGNAL(pressed(QListViewItem*)), SLOT(setCurrent(QListViewItem*)));
  connect(resultsList, SIGNAL(clicked(QListViewItem*)), SLOT(stopTimer(QListViewItem*)));
  connect(buttonGroupActions, SIGNAL(clicked(int)), SLOT( slotAction(int) ) );
//   connect(buttonEdit, SIGNAL(clicked()), SLOT( editItem() ) );

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
		QIntDict<QString> acts = res->actions();
		QButton *button;
		for (uint i = 0; i < acts.count(); i++){
			button = buttonGroupActions->find( i );
			qDebug("action %i >%s<",i,acts[i]->latin1());
			if (!button) {
				qDebug("BUTTON");
				button = new QPushButton( detailsFrame );
				buttonLayout->addWidget( button, 0 );
				buttonGroupActions->insert( button, i);
			}
			button->setText( *acts[i] );
			button->show();
		}
		for (uint i = acts.count(); i < _buttonCount; i++){
			qDebug("remove button %i of %i",i, _buttonCount);
			button = buttonGroupActions->find( i );
			if (button) button->hide();
		}
		_buttonCount = acts.count();
// 		buttonShow = new QPushButton( detailsFrame, "Show" ) ;
// 		buttonShow->setText( "test" );
// 		buttonLayout->addWidget( buttonShow, 0 );
// buttonGroupActions->insert(buttonShow);
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
        if (!_currentItem) return;
}

void MainWindow::setSearch( const QString &key )
{
	for (SearchGroup *s = searches.first(); s != 0; s = searches.next() )
		s->setSearch( key );
}


void MainWindow::slotAction( int act)
{
	if (_currentItem->rtti() == OListViewItem::Result){
		ResultItem *res = (ResultItem*)_currentItem;
//		ResultItem *res = dynamic_cast<ResultItem*>(item);
		res->action(act);
	}
}

