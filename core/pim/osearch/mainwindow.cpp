/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// (c) 2002 Patrick S. Vogt <tille@handhelds.org>


#include "olistview.h"
#include "olistviewitem.h"
#include "resultitem.h"
#include "adresssearch.h"
#include "todosearch.h"
#include "datebooksearch.h"
#include "applnksearch.h"
#include "doclnksearch.h"
#include "mainwindow.h"

#include <opie2/owait.h>

#include <qpe/qpemessagebox.h>
#include <qpe/resource.h>
#include <qpe/config.h>


#include <qpe/qpeapplication.h>
#include <qpe/global.h>

#include <qtoolbar.h>
#include <qaction.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qtoolbutton.h>
#include <qstring.h>
#include <qlabel.h>
#include <qfile.h>
#include <qhbuttongroup.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include <qintdict.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qsignalmapper.h>
#include <qtextbrowser.h>
#include <qregexp.h>
#include <qwhatsthis.h>
#include <qmenubar.h>



static const char* const image1_data[] = {
"14 14 3 1",
". c None",
"# c #000000",
"a c #ff0000",
"..............",
"..##.......###",
".#aa#....##aa#",
"#aaaa#.##aaaa#",
".##aaa#aaaaa##",
"...#aaaaaaa#..",
"....#aaaaa#...",
"...#aaaaa#....",
"..#aaaaaaa#...",
".#aaaaaaaaa#..",
"#aaaa###aaaa#.",
"#aaa#..##aaa#.",
"#aaa#...#aa#..",
".###.....##..."};


MainWindow::MainWindow( QWidget *parent, const char *name, WFlags f ) :
  QMainWindow( parent, name, f ), _currentItem(0)
{
  setCaption( tr("OSearch") );

  setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

  QFrame *mainFrame = new QFrame( this, "mainFrame" );
  mainFrame->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

  mainLayout = new QVBoxLayout( mainFrame );
  mainLayout->setSpacing( 0 );
  mainLayout->setMargin( 3 );

  resultsList = new OListView( mainFrame );
  resultsList->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
  mainLayout->addWidget( resultsList );

  detailsFrame = new QFrame( mainFrame, "detailsFrame" );
  QVBoxLayout *detailsLayout = new QVBoxLayout( detailsFrame );
  richEdit = new QTextView( detailsFrame );
  QWhatsThis::add( richEdit, tr("The details of the current result") );
  richEdit->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
  detailsLayout->addWidget( richEdit );

  buttonBox = new QHBox( mainFrame, "Button Box" );

  _buttonCount = 0;

  mainLayout->addWidget( detailsFrame );
  mainLayout->addWidget( buttonBox );
  detailsFrame->hide();
  buttonBox->hide();

  searches.append( new AppLnkSearch( resultsList, tr("Applications") ) );
  searches.append( new DocLnkSearch( resultsList, tr("Documents") ) );
  searches.append( new TodoSearch( resultsList, tr("Todo List") ) );
  searches.append( new DatebookSearch( resultsList, tr("Calendar") ) );
  searches.append( new AdressSearch( resultsList, tr("Contacts") ) );

  setCentralWidget( mainFrame );

  popupTimer = new QTimer();
  searchTimer = new QTimer();

  connect(popupTimer, SIGNAL(timeout()), SLOT(showPopup()));
  connect(searchTimer, SIGNAL(timeout()), SLOT(searchStringChanged()));
  connect(resultsList, SIGNAL(pressed(QListViewItem*)), SLOT(setCurrent(QListViewItem*)));
  connect(resultsList, SIGNAL(clicked(QListViewItem*)), SLOT(stopTimer(QListViewItem*)));

  signalMapper = new QSignalMapper( this );

  connect(signalMapper, SIGNAL(mapped(int)), SLOT( slotAction(int) ) );

  makeMenu();

   Config cfg( "osearch", Config::User );
   cfg.setGroup( "search_settings" );
   actionCaseSensitiv->setOn( cfg.readBoolEntry( "caseSensitiv", false ) );
   actionWildcards->setOn( cfg.readBoolEntry( "wildcards", false ) );
//   actionWholeWordsOnly->setOn( cfg.readBoolEntry( "whole_words_only", false ) );
}

void MainWindow::makeMenu()
{
   QToolBar *toolBar = new QToolBar( this );
   QToolBar *searchBar = new QToolBar(this);
   QMenuBar *menuBar = new QMenuBar( toolBar );
   QPopupMenu *searchMenu = new QPopupMenu( menuBar );
//   QPopupMenu *viewMenu = new QPopupMenu( menuBar );
   QPopupMenu *cfgMenu = new QPopupMenu( menuBar );
   QPopupMenu *searchOptions = new QPopupMenu( cfgMenu );

   setToolBarsMovable( false );
   toolBar->setHorizontalStretchable( true );
   menuBar->insertItem( tr( "Search" ), searchMenu );
   menuBar->insertItem( tr( "Settings" ), cfgMenu );

   //SETTINGS MENU
   cfgMenu->insertItem( tr( "Search" ), searchOptions );
   QPopupMenu *pop;
   for (SearchGroup *s = searches.first(); s != 0; s = searches.next() ){
		pop = s->popupMenu();
		if (pop){
			cfgMenu->insertItem( s->text(0), pop );
		}
  }


  //SEARCH
  SearchAllAction = new QAction( tr("Search all"),QString::null,  0, this, 0 );
  SearchAllAction->setIconSet( Resource::loadIconSet( "find" ) );
 // QWhatsThis::add( SearchAllAction, tr("Search everything...") );
  connect( SearchAllAction, SIGNAL(activated()), this, SLOT(searchAll()) );
  SearchAllAction->addTo( searchMenu );
  searchMenu->insertItem( tr( "Options" ), searchOptions );

  //SEARCH OPTIONS
  //actionWholeWordsOnly = new QAction( tr("Whole words only"),QString::null,  0, this, 0, true );
  //actionWholeWordsOnly->addTo( searchOptions );
  actionCaseSensitiv = new QAction( tr("Case sensitiv"),QString::null,  0, this, 0, true );
  actionCaseSensitiv->addTo( searchOptions );
  actionWildcards = new QAction( tr("Use wildcards"),QString::null,  0, this, 0, true );
  actionWildcards->addTo( searchOptions );

  //SEARCH BAR
  LabelEnterText = new QLabel( searchBar, "Label" );
  LabelEnterText->setAutoMask( FALSE );
  LabelEnterText->setText( tr( "Search for: " ) );

  addToolBar( searchBar, "Search", QMainWindow::Top, TRUE );
  QLineEdit *searchEdit = new QLineEdit( searchBar, "seachEdit" );
  QWhatsThis::add( searchEdit, tr("Enter your search terms here") );
  searchEdit->setFocus();
  searchBar->setHorizontalStretchable( TRUE );
  searchBar->setStretchableWidget( searchEdit );

  //Search button
  SearchAllAction->addTo( searchBar );

  //image ripped of off opie-login/loginwindow.cpp
  QPixmap image1( ( const char** ) image1_data );

  //Clear text
  ClearSearchText = new QToolButton( searchBar, "ClearSearchText");
  ClearSearchText->setText( tr( "" ) );
  ClearSearchText->setPixmap( image1 );

  connect( searchEdit, SIGNAL( textChanged(const QString&) ),this, SLOT( setSearch(const QString&) ) );
  connect( ClearSearchText, SIGNAL( clicked() ), searchEdit, SLOT( clear() ) );

}

MainWindow::~MainWindow()
{
   Config cfg( "osearch", Config::User );
   cfg.setGroup( "search_settings" );
   cfg.writeEntry( "caseSensitiv", actionCaseSensitiv->isOn() );
   cfg.writeEntry( "wildcards", actionWildcards->isOn() );
   //cfg.writeEntry( "whole_words_only", actionWholeWordsOnly->isOn() );
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
			button = buttonMap[i];
			if (!button) {
			    qWarning(" no button for %s", (*acts[i]).latin1() );
			    button = new QPushButton( buttonBox );
			    buttonMap.insert( i,  button );
                            signalMapper->setMapping(button, i );
                            connect(button, SIGNAL(clicked() ), signalMapper, SLOT(map() ) );
			}
			button->setText( *acts[i] );
			button->show();
		}
		for (uint i = acts.count(); i < _buttonCount; i++){
			button = buttonMap[i];
			if (button) button->hide();
		}
		_buttonCount = acts.count();
		detailsFrame->show();
                buttonBox->show();

	}else {
            detailsFrame->hide();
            buttonBox->hide();
        }
	popupTimer->start( 300, true );
}

void MainWindow::stopTimer(QListViewItem*)
{
        popupTimer->stop();
}

void MainWindow::showPopup()
{
	popupTimer->stop();
        if (!_currentItem) return;
	QPopupMenu *pop = _currentItem->popupMenu();
	if (pop) pop->popup( QCursor::pos() );
}

void MainWindow::setSearch( const QString &key )
{
	searchTimer->stop();
	_searchString = key;
	searchTimer->start( 300 );
}

void MainWindow::searchStringChanged()
{
#ifdef NEW_OWAIT
	OWait("setting search string");
#endif
	searchTimer->stop();
	QString ss = _searchString;
	//ss = Global::stringQuote( _searchString );
//	if (actionWholeWordsOnly->isOn())
//		ss = "\\s"+_searchString+"\\s";
//	qDebug(" set searchString >%s<",ss.latin1());
	QRegExp re( ss );
	re.setCaseSensitive( actionCaseSensitiv->isOn() );
	re.setWildcard( actionWildcards->isOn() );
	for (SearchGroup *s = searches.first(); s != 0; s = searches.next() )
		s->setSearch( re );
}

void MainWindow::searchAll()
{
#ifdef NEW_OWAIT
	OWait("searching...");
#endif
	for (SearchGroup *s = searches.first(); s != 0; s = searches.next() ){
		s->doSearch();
		//resultsList->repaint();
	}
}

void MainWindow::slotAction( int act )
{
	if ( _currentItem && _currentItem->rtti() == OListViewItem::Result){
		ResultItem *res = (ResultItem*)_currentItem;
//		ResultItem *res = dynamic_cast<ResultItem*>(item);
		res->action(act);
	}
}

void MainWindow::optionChanged(int )
{
	searchStringChanged();
}
