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
#include <qlabel.h>
#include <qlistview.h>
#include <qtextview.h>
#include <qlineedit.h>
#include <qtabwidget.h>
#include <qcombobox.h>
#include <qlayout.h>

#include "pksettingsbase.h"
#include "packagelistitem.h"

MainWindow::MainWindow( QWidget *parent, const char *name, WFlags f = 0 ) :
  QMainWindow( parent, name, f )
{	
  setCaption( tr("Package Manager") );

  listViewPackages =  new QListView( this,0,0 );
  setCentralWidget( listViewPackages );

  makeMenu();		

  QFontMetrics fm = fontMetrics();
  int wlw = width()*2;
  int w0  = fm.width(tr("Package"))+30;
 // int w0  = fm.width(tr("Package"))+30;
  int w2  = fm.width("00000")+4;
  int w1  = wlw-w2-w0-20;
	listViewPackages->addColumn( tr("Package"), w0 );
	listViewPackages->addColumn( tr("Description"), w1 );
	listViewPackages->addColumn( tr("Size"), w2 );
  listViewPackages->setColumnWidthMode(0,QListView::Manual);
  listViewPackages->setColumnWidthMode(1,QListView::Manual);
  listViewPackages->setColumnWidthMode(2,QListView::Manual);
  listViewPackages->setSelectionMode( QListView::Multi );

  connect( section, SIGNAL( activated(int) ),
	   this, SLOT( sectionChanged() ) );
  connect( subsection, SIGNAL(activated(int) ),
	   this, SLOT( subSectionChanged() ) );
  connect( listViewPackages, SIGNAL( pressed( QListViewItem* ) ),
	   this, SLOT( setCurrent( QListViewItem* ) ) );
	
  settings = new PackageManagerSettings(this,0,TRUE);

  ipkg = new PmIpkg( settings, this );
  packageList.setSettings( settings );
  getList();	
  setSections();
  setSubSections();
  displayList();
}

void MainWindow::makeMenu()
{

  QPEToolBar *toolBar = new QPEToolBar( this );
  QPEMenuBar *menuBar = new QPEMenuBar( toolBar );
  QPopupMenu *srvMenu = new QPopupMenu( menuBar );
  QPopupMenu *viewMenu = new QPopupMenu( menuBar );
  QPopupMenu *cfgMenu = new QPopupMenu( menuBar );
  //    QPopupMenu *sectMenu = new QPopupMenu( menuBar );

  popupMenu = new QPopupMenu( this );

  contextMenu = new QPopupMenu( this );

  setToolBarsMovable( false );
  toolBar->setHorizontalStretchable( true );
  menuBar->insertItem( tr( "Package" ), srvMenu );
  menuBar->insertItem( tr( "View" ), viewMenu );
  menuBar->insertItem( tr( "Settings" ), cfgMenu );
  //    menuBar->insertItem( tr( "Sections" ), sectMenu );

  QLabel *spacer = new QLabel( "", toolBar );
  spacer->setBackgroundColor( toolBar->backgroundColor() );
  toolBar->setStretchableWidget( spacer );

		
  runAction = new QAction( tr( "Commit" ),
			   Resource::loadPixmap( "oipkg/install" ),
			   QString::null, 0, this, 0 );
  connect( runAction, SIGNAL( activated() ),
	   this, SLOT( runIpkg() ) );
  runAction->addTo( toolBar );
  runAction->addTo( srvMenu );

  srvMenu->insertSeparator ();

  updateAction = new QAction( tr( "Update" ),
			      Resource::loadIconSet( "oipkg/update" ),
			      QString::null, 0, this, 0 );
  connect( updateAction, SIGNAL( activated() ),
	   this , SLOT( updateList() ) );
  updateAction->addTo( toolBar );
  updateAction->addTo( srvMenu );

  detailsAction = new QAction( tr( "Details" ),
      		Resource::loadIconSet( "find" ),
  		    QString::null, 0, this, 0 );
  connect( detailsAction, SIGNAL( activated() ),
               this , SLOT( showDetails() ) );
  detailsAction->addTo( toolBar );
  detailsAction->addTo( srvMenu );

  QAction *cfgact;

  cfgact = new QAction( tr( "Setups" ),
	//		Resource::loadIconSet( "" ),
			QString::null, 0, this, 0 );
  connect( cfgact, SIGNAL( activated() ),
	   SLOT( showSettings() ) );
  cfgact->addTo( cfgMenu );
		
  cfgact = new QAction( tr( "Servers" ),
	//		Resource::loadIconSet( "" ),
			QString::null, 0, this, 0 );
  connect( cfgact, SIGNAL( activated() ),
	   SLOT( showSettingsSrv() ) );
  cfgact->addTo( cfgMenu );
  cfgact = new QAction( tr( "Destinations" ),
		//	Resource::loadIconSet( "" ),
			QString::null, 0, this, 0 );
  connect( cfgact, SIGNAL( activated() ),
	   SLOT( showSettingsDst() ) );
  cfgact->addTo( cfgMenu );

    QAction *a;

	sectionBar = new QPEToolBar( this );
 	addToolBar( sectionBar,  "Section", QMainWindow::Top, TRUE );
  sectionBar->setHorizontalStretchable( true );
  QLabel *label = new QLabel( tr("Section: "), sectionBar );
  label->setBackgroundColor( sectionBar->backgroundColor() );
 	sectionBar->setStretchableWidget( label );
  section = new QComboBox( false, sectionBar );
//  section->setBackgroundMode( PaletteBackground );
  label = new QLabel( " / ", sectionBar );
  label->setBackgroundColor( sectionBar->backgroundColor() );
  subsection = new QComboBox( false, sectionBar );

  a = new QAction( tr( "Close Section" ), Resource::loadPixmap( "close" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( sectionClose() ) );
  a->addTo( sectionBar );

  sectionAction = new QAction( tr( "Sections" ), QString::null, 0, this, 0 );
  connect( sectionAction, SIGNAL( toggled(bool) ), this, SLOT( sectionShow(bool) ) );
  sectionAction->setToggleAction( true );
  sectionAction->setOn( true );
  sectionAction->addTo( viewMenu );

  findBar = new QPEToolBar(this);
  addToolBar( findBar,  "Search", QMainWindow::Top, TRUE );
  label = new QLabel( tr("Filter: "), findBar );
  label->setBackgroundColor( findBar->backgroundColor() );
  findBar->setHorizontalStretchable( TRUE );
  findEdit = new QLineEdit( findBar, "findEdit" );
  findBar->setStretchableWidget( findEdit );
  connect( findEdit, SIGNAL( textChanged( const QString & ) ),
       this, SLOT( displayList() ) );
//	a = new QAction( tr( "Filter" ), Resource::loadPixmap( "next" ), QString::null, 0, this, 0 );
//  connect( a, SIGNAL( activated() ), this, SLOT( filterList() ) );
//  a->addTo( findBar );
//    a->addTo( edit );
  a = new QAction( tr( "Close Find" ), Resource::loadPixmap( "close" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( findClose() ) );
  a->addTo( findBar );
  findAction = new QAction( tr( "Find" ), QString::null, 0, this, 0 );
  connect( findAction, SIGNAL( toggled(bool) ), this, SLOT( findShow(bool) ) );
  findAction->setToggleAction( true );
  findAction->setOn( true );
  findAction->addTo( viewMenu );

}

MainWindow::~MainWindow()
{
}

void MainWindow::runIpkg()
{
  ipkg->commit( packageList );
  updateList(); //to remove
}

void MainWindow::updateList()
{
	packageList.clear();
  ipkg->update();
  getList();
}

void MainWindow::getList()
{
  packageList.update();
  displayList();
}

void MainWindow::filterList()
{
 	QString f = "";
  if ( findAction->isOn() ) f = findEdit->text();
  packageList.filterPackages( f );
}

void MainWindow::displayList()
{
	filterList();
  listViewPackages->clear();
  Package *pack = packageList.first();
  while( pack )
  {
  	if ( pack && (pack->name() != "") )
	  	listViewPackages->insertItem( new PackageListItem( listViewPackages, pack ) );
    pack = packageList.next();
  }	
}

void MainWindow::sectionChanged()
{
  disconnect( section, SIGNAL( activated(int) ),
	      this, SLOT( sectionChanged() ) );
  disconnect( subsection, SIGNAL(activated(int) ),
	      this, SLOT( subSectionChanged() ) );
  subsection->clear();
  packageList.setSection( section->currentText() );
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
  packageList.setSubSection( subsection->currentText() );
  connect( section, SIGNAL( activated(int) ),
	   this, SLOT( sectionChanged() ) );
  connect( subsection, SIGNAL(activated(int) ),
	   this, SLOT( subSectionChanged() ) );
  displayList();
}

void MainWindow::setSections()
{
  section->clear();
  section->insertStringList( packageList.getSections() );
}

void MainWindow::setSubSections()
{
  subsection->clear();
  subsection->insertStringList( packageList.getSubSections() );
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
  return;
  pvDebug(2, "name "+((Package*)p)->name());
  activePackage = (Package*)p;
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

