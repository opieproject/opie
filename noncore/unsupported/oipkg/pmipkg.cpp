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
  : RunWindow ( p, name, f )
  //	: QObject ( p )
{
  settings = s;
  
  linkDest = new QCopChannel( "QPE/MakeLinks", this );
  connect( linkDest, SIGNAL(received(const QCString &, const QByteArray &)),
	   this, SLOT(linkDestination( const QString &, const  QString&)) );
}

PmIpkg::~PmIpkg()
{
}

int PmIpkg::runIpkg(const QString& args)
{
  pvDebug(4,"PmIpkg::runIpkg");
  QString cmd = "/usr/bin/ipkg ";
  cmd += " -dest "+settings->getDestinationName();
  cmd += " -force-defaults ";
  
  outPut->setText( outPut->text()+"<br><br>Starting to "+ args+"<br>\n");
  QString redirect = "/tmp/ipkg.pipe";
  cmd += args+" | tee "+redirect+" 2>&1&";

  outPut->setText( outPut->text() + "running:<br>\n"+cmd+"<br>\n" );
  pvDebug( 0, "Execute >"+cmd+"<" );
  int r = system(cmd.latin1());
  QFile f( redirect );
  if (  f.open(IO_ReadOnly) ) {
    QTextStream t( &f );
    QString fp;
    while ( !t.eof() )
      {
	outPut->setText( outPut->text() + t.readLine() +"\n<br>" );
      }
  }
  f.close();
  outPut->setText( outPut->text() + "\n<br><br>Finished!");
  outPut->setContentsPos(0,outPut->contentsHeight());

  if ( r == 0 )
    {
      QString param = args.left( args.find(" ") );
      QString file = args.right( args.length() - args.find(" ") );
      if ( param == "install" && settings->createLinks() )
	makeLinks( file);
    }

  return r;
}

void PmIpkg::makeLinks(QString file)
{
  outPut->setText( outPut->text() + "<br>creating links<br>" );
  QString dest = settings->getDestinationUrl();
  system(("ipkg -d "+dest+" files "+file+"> /tmp/oipkg.pipe").latin1());
  QFile f( "/tmp/oipkg.pipe" );
  if ( ! f.open(IO_ReadOnly) )return;
  QTextStream t( &f );
  QString fp;
  while ( !t.eof() )
    {
      processLinkDir( t.readLine(), dest );
    }	
  f.close();
}

void PmIpkg::processLinkDir( QString file, QString dest )
{
  QString destFile = file.right( file.length() - dest.length() );
  QFileInfo fileInfo( file );
  if ( fileInfo.isFile() )
    {
      const char *instFile = strdup( (file).ascii() );
      const char *linkFile = strdup(  (destFile).ascii());
      outPut->setText( outPut->text() + "linking: "+file+" -> "+destFile  );
      symlink( instFile, linkFile );
    }
  if ( fileInfo.isDir() )
    {
      QDir destDir( destFile );
      destDir.mkdir( destFile, true );
      QDir d( file );
      d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );

      const QFileInfoList *list = d.entryInfoList();
      QFileInfoListIterator it( *list );
      QFileInfo *fi;
      while ( (fi=it.current()) ) {
    	processLinkDir( fi->absFilePath(), dest );      			
	++it;
      }
    }
}

void PmIpkg::commit( PackageList pl )
{   	
  showMaximized();
  exec();
  outPut->setText( "<b>Starting...</b><br>\n");
  QStringList to_remove, to_install;   	

  QString rem="To remove:<br>\n";
  QString inst="To install:<br>\n";;
  for( Package *pack = pl.first();pack ; (pack = pl.next())  )
    {
      if ( pack && (pack->name() != "") && pack)
	{
	  if ( pack->toInstall() )
	    {
	      to_install.append( pack->name() );
	      inst += pack->name()+"\n";
	    }
	  if ( pack->toRemove() )
	    {
	      to_remove.append( pack->name() );
	      rem += pack->name()+"\n";
	    }
	}
    }

  outPut->setText( outPut->text()+inst+rem);
  bool ok=TRUE;

  qDebug("to remove=%i; to install=%i",to_remove.count(),to_install.count());

  int jobs = to_remove.count()+to_install.count();
  if ( jobs < 1 ) return;

  if ( to_remove.count() )
    for (QStringList::ConstIterator it=to_remove.begin(); it!=to_remove.end(); ++it)
      if ( runIpkg("remove " + *it) != 0 ) ok = false;
  if ( to_install.count() )
    for (QStringList::ConstIterator it=to_install.begin(); it!=to_install.end(); ++it)
      if ( runIpkg("install " + *it) != 0 ) ok = false;
	
  // ##### If we looked in the list of files, we could send out accurate
  // ##### messages. But we don't bother yet, and just do an "all".
  QCopEnvelope e("QPE/System", "linkChanged(QString)");
  QString lf = QString::null;
  e << lf;
}

void PmIpkg::linkDestination( QString src, QString dest )
{
  QDir d( src );
  d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
  if (! d.exists() ) return;
  const QFileInfoList *list = d.entryInfoList();
  QFileInfoListIterator it( *list );
  QFileInfo *fi;
  while ( (fi=it.current()) ) {
    processLinkDir( fi->absFilePath(), dest );      			
    ++it;
  }
}
