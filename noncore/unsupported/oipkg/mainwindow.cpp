// adadpted form qpe/qipkg


#include "mainwindow.h"

#include <qpe/qpemenubar.h>
#include <qpe/qpemessagebox.h>
#include <qpe/resource.h>
#include <qpe/qpetoolbar.h>
#include <qaction.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qtoolbutton.h>
#include <qstring.h>
#include <qlistview.h>
#include <qtextview.h>
#include <qtabwidget.h>
#include <qcombobox.h>
#include <qlayout.h>

#include "pksettingsbase.h"
#include "packagelistitem.h"

MainWindow::MainWindow( QWidget *parent, const char *name, WFlags f = 0 ) :
  QMainWindow( parent, name, f )
{	
  setCaption( tr("Package Manager") );
  table =  new PackageWindow( this,0,0 );
  setCentralWidget( table );
  makeMenu();		

  QFontMetrics fm = fontMetrics();
  int w0 = fm.width(tr("Package"))+30;
  int w2 = fm.width("00000")+4;
  table->ListViewPackages->setColumnWidth(0,w0);
  table->ListViewPackages->setColumnWidth(1,228-w2-w0); // ### screen-biased
  table->ListViewPackages->setColumnWidth(2,w2);
  table->ListViewPackages->setColumnWidthMode(0,QListView::Manual);
  table->ListViewPackages->setColumnWidthMode(1,QListView::Manual);
  table->ListViewPackages->setColumnWidthMode(2,QListView::Manual);
  table->ListViewPackages->setSelectionMode( QListView::Multi );

  connect( table->section, SIGNAL( activated(int) ),
	   this, SLOT( sectionChanged() ) );
  connect( table->subsection, SIGNAL(activated(int) ),
	   this, SLOT( subSectionChanged() ) );
  connect( table->ListViewPackages, SIGNAL( clicked( QListViewItem* ) ),
	   this, SLOT( setCurrent( QListViewItem* ) ) );
	
  settings = new PackageManagerSettings(this,0,TRUE);

  ipkg = new PmIpkg( settings, this );
  packageList.setSettings( settings );
  newList();	
  setSections();
  setSubSections();
  displayList();
}

void MainWindow::makeMenu()
{

  QPEToolBar *toolBar = new QPEToolBar( this );
  QPEMenuBar *menuBar = new QPEMenuBar( toolBar );
  QPopupMenu *srvMenu = new QPopupMenu( menuBar );
  QPopupMenu *cfgMenu = new QPopupMenu( menuBar );
  //    QPopupMenu *sectMenu = new QPopupMenu( menuBar );

  contextMenu = new QPopupMenu( this );

  setToolBarsMovable( false );
  toolBar->setHorizontalStretchable( true );
  menuBar->insertItem( tr( "Package" ), srvMenu );
  menuBar->insertItem( tr( "Settings" ), cfgMenu );
  //    menuBar->insertItem( tr( "Sections" ), sectMenu );

  toolBar->setStretchableWidget (srvMenu);

		
  runAction = new QAction( tr( "Run" ),
			   Resource::loadPixmap( "oipkg/install" ),
			   QString::null, 0, this, 0 );
  connect( runAction, SIGNAL( activated() ),
	   this, SLOT( runIpkg() ) );
  runAction->addTo( toolBar );
  runAction->addTo( srvMenu );

  srvMenu->insertSeparator ();

  updateAction = new QAction( tr( "Update" ),
			      Resource::loadIconSet( "oipkg/repeat" ),
			      QString::null, 0, this, 0 );
  connect( updateAction, SIGNAL( activated() ),
	   this , SLOT( updateList() ) );
  updateAction->addTo( toolBar );
  updateAction->addTo( srvMenu );

  //    detailsAction = new QAction( tr( "Details" ),
  //    		Resource::loadIconSet( "oipkg/details" ),
  //		    QString::null, 0, this, 0 );
  //    connect( detailsAction, SIGNAL( activated() ),
  //             this , SLOT( showDetails() ) );
  //    detailsAction->addTo( toolBar );
  //    detailsAction->addTo( srvMenu );

  QAction *cfgact;

  cfgact = new QAction( tr( "Setups" ),
			Resource::loadIconSet( "" ),
			QString::null, 0, this, 0 );
  connect( cfgact, SIGNAL( activated() ),
	   SLOT( showSettings() ) );
  cfgact->addTo( cfgMenu );
		
  cfgact = new QAction( tr( "Servers" ),
			Resource::loadIconSet( "" ),
			QString::null, 0, this, 0 );
  connect( cfgact, SIGNAL( activated() ),
	   SLOT( showSettingsSrv() ) );
  cfgact->addTo( cfgMenu );
  cfgact = new QAction( tr( "Destinations" ),
			Resource::loadIconSet( "" ),
			QString::null, 0, this, 0 );
  connect( cfgact, SIGNAL( activated() ),
	   SLOT( showSettingsDst() ) );
  cfgact->addTo( cfgMenu );
	
}

MainWindow::~MainWindow()
{
}

void MainWindow::runIpkg()
{
  ipkg->commit( packageList );
  ipkg->runIpkg("update");
  packageList.update();
}

void MainWindow::updateList()
{
  ipkg->runIpkg("update");
  packageList.update();
}

void MainWindow::newList()
{
  packageList.update();
}

void MainWindow::filterList()
{
  packageList.filterPackages();
}

void MainWindow::displayList()
{
  table->ListViewPackages->clear();
  Package *pack = packageList.first();
  while( pack )
    {
      if ( pack && (pack->name() != "") )
	{
	  table->ListViewPackages->insertItem(
					      new PackageListItem( table->ListViewPackages, pack ) );
	}
      pack = packageList.next();
    }	
}

void MainWindow::sectionChanged()
{
  disconnect( table->section, SIGNAL( activated(int) ),
	      this, SLOT( sectionChanged() ) );
  disconnect( table->subsection, SIGNAL(activated(int) ),
	      this, SLOT( subSectionChanged() ) );
  table->subsection->clear();
  packageList.setSection( table->section->currentText() );
  setSubSections();
  filterList();
  connect( table->section, SIGNAL( activated(int) ),
	   this, SLOT( sectionChanged() ) );
  connect( table->subsection, SIGNAL(activated(int) ),
	   this, SLOT( subSectionChanged() ) );
  displayList();
}

void MainWindow::subSectionChanged()
{
  disconnect( table->section, SIGNAL( activated(int) ),
	      this, SLOT( sectionChanged() ) );
  disconnect( table->subsection, SIGNAL(activated(int) ),
	      this, SLOT( subSectionChanged() ) );
  packageList.setSubSection( table->subsection->currentText() );
  filterList();
  connect( table->section, SIGNAL( activated(int) ),
	   this, SLOT( sectionChanged() ) );
  connect( table->subsection, SIGNAL(activated(int) ),
	   this, SLOT( subSectionChanged() ) );
  displayList();
}

void MainWindow::setSections()
{
  table->section->clear();
  table->section->insertStringList( packageList.getSections() );
}

void MainWindow::setSubSections()
{
  table->subsection->clear();
  table->subsection->insertStringList( packageList.getSubSections() );
}


void MainWindow::showSettings()
{
  if ( settings->showDialog( 0 ) )
    newList();
}
void MainWindow::showSettingsSrv()
{
  if ( settings->showDialog( 1 ) )
    newList();
}
void MainWindow::showSettingsDst()
{
  if ( settings->showDialog( 2 ) )
    newList();
}


void MainWindow::showDetails()
{
  if ( activePackage ) return;
  if ( details )
    {
      details = new PackageDetails( this );
      connect( details->install, SIGNAL(clicked()), SLOT( toggleActivePackage() ) );
      connect( details->remove,  SIGNAL(clicked()), SLOT( toggleActivePackage() ) );
      connect( details->ignore,  SIGNAL(clicked()), details, SLOT(close()));
      details->description->setTextFormat(RichText);
    }

  details->setCaption("Package: " + activePackage->name());
  details->description->setText(activePackage->details() );
  details->install->setEnabled(!activePackage->installed());
  details->remove->setEnabled(activePackage->installed());
  details->showMaximized();
}

void MainWindow::toggleActivePackage()
{
  activePackage->toggleProcess();
  if ( details ) details->close();
}

void MainWindow::setCurrent( QListViewItem* p )
{
  pvDebug(2, "MainWindow::setCurrent ");
  //+((Package*)p)->name());
  activePackage = (Package*)p;
}
