#include "pmipkg.h"
#include "pkdesc.h"
#include "pksettings.h"
#include "package.h"
#include "packagelistitem.h"

#include <qpe/process.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/stringutil.h>
#include <qpe/qcopenvelope_qws.h>
#include <qdir.h>
#include <qfile.h>
#include <qmultilineedit.h>
#include <qstring.h>
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
int PmIpkg::runIpkg(const QString& args)
{
  pvDebug(2,"PmIpkg::runIpkg "+args);
#ifdef PROC
  QStringList cmd = "/usr/bin/ipkg ";
#endif
#ifdef SYSTEM
  QString cmd = "/usr/bin/ipkg ";
#endif
  cmd += " -dest "+settings->getDestinationName();
 	cmd += " -force-defaults ";

  out( "<hr><br>Starting to "+ args+"<br>\n");
  cmd += args;
  out( "running:<br>\n"+cmd+"<br>\n" );
  int r = 0;
#ifdef PROC
	QString o = "start";
  Process ipkg( cmd );
  QString description;
  ipkg.exec("",o);
  out( o );
#endif
#ifdef SYSTEM
  QString redirect = "/tmp/oipkg.pipe";
  cmd += " | tee "+redirect+" 2>&1";
  pvDebug(2, "running >"+cmd+"<");
	r = system(cmd.latin1());
  QFile f( redirect );
  QString line;
  QString oldLine;
  while ( ! f.open(IO_ReadOnly) ) {};
  {
    QTextStream t( &f );
 //   QString fp;
    while ( !t.eof() )
    {
      line = t.readLine();
			if ( line != oldLine ) out( line +"<br>" );
    	oldLine = line;
    }
  }
  f.close();
  out( "Finished!<br>");
#endif

  return r;
}

void PmIpkg::makeLinks(QString pack)
{
	pvDebug( 2, "PmIpkg::makeLinks "+ pack);
  out( "<br>creating links<br>" );
  QString dest = settings->getDestinationUrl();
  out("for package "+pack+" in "+dest+"<br>");
	{
    Config cfg( "oipkg", Config::User );
    cfg.setGroup( "Common" );
    QString statusDir = cfg.readEntry( "statusDir", "" );
	}
 	QString fn = dest+"/"+statusDir+"/info/"+pack+".list";
  QFile f( fn );
  if ( ! f.open(IO_ReadOnly) )
  {
  	out( "<b>Panik!</b> Could not open:<br>"+fn );
  };
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
  QString destFile = file;
	file = dest+"/"+file;
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
	show( false );
  runwindow->outPut->setText("");
  out( "<h1>"+tr("Todo")+"</h1>\n"); 	

  QString rem="<b>To remove:</b><br>\n";
  QString inst="<b>To install:</b><br>\n";;
  for( Package *pack = pl.first();pack ; (pack = pl.next())  )
    {
      if ( pack && (pack->name() != "") && pack)
	{
	  if ( pack->toInstall() )
	    {
	      to_install.append( pack->name() );
	      inst += pack->name()+"<br>";
	    }
	  if ( pack->toRemove() )
	    {
	      to_remove.append( pack->name() );
	      rem += pack->name()+"<br>";
	    }
	}
    }

  out("<p>"+inst+"</p>"+"<p>"+rem+"</p><hl>");

  qDebug("to remove=%i; to install=%i",to_remove.count(),to_install.count());

  connect( runwindow->doItButton, SIGNAL( clicked() ),
  					 SLOT( doIt() ) );
  connect( runwindow->installButton, SIGNAL( clicked() ),
  					this, SLOT( install() ) );
  connect( runwindow->removeButton, SIGNAL( clicked() ),
  					this, SLOT( remove() ) );
  connect( runwindow->cancelButton, SIGNAL( clicked() ),
  					runwindow, SLOT( close() ) );

	runwindow->exec();
  // ##### If we looked in the list of files, we could send out accurate
  // ##### messages. But we don't bother yet, and just do an "all".
  QCopEnvelope e("QPE/System", "linkChanged(QString)");
  QString lf = QString::null;
  e << lf;
  return;
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
    for (QStringList::ConstIterator it=to_remove.begin(); it!=to_remove.end(); ++it)
    {
      if ( runIpkg("remove " + *it) == 0)
      {

  		}else{
      	out("<b>"+tr("Error while removing")+"</b>"+*it);
      }
    }
}


void PmIpkg::install()
{
 	if ( to_install.count() == 0 ) return;
	show( true );
	out("<hr><hr><b>"+tr("Installing")+"<br>"+tr("please wait")+"<b><br>");
  if ( to_install.count() )
    for (QStringList::ConstIterator it=to_install.begin(); it!=to_install.end(); ++it)
    {
      if ( runIpkg("install " + *it) == 0 )
		  {    	
	    	if ( settings->createLinks() )
					makeLinks( *it );
  		}else{
      	out("<b>"+tr("Error while installing")+"</b>"+*it);
      }
    }
	
}

void PmIpkg::linkDestination( const QString msg, const QByteArray dest )
// add 3rd package parameter
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
	if ( b )
 		runwindow->progress->show();
  else
 		runwindow->progress->show();
}
