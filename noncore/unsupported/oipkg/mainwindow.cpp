/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// Copyright (C) 2000 Trolltech AS.
// adadpted form qpe/qipkg
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>


#include "mainwindow.h"

#include <qpe/qpemenubar.h>
#include <qpe/qpemessagebox.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/qpetoolbar.h>
#include <qpe/qcopenvelope_qws.h>
#include <qaction.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qtoolbutton.h>
#include <qstring.h>
#include <qlabel.h>
#include <qfile.h>
#include <qlistview.h>
#include <qtextview.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qtabwidget.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlayout.h>

#include "pksettingsbase.h"
#include "utils.h"
#include "packagelistitem.h"


MainWindow::MainWindow( QWidget *parent, const char *name, WFlags f ) :
  QMainWindow( parent, name, f )
//  packageListServers( QObject(parent), name ),
//  packageListSearch( parent, name ),
//  packageListDocLnk( parent, name )
{	
  setCaption( tr("Package Manager") );
  settings = new PackageManagerSettings(this,0,TRUE);
  listViewPackages =  new PackageListView( this,"listViewPackages",settings );
  setCentralWidget( listViewPackages );
  listViewPackages->addList( tr("feeds"), &packageListServers );
  listViewPackages->addList( tr("ipkgfind"), &packageListSearch );
  listViewPackages->addList( tr("documents"), &packageListDocLnk );
  ipkg = new PmIpkg( settings, this );
  packageListServers.setSettings( settings );
  packageListSearch.setSettings( settings );
  packageListDocLnk.setSettings( settings );
	pvDebug(9,"packageListServers.update");
  packageListServers.update();
	pvDebug(9,"packageListDocLnk.update");
 	pvDebug(0,"no UPDATE of DocLnk");
  packageListDocLnk.update();
	pvDebug(9,"makeMenu");
  makeMenu();	
  makeChannel();
  //opie is hardcoded default ;)
//	pvDebug(9,"section->setCurrentItem");
//  for (int i=0;i<section->count();i++)
//  	if (section->text(i)=="opie")
//   		section->setCurrentItem(i);
//  sectionChanged();

  connect( section,    SIGNAL(activated(int)), SLOT(sectionChanged()) );
  connect( subsection, SIGNAL(activated(int)), SLOT(subSectionChanged()) );

  connect( settings->removeLinksButton, SIGNAL( clicked()),
  					 SLOT(removeLinks()) );
  connect( settings->createLinksButton, SIGNAL( clicked()),
  					 SLOT(createLinks()) );

	pvDebug(9,"displayList");
  displayList();
}

void MainWindow::makeMenu()
{

  QPEToolBar *toolBar = new QPEToolBar( this );
  QPEMenuBar *menuBar = new QPEMenuBar( toolBar );
  QPopupMenu *srvMenu = new QPopupMenu( menuBar );
  QPopupMenu *viewMenu = new QPopupMenu( menuBar );
  QPopupMenu *cfgMenu = new QPopupMenu( menuBar );
  QPopupMenu *helpMenu = new QPopupMenu( menuBar );

  setToolBarsMovable( false );
  toolBar->setHorizontalStretchable( true );
  menuBar->insertItem( tr( "Package" ), srvMenu );
  menuBar->insertItem( tr( "View" ), viewMenu );
  menuBar->insertItem( tr( "Settings" ), cfgMenu );
  menuBar->insertItem( tr( "Help" ), helpMenu );

  QLabel *spacer;
//  spacer = new QLabel( "", toolBar );
//  spacer->setBackgroundColor( toolBar->backgroundColor() );
//  toolBar->setStretchableWidget( spacer );

		
  runAction = new QAction( tr( "Apply" ),
			   Resource::loadPixmap( "oipkg/install" ),
			   QString::null, 0, this, 0 );
  connect( runAction, SIGNAL( activated() ),
	   this, SLOT( runIpkg() ) );
  runAction->addTo( toolBar );
  runAction->addTo( srvMenu );

  srvMenu->insertSeparator();

  updateAction = new QAction( tr( "Update" ),
			      Resource::loadIconSet( "oipkg/update" ),
			      QString::null, 0, this, 0 );
  connect( updateAction, SIGNAL( activated() ),
	   this , SLOT( updateList() ) );
  updateAction->addTo( toolBar );
  updateAction->addTo( srvMenu );

  QAction *cfgact;

  cfgact = new QAction( tr( "Setups" ),
			QString::null, 0, this, 0 );
  connect( cfgact, SIGNAL( activated() ),
	   SLOT( showSettings() ) );
  cfgact->addTo( cfgMenu );
		
  cfgact = new QAction( tr( "Servers" ),
			QString::null, 0, this, 0 );
  connect( cfgact, SIGNAL( activated() ),
	   SLOT( showSettingsSrv() ) );
  cfgact->addTo( cfgMenu );
  cfgact = new QAction( tr( "Destinations" ),
			QString::null, 0, this, 0 );
  connect( cfgact, SIGNAL( activated() ),
	   SLOT( showSettingsDst() ) );
  cfgact->addTo( cfgMenu );

  QAction *a;

  // SECTIONS
	sectionBar = new QPEToolBar( this );
 	addToolBar( sectionBar,  "Section", QMainWindow::Top, TRUE );
  sectionBar->setHorizontalStretchable( true );
  QLabel *label = new QLabel( tr("Section: "), sectionBar );
  label->setBackgroundColor( sectionBar->backgroundColor() );
  section = new QComboBox( false, sectionBar );
  label = new QLabel( " / ", sectionBar );
  label->setBackgroundColor( sectionBar->backgroundColor() );
  subsection = new QComboBox( false, sectionBar );
  a = new QAction( tr( "Close Section" ), Resource::loadPixmap( "close" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( sectionClose() ) );
  a->addTo( sectionBar );
  setSections();
  setSubSections();
  sectionAction = new QAction( tr( "Sections" ), QString::null, 0, this, 0 );
  connect( sectionAction, SIGNAL( toggled(bool) ), this, SLOT( sectionShow(bool) ) );
  sectionAction->setToggleAction( true );
  sectionAction->addTo( viewMenu );
 	sectionBar->setStretchableWidget( section );

  //FIND
  findBar = new QPEToolBar(this);
  addToolBar( findBar,  "Filter", QMainWindow::Top, TRUE );
  label = new QLabel( tr("Filter: "), findBar );
  label->setBackgroundColor( findBar->backgroundColor() );
  findBar->setHorizontalStretchable( TRUE );
  findEdit = new QLineEdit( findBar, "findEdit" );
  findBar->setStretchableWidget( findEdit );
  connect( findEdit, SIGNAL( textChanged( const QString & ) ),
       this, SLOT( displayList() ) );
  a = new QAction( tr( "Clear Find" ), Resource::loadPixmap( "back" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), findEdit, SLOT( clear() ) );
  a->addTo( findBar );
  a = new QAction( tr( "Close Find" ), Resource::loadPixmap( "close" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( findClose() ) );
  a->addTo( findBar );
  findAction = new QAction( tr( "Filter" ), QString::null, 0, this, 0 );
  connect( findAction, SIGNAL( toggled(bool) ), this, SLOT( findShow(bool) ) );
  findAction->setToggleAction( true );
  findAction->addTo( viewMenu );

  //SEARCH
  searchBar = new QPEToolBar(this);
  addToolBar( searchBar,  "Search", QMainWindow::Top, TRUE );
  label = new QLabel( tr("ipkgfind: "), searchBar );
  label->setBackgroundColor( searchBar->backgroundColor() );
  searchBar->setHorizontalStretchable( TRUE );
  searchEdit = new QLineEdit( searchBar, "seachEdit" );
  searchBar->setStretchableWidget( searchEdit );
//  connect( searchEdit, SIGNAL( textChanged( const QString & ) ),
//       this, SLOT( displayList() ) );
  a = new QAction( tr( "Clear Search" ), Resource::loadPixmap( "back" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), searchEdit, SLOT( clear() ) );
  a->addTo( searchBar );
  searchCommit  = new QAction( tr( "Do Search" ), Resource::loadPixmap( "find" ), QString::null, 0, this, 0 );
  connect( searchCommit, SIGNAL( activated() ),  SLOT( remotePackageQuery() ) );
  searchCommit->addTo( searchBar );
  a = new QAction( tr( "Close Find" ), Resource::loadPixmap( "close" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( searchClose() ) );
  a->addTo( searchBar );
  searchAction = new QAction( tr( "Search" ), QString::null, 0, this, 0 );
  connect( searchAction, SIGNAL( toggled(bool) ), this, SLOT( searchShow(bool) ) );
  searchAction->setToggleAction( true );
  searchAction->addTo( viewMenu );

  //DEST
  destBar = new QPEToolBar(this);
  addToolBar( destBar,  "Destination", QMainWindow::Top, TRUE );
  label = new QLabel( tr("Destination: "), destBar );
  label->setBackgroundColor( destBar->backgroundColor() );
  destBar->setHorizontalStretchable( TRUE );
  destination = new QComboBox( false, destBar );
  destination->insertStringList( settings->getDestinationNames() );
  setComboName(destination,settings->getDestinationName());
  connect( destination, SIGNAL(activated(int)),
  			 settings, SLOT(activeDestinationChange(int)) );
  spacer = new QLabel( " ", destBar );
  spacer->setBackgroundColor( destBar->backgroundColor() );
  CheckBoxLink = new QCheckBox( tr("Link"), destBar);
  CheckBoxLink->setBackgroundColor( destBar->backgroundColor() );
  CheckBoxLink->setChecked( settings->createLinks() );
  connect( CheckBoxLink, SIGNAL(toggled(bool)),
     				 settings, SLOT(linkEnabled(bool)) );
  destAction = new QAction( tr( "Destinations" ), QString::null, 0, this, 0 );
  connect( destAction, SIGNAL( toggled(bool) ), SLOT( destShow(bool) ) );
  a = new QAction( tr( "Close Destinations" ), Resource::loadPixmap( "close" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), SLOT( destClose() ) );
  a->addTo( destBar );
  destBar->setStretchableWidget( CheckBoxLink );
  destAction->setToggleAction( true );
 // destAction->addTo( viewMenu );

//   helpMenu
  helpMenu->insertSeparator();
	a = new QAction( tr( "Package Actions" ), QString::null, 0, this, 0 );
 	a->addTo( helpMenu );
  helpMenu->insertSeparator();
	a = new QAction( tr( "Install" ),
 						Resource::loadPixmap( "oipkg/install" ), QString::null, 0, this, 0 );
 	a->addTo( helpMenu );
	a = new QAction( tr( "Remove" ),
 						Resource::loadPixmap( "oipkg/uninstall" ), QString::null, 0, this, 0 );
 	a->addTo( helpMenu );
  helpMenu->insertSeparator();
	a = new QAction( tr( "Package Status" ), QString::null, 0, this, 0 );
 	a->addTo( helpMenu );
  helpMenu->insertSeparator();
	a = new QAction( tr( "New version, installed" ),
 						Resource::loadPixmap( "oipkg/installed" ), QString::null, 0, this, 0 );
 	a->addTo( helpMenu );
	a = new QAction( tr( "New version, not installed" ),
 						Resource::loadPixmap( "oipkg/uninstalled" ), QString::null, 0, this, 0 );
 	a->addTo( helpMenu );
	a = new QAction( tr( "Old version, installed" ),
 						Resource::loadPixmap( "oipkg/installedOld" ), QString::null, 0, this, 0 );
 	a->addTo( helpMenu );
	a = new QAction( tr( "Old version, not installed" ),
 						Resource::loadPixmap( "oipkg/uninstalledOld" ), QString::null, 0, this, 0 );
 	a->addTo( helpMenu );
	a = new QAction( tr( "Old version, new version installed" ),
 						Resource::loadPixmap( "oipkg/uninstalledOldinstalledNew" ), QString::null, 0, this, 0 );
 	a->addTo( helpMenu );
	a = new QAction( tr( "New version, old version installed" ),
 						Resource::loadPixmap( "oipkg/uninstalledInstalledOld" ), QString::null, 0, this, 0 );
 	a->addTo( helpMenu );
//	a = new QAction( tr( "" ),
// 						Resource::loadPixmap( "oipkg/" ), QString::null, 0, this, 0 );
// 	a->addTo( helpMenu );

  // configure the menus
  Config cfg( "oipkg", Config::User );
  cfg.setGroup( "gui" );

  findShow( cfg.readBoolEntry( "findBar", true ) );
  searchShow( cfg.readBoolEntry( "searchBar", true ) );
  sectionShow( cfg.readBoolEntry( "sectionBar", true ) );
  destShow( cfg.readBoolEntry( "destBar", false ) );
}

MainWindow::~MainWindow()
{
  Config cfg( "oipkg", Config::User );
  cfg.setGroup( "gui" );
  cfg.writeEntry( "findBar", !findBar->isHidden() );
  cfg.writeEntry( "searchBar", !searchBar->isHidden() );
  cfg.writeEntry( "sectionBar", !sectionBar->isHidden() );
  cfg.writeEntry( "destBar", !destBar->isHidden() );
	
}

void MainWindow::runIpkg()
{
  packageListServers.allPackages();
  ipkg->loadList( &packageListSearch );
	ipkg->loadList( &packageListDocLnk );
  ipkg->loadList( &packageListServers );
  ipkg->commit();
  ipkg->clearLists();
  // ##### If we looked in the list of files, we could send out accurate
  // ##### messages. But we don't bother yet, and just do an "all".
  QCopEnvelope e("QPE/System", "linkChanged(QString)");
  QString lf = QString::null;
  e << lf;
  displayList();
}

void MainWindow::updateList()
{
	packageListServers.clear();
	packageListSearch.clear();

  packageListDocLnk.clear();
  ipkg->update();
  packageListServers.update();
  packageListSearch.update();
  packageListDocLnk.update();
}

void MainWindow::filterList()
{
 	QString f = "";
  if ( findAction->isOn() ) f = findEdit->text();
  packageListServers.filterPackages( f );
}

void MainWindow::displayList()
{
	filterList();
  listViewPackages->display();
}

void MainWindow::sectionChanged()
{
  disconnect( section, SIGNAL( activated(int) ),
	      this, SLOT( sectionChanged() ) );
  disconnect( subsection, SIGNAL(activated(int) ),
	      this, SLOT( subSectionChanged() ) );
  subsection->clear();
  packageListServers.setSection( section->currentText() );
  setSubSections();
  connect( section, SIGNAL( activated(int) ),
	   this, SLOT( sectionChanged() ) );
  connect( subsection, SIGNAL(activated(int) ),
	   this, SLOT( subSectionChanged() ) );
  displayList();
}

void MainWindow::subSectionChanged()
{
  disconnect( section, SIGNAL( activated(int) ),
	      this, SLOT( sectionChanged() ) );
  disconnect( subsection, SIGNAL(activated(int) ),
	      this, SLOT( subSectionChanged() ) );
  packageListServers.setSubSection( subsection->currentText() );
  connect( section, SIGNAL( activated(int) ),
	   this, SLOT( sectionChanged() ) );
  connect( subsection, SIGNAL(activated(int) ),
	   this, SLOT( subSectionChanged() ) );
  displayList();
}

void MainWindow::setSections()
{
  section->clear();
  section->insertStringList( packageListServers.getSections() );
}

void MainWindow::setSubSections()
{
  subsection->clear();
  subsection->insertStringList( packageListServers.getSubSections() );
}


void MainWindow::showSettings()
{
  if ( settings->showDialog( 0 ) )
    updateList();
}
void MainWindow::showSettingsSrv()
{
  if ( settings->showDialog( 1 ) )
    updateList();
}
void MainWindow::showSettingsDst()
{
  if ( settings->showDialog( 2 ) )
    updateList();
}

void MainWindow::sectionShow(bool b)
{
	if (b) sectionBar->show();
  else sectionBar->hide();
  sectionAction->setOn( b );
}

void MainWindow::sectionClose()
{
  sectionAction->setOn( false );
}

void MainWindow::findShow(bool b)
{
	if (b) findBar->show();
  else findBar->hide();
  findAction->setOn( b );
}

void MainWindow::findClose()
{
  findAction->setOn( false );
}

void MainWindow::searchShow(bool b)
{
	if (b) searchBar->show();
  else searchBar->hide();
  searchAction->setOn( b );
}

void MainWindow::searchClose()
{
  searchAction->setOn( false );
}


void MainWindow::destShow(bool b)
{
	if (b) destBar->show();
  else destBar->hide();
  destAction->setOn( b );
}

void MainWindow::destClose()
{
  destAction->setOn( false );
}

void MainWindow::setDocument(const QString &fileName)
{
 	if ( !QFile::exists( fileName ) ) return;
	ipkg->installFile( fileName );
  QCopEnvelope e("QPE/System", "linkChanged(QString)");
  QString lf = QString::null;
  e << lf;
}


void MainWindow::makeChannel()
{   	
	channel = new QCopChannel( "QPE/Application/oipkg", this );
	connect( channel, SIGNAL(received(const QCString&, const QByteArray&)),
		this, SLOT(receive(const QCString&, const QByteArray&)) );
}



void MainWindow::receive(const QCString &msg, const QByteArray &arg)
{
	pvDebug(3, "QCop "+msg+" "+QCString(arg));
	if ( msg == "installFile(QString)" )
 	{
  	ipkg->installFile( QString(arg) );
	}else if( msg == "removeFile(QString)" )
 	{
  	ipkg->removeFile( QString(arg) );
	}else if( msg == "createLinks(QString)" )
 	{
  	ipkg->createLinks( QString(arg) );
	}else if( msg == "removeLinks(QString)" )
 	{
  	ipkg->removeLinks( QString(arg) );
	}else{
    	pvDebug(2,"Huh what do ya want")
 	}
}


void MainWindow::createLinks()
{
	pvDebug(2,"creating links...");
	ipkg->createLinks( settings->destinationurl->text() );
}

void MainWindow::removeLinks()
{
	pvDebug(2,"removing links...");
	ipkg->removeLinks( settings->destinationurl->text() );
}

void MainWindow::remotePackageQuery()
{
 	packageListSearch.query( searchEdit->text() );
  packageListSearch.update();
 	displayList();
}
