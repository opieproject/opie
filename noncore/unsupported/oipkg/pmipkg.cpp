#include "pmipkg.h"
#include "pkdesc.h"
#include "pkfind.h"
#include "pksettings.h"
#include "package.h"
#include "packagelistitem.h"

#include <qpe/process.h>
#include <qpe/resource.h>
#include <qpe/stringutil.h>
#include <qpe/qcopenvelope_qws.h>
#include <qdir.h>
#include <qfile.h>
#include <qmultilineedit.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qtextview.h>

#include <stdlib.h>
#include <unistd.h>

#include "mainwindow.h"
#include "runwindow.h"


PmIpkg::PmIpkg( PackageManagerSettings* s, QWidget* p,  const char * name, WFlags f )
//  : RunWindow ( p, name, f )
  	: QObject ( p )
{
  settings = s;
  runwindow = new RunWindow ( p, name, f );
  linkDest = new QCopChannel( "QPE/MakeLinks", this );
  connect( linkDest, SIGNAL(received(const QCString &, const QByteArray &)),
	   this, SLOT(linkDestination( const QString &, const  QByteArray&)) );
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

  out( "Starting to "+ args+"<br>\n");
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
	r = system(cmd.latin1());
  QFile f( redirect );
  while ( ! f.open(IO_ReadOnly) ) {};
 // if ( f.open(IO_ReadOnly) ) {};
  {
    QTextStream t( &f );
    QString fp;
    while ( !t.eof() )
    {
				out( t.readLine() +"<br>" );
    }
  }
  f.close();
  out( "Finished!<br>");
#endif

  return r;
}

void PmIpkg::makeLinks(QString file)
{
  out( "<br>creating links<br>" );
  QString dest = settings->getDestinationUrl();
  out("for package "+file+" in "+dest+"<br>");
  system(("ipkg -d "+dest+" files "+file+"> /tmp/oipkg.pipe 2>&1").latin1());
  QFile f( "/tmp/oipkg.pipe" );
  while ( ! f.open(IO_ReadOnly) ) {};
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
  QString destFile = file.right( file.length() - dest.length() );
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
      out( "<b>"+fi->absFilePath()+"</b>" );
  		processLinkDir( fi->absFilePath(), dest );      			
			++it;
    }
  } else
  if ( fileInfo.isFile() )
  {
    const char *instFile = strdup( (file).ascii() );
    const char *linkFile = strdup(  (destFile).ascii());
    out( "linking: "+file+" -> "+destFile  );
    qDebug( "linking: %s -> %s", instFile, linkFile );
    symlink( instFile, linkFile );
  }
}

void PmIpkg::commit( PackageList pl )
{   	
  runwindow->showMaximized();
//  exec();
  runwindow->outPut->setText("");
  out( "<h1>Starting...</h1><br>\n");
  QStringList to_remove, to_install;   	

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

  out("<p>"+inst+"</p>"+"<p>"+rem+"</p>");
  bool ok=TRUE;

  qDebug("to remove=%i; to install=%i",to_remove.count(),to_install.count());

  int jobs = to_remove.count()+to_install.count();
  if ( jobs < 1 ) return;

  if ( to_remove.count() )
    for (QStringList::ConstIterator it=to_remove.begin(); it!=to_remove.end(); ++it)
      if ( runIpkg("remove " + *it) != 0 ) ok = false;
  if ( to_install.count() )
    for (QStringList::ConstIterator it=to_install.begin(); it!=to_install.end(); ++it)
    {
      if ( runIpkg("install " + *it) == 0 )
		  {    	
	    	if ( settings->createLinks() )
					makeLinks( *it );
  		}else{
      	ok = false;
      }
    }
	
  // ##### If we looked in the list of files, we could send out accurate
  // ##### messages. But we don't bother yet, and just do an "all".
  QCopEnvelope e("QPE/System", "linkChanged(QString)");
  QString lf = QString::null;
  e << lf;
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
  runwindow->showMaximized();
	runIpkg( "update" );
}

void PmIpkg::out( QString o )
{
	runwindow->outPut->setText( runwindow->outPut->text()+o );
  runwindow->outPut->setContentsPos(0,runwindow->outPut->contentsHeight());
}
