#include "pmipkg.h"
#include "pksettings.h"
#include "package.h"
#include "packagelistitem.h"

#include <qpe/process.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/stringutil.h>
#include <qdir.h>
#include <qfile.h>
#include <qmultilineedit.h>
#include <qstring.h>
#include <qcheckbox.h>
#include <qtextstream.h>
#include <qtextview.h>

#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qlayout.h>

#include <stdlib.h>
#include <unistd.h>

#include "mainwindow.h"


PmIpkg::PmIpkg( PackageManagerSettings* s, QWidget* p,  const char * name, WFlags f )
  	: QObject ( p )
{
  settings = s;
  runwindowopen = false;
 	runwindow = new RunWindow( p, name, true, f );
}

PmIpkg::~PmIpkg()
{
}

//#define PROC
#define SYSTEM
int PmIpkg::runIpkg(const QString& args, const QString& dest )
{
  pvDebug(2,"PmIpkg::runIpkg "+args);

#ifdef PROC
  QStringList cmd = "/usr/bin/ipkg ";
#endif
#ifdef SYSTEM
  QString cmd = "/usr/bin/ipkg ";
#endif
	pvDebug( 3,"PmIpkg::runIpkg got dest="+dest);
	if ( dest == "" )
	  cmd += " -dest "+settings->getDestinationName();
  else
	  cmd += " -dest "+ dest;

 	cmd += " -force-defaults ";

//  if (runwindow->forcedepends->isChecked())
//  	 cmd += " -force-depends ";

  out( "<hr><br>Starting to "+ args+"<br>\n");
  cmd += args;
  int r = 0;
#ifdef PROC
	QString o = "start";
  Process *ipkg = new Process( "ls");//cmd );
  out( "running:<br>\n"+ipkg->arguments().join(" ")+"<br>\n" );
  QString description;
  ipkg->exec("",o);
//  out( o );
#endif
#ifdef SYSTEM
  out( "running:<br>\n"+cmd+"<br>\n" );
  QString redirect = "/tmp/oipkg.pipe";
  cmd += " 2>&1 | tee "+redirect+" 2>&1";
  pvDebug(2, "running >"+cmd+"<");
	r = system(cmd.latin1());
  QFile f( redirect );
  QString line;
  QString oldLine;
  while ( ! f.open(IO_ReadOnly) ) {};
 // {
    QTextStream t( &f );
 //   QString fp;
    while ( !t.eof() )
    {
      line = t.readLine();
			if ( line != oldLine )
   	 	{
      	out( line +"<br>" );
				oldLine = line;
    	}
    }
//  }
  f.close();
  out( "Finished!<br>");
#endif

  return r;
}

void PmIpkg::makeLinks(Package *pack)
{
	pvDebug( 2, "PmIpkg::makeLinks "+ pack->name());
  QString dest = settings->getDestinationUrlByName( pack->dest() );
  if (dest == "/" ) return;
	{
    Config cfg( "oipkg", Config::User );
    cfg.setGroup( "Common" );
    QString statusDir = cfg.readEntry( "statusDir", "" );
	}
 	QString fn = dest+"/"+statusDir+"/info/"+pack->name()+".list";
  linkPackage( fn, dest );
}

void PmIpkg::linkPackage( QString packFileName, QString dest )
{
 QFile f( packFileName );
 if ( ! f.open(IO_ReadOnly) )
  {
  	out( "<b>Panik!</b> Could not open:<br>"+packFileName );
   	return;
  };
  out( "<br>creating links<br>" );
  out("for package "+packFileName+" in "+dest+"<br>");
  QTextStream t( &f );
  QString fp;
  while ( !t.eof() )
    {
      fp = t.readLine();
      processLinkDir( fp, dest );
    }	
  f.close();
}

void PmIpkg::processLinkDir( QString file, QString dest )
{
	pvDebug( 7,"PmIpkg::processLinkDir "+file+" to "+ dest);
	if ( dest == "???" ) return;
  QString destFile = file;
	file = dest+"/"+file;
 	if (file == dest) return;
  QFileInfo fileInfo( file );
  if ( fileInfo.isDir() )
  {
  	QDir destDir( destFile );
    destDir.mkdir( destFile, true );
    QDir d( file );
    d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
    const QFileInfoList *list = d.entryInfoList();
    QFileInfoListIterator it( *list );
    QFileInfo *fi;
    while ( (fi=it.current()) )
    {
   //   out( "<b>"+fi->absFilePath()+"</b>" );
  		processLinkDir( fi->absFilePath(), dest );      			
			++it;
    }
  } else
  if ( fileInfo.isFile() )
  {
    const char *instFile = strdup( (file).ascii() );
    const char *linkFile = strdup(  (destFile).ascii());
//    out( "linking: "+file+" -> "+destFile  );
    qDebug( "linking: %s -> %s", instFile, linkFile );
    symlink( instFile, linkFile );
  }
}

void PmIpkg::commit( PackageList pl )
{   	
  int sizecount = 0;  	
  to_install.clear();
  to_remove.clear();
  QString rem="<b>"+tr("To remove:")+"</b><br>\n";
  QString inst="<b>"+tr("To install:")+"</b><br>\n";
  pl.allPackages();
  for( Package *pack = pl.first();pack ; (pack = pl.next())  )
    {
      if ( pack && (pack->name() != "") && pack)
	{
	  if ( pack->toInstall() )
	    {
	      to_install.append( pack );
       	sizecount += pack->size().toInt();
	      inst += pack->name()+"\t("+tr("on ")+pack->dest()+")<br>";
	    }
	  if ( pack->toRemove() )
	    {
	      to_remove.append( pack );
       	sizecount += 1;
	      rem += pack->name()+"<br>";
	    }
	}
    }

	startDialog();

}

void PmIpkg::startDialog()
{
  QDialog *d = new QDialog();
  QGridLayout *RunWindowLayout = new QGridLayout( d );
  RunWindowLayout->setSpacing( 2 );
  RunWindowLayout->setMargin( 2 );

    QHBoxLayout *buttons = new QHBoxLayout;
    buttons->setSpacing( 6 );
    buttons->setMargin( 0 );

    PackageListView *plv = new PackageListView(d, "install",settings);
    RunWindowLayout->addWidget( plv, 1, 0 );
    for (Package *it=to_remove.first(); it != 0; it=to_remove.next() )
	  {
      plv->insertItem( new PackageListItem(plv, it,settings) );
   	} 	
    for (Package *it=to_install.first(); it != 0; it=to_install.next() )
    {
      plv->insertItem( new PackageListItem(plv, it,settings) );
    }
    QPushButton *doItButton = new QPushButton( d, "doItButton" );
    doItButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, doItButton->sizePolicy().hasHeightForWidth() ) );
    QFont doItButton_font(  doItButton->font() );
    doItButton_font.setPointSize( 8 );
    doItButton->setFont( doItButton_font );
    doItButton->setText( tr( "Do all " ) );
    doItButton->setAutoResize( FALSE );
    buttons->addWidget( doItButton );

    QPushButton *installButton = new QPushButton( d, "installButton" );
    QFont installButton_font(  installButton->font() );
    installButton_font.setPointSize( 8 );
    installButton->setFont( installButton_font );
    installButton->setText( tr( "Install" ) );
    installButton->setAutoResize( TRUE );
    buttons->addWidget( installButton );

    QPushButton *removeButton = new QPushButton( d, "removeButton" );
    QFont removeButton_font(  removeButton->font() );
    removeButton_font.setPointSize( 7 );
    removeButton->setFont( removeButton_font );
    removeButton->setText( tr( "Remove" ) );
    removeButton->setAutoResize( TRUE );
    buttons->addWidget( removeButton );

    QPushButton *cancelButton = new QPushButton( d, "cancelButton" );
    QFont cancelButton_font(  cancelButton->font() );
    cancelButton_font.setPointSize( 8 );
    cancelButton->setFont( cancelButton_font );
    cancelButton->setText( tr( "Cancel" ) );
    cancelButton->setAutoResize( TRUE );
    buttons->addWidget( cancelButton );

    RunWindowLayout->addLayout( buttons, 3, 0 );

  connect( doItButton, SIGNAL( clicked() ),
  					this, SLOT( doIt() ) );
  connect( installButton, SIGNAL( clicked() ),
  					this, SLOT( install() ) );
  connect( removeButton, SIGNAL( clicked() ),
  					this, SLOT( remove() ) );
  connect( cancelButton, SIGNAL( clicked() ),
  					d, SLOT( close() ) );
  d->showMaximized();
  d->exec();
 // d->close();
}

void PmIpkg::doIt()
{
 	remove();
  install();
}


void PmIpkg::remove()
{
 	if ( to_remove.count() == 0 ) return;
	show( true );

	out("<hr><hr><b>"+tr("Removing")+"<br>"+tr("please wait")+"</b><br>");

   for (Package *it=to_remove.first(); it != 0; it=to_remove.next() )
   {
      if ( runIpkg("remove " + it->name()) == 0)
      {
      	runwindow->progress->setProgress( it->size().toInt() + runwindow->progress->progress());
        it->processed();
        runwindow->progress->setProgress( 1 );
  		}else{
      	out("<b>"+tr("Error while removing")+"</b>"+it->name());
      }
    }
}


void PmIpkg::install()
{
 	if ( to_install.count() == 0 ) return;
	show( true );
	out("<hr><hr><b>"+tr("Installing")+"<br>"+tr("please wait")+"</b><br>");
 	for (Package *it=to_install.first(); it != 0; it=to_install.next() )
    {

      if ( runIpkg("install " + it->getPackageName(), it->dest() ) == 0 )
		  {    	
      	runwindow->progress->setProgress( it->size().toInt() + runwindow->progress->progress());
	    	if ( it->link() )
					makeLinks( it );
     		it->processed();
  		}else{
      	out("<b>"+tr("Error while installing")+"</b>"+it->name());
      }
    }
}

void PmIpkg::linkDestination( const QString msg, const QByteArray dest )
{
	qDebug("msg="+msg+" -- "+QString(dest) );	
//  QDir d( src );
//  d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
////  if (! d.exists() ) return;
//  const QFileInfoList *list = d.entryInfoList();
//  QFileInfoListIterator it( *list );
//  QFileInfo *fi;
//  while ( (fi=it.current()) ) {
//    processLinkDir( fi->absFilePath(), dest );      			
//    ++it;
//  }
}

void PmIpkg::update()
{
	show( false );
	runIpkg( "update" );
 	runwindow->close();
}

void PmIpkg::out( QString o )
{
	runwindow->outPut->setText( runwindow->outPut->text()+o );
  runwindow->outPut->setContentsPos(0, runwindow->outPut->contentsHeight());
}


void PmIpkg::showButtons(bool b)
{
	if ( b )
 	{
		runwindow->cancelButton->hide();
 		runwindow->doItButton->hide();
	  runwindow->removeButton->hide();
  	runwindow->installButton->hide();
  }else{
		runwindow->cancelButton->show();
 		runwindow->doItButton->show();
	  runwindow->removeButton->show();
  	runwindow->installButton->show();

  }
}

void PmIpkg::show(bool b)
{
	if (!runwindow->isVisible())
	  runwindow->showMaximized();
	showButtons(b);
	if ( !b )
 		runwindow->progress->hide();
  else
 		runwindow->progress->show();
}

void PmIpkg::installFile(const QString &fileName)
{

	to_install.clear();
  to_remove.clear();
	pvDebug( 2,"PmIpkg::installFile "+ fileName);
	to_install.append( new Package(fileName,settings) );
	startDialog();
}

