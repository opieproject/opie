// adadpted form qpe/qipkg


#include "mainwindow.h"

#include <qpe/qpemenubar.h>
#include <qpe/qpemessagebox.h>
#include <qpe/resource.h>
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
#include <qlineedit.h>
#include <qtabwidget.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qlayout.h>

#include "pksettingsbase.h"
#include "packagelistitem.h"


MainWindow::MainWindow( QWidget *parent, const char *name, WFlags f = 0 ) :
  QMainWindow( parent, name, f )
{	
  settings = new PackageManagerSettings(this,0,TRUE);
  listViewPackages =  new PackageListView( this,"listViewPackages",settings );
  ipkg = new PmIpkg( settings, this );

  setCentralWidget( listViewPackages );
  setCaption( tr("Package Manager") );

//	wait = new QMessageBox(tr("oipkg"),tr("Please wait")//,QMessageBox::Information,QMessageBox::NoButton,QMessageBox::NoButton,QMessageBox::NoButton);
	wait = new QMessageBox(this);
 	wait->setText(tr("Please wait"));

	channel = new QCopChannel( "QPE/Application/oipkg", this );
	connect( channel, SIGNAL(received(const QCString&, const QByteArray&)),
		this, SLOT(receive(const QCString&, const QByteArray&)) );

  makeMenu();		

  connect( section, SIGNAL( activated(int) ),
	   this, SLOT( sectionChanged() ) );
  connect( subsection, SIGNAL(activated(int) ),
	   this, SLOT( subSectionChanged() ) );

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

  setToolBarsMovable( false );
  toolBar->setHorizontalStretchable( true );
  menuBar->insertItem( tr( "Package" ), srvMenu );
  menuBar->insertItem( tr( "View" ), viewMenu );
  menuBar->insertItem( tr( "Settings" ), cfgMenu );
  //    menuBar->insertItem( tr( "Sections" ), sectMenu );

  QLabel *spacer = new QLabel( "", toolBar );
  spacer->setBackgroundColor( toolBar->backgroundColor() );
  toolBar->setStretchableWidget( spacer );

		
  runAction = new QAction( tr( "Apply" ),
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

// would we use for find
//  detailsAction = new QAction( tr( "Details" ),
//      		Resource::loadIconSet( "find" ),
//  		    QString::null, 0, this, 0 );
//  connect( detailsAction, SIGNAL( activated() ),
//               this , SLOT( showDetails() ) );
//  detailsAction->addTo( toolBar );
//  detailsAction->addTo( srvMenu );

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

  a = new QAction( tr( "Close Find" ), Resource::loadPixmap( "close" ), QString::null, 0, this, 0 );
  connect( a, SIGNAL( activated() ), this, SLOT( findClose() ) );
  a->addTo( findBar );
  findAction = new QAction( tr( "Find" ), QString::null, 0, this, 0 );
  connect( findAction, SIGNAL( toggled(bool) ), this, SLOT( findShow(bool) ) );
  findAction->setToggleAction( true );
  findAction->setOn( true );
  findAction->addTo( viewMenu );

  #ifdef NEW
  Config cfg( "oipkg", Config::User );
  cfg.setGroup( "Setting_" + QString::number( setting ) );
  CheckBoxLink->setChecked( cfg.readBoolEntry( "link", false ) );
  findShow(bool b)
  sectionShow(bool b)
  #endif
}

MainWindow::~MainWindow()
{
}

void MainWindow::runIpkg()
{
  ipkg->commit( packageList );
  // ##### If we looked in the list of files, we could send out accurate
  // ##### messages. But we don't bother yet, and just do an "all".
  QCopEnvelope e("QPE/System", "linkChanged(QString)");
  QString lf = QString::null;
  e << lf;
  displayList();
}

void MainWindow::updateList()
{
	wait->show();
	QTimer *t = new QTimer( this );
  connect( t, SIGNAL(timeout()), SLOT( rotateUpdateIcon() ) );
  t->start( 0, false );
	packageList.clear();
  ipkg->update();
  getList();
  t->stop();
	wait->hide();
  	
}

void MainWindow::getList()
{
	wait->show();
  packageList.update();
  displayList();
	wait->hide();
}

void MainWindow::filterList()
{
	wait->show();
 	QString f = "";
  if ( findAction->isOn() ) f = findEdit->text();
  packageList.filterPackages( f );
	wait->hide();
}

void MainWindow::displayList()
{
	wait->hide();
	filterList();
  listViewPackages->clear();
  Package *pack = packageList.first();
  while( pack )
  {
  	if ( pack && (pack->name() != "") )
	  	listViewPackages->insertItem( new PackageListItem( listViewPackages, pack, settings ) );
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

void MainWindow::rotateUpdateIcon()
{
	pvDebug(2, "MainWindow::rotateUpdateIcon");
 if ( updateIcon )
	updateAction->setIconSet( Resource::loadIconSet( "oipkg/update" ) );
 else
	updateAction->setIconSet( Resource::loadIconSet( "oipkg/update2" ) );
 updateIcon = !updateIcon;
}


void MainWindow::setDocument(const QString &fileName)
{
  installFile(fileName);
  // ##### If we looked in the list of files, we could send out accurate
  // ##### messages. But we don't bother yet, and just do an "all".
  QCopEnvelope e("QPE/System", "linkChanged(QString)");
  QString lf = QString::null;
  e << lf;
  displayList();
}

void MainWindow::installFile(const QString &fileName)
{
	pvDebug(3, "MainWindow::installFile "+fileName);
 	if ( !QFile::exists( fileName ) ) return;
	ipkg->installFile( fileName );
  // ##### If we looked in the list of files, we could send out accurate
  // ##### messages. But we don't bother yet, and just do an "all".
  QCopEnvelope e("QPE/System", "linkChanged(QString)");
  QString lf = QString::null;
  e << lf;
  displayList();
}

void MainWindow::receive(const QCString &msg, const QByteArray &arg)
{
	pvDebug(3, "QCop "+msg);
	if ( msg == "installFile(QString)" )
 	{
  	installFile( QString(arg) );
	}
}