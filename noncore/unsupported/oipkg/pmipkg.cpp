/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>
#include "pmipkg.h"
#include "pksettings.h"
#include "package.h"
#include "packagelistitem.h"

//#include <opie/oprocess.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/stringutil.h>
#include <qpe/qpeapplication.h>
#include <qdir.h>
#include <qfile.h>
#include <qgroupbox.h>
#include <qmultilineedit.h>
#include <qstring.h>
#include <qcheckbox.h>
#include <qtextstream.h>
#include <qtextview.h>
#include <qmessagebox.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qlayout.h>

#include <stdlib.h>
#include <unistd.h>

#include "mainwindow.h"



PmIpkg::PmIpkg( QWidget* p, PackageManagerSettings* s, const char * name, WFlags f )
  	: QObject ( p )
{
  settings = s;
 	runwindow = new RunWindow( p, name, true, f );

  Config cfg( "oipkg", Config::User );
  cfg.setGroup( "ipkg" );
  ipkg_cmd = cfg.readEntry( "cmd", "ipkg" )+" ";
}

PmIpkg::~PmIpkg()
{
}

bool PmIpkg::runIpkg(const QString& args, const QString& dest )
{
	bool ret=false;
  QDir::setCurrent("/tmp");
  QString cmd = ipkg_cmd;
	qDebug("PmIpkg::runIpkg got dest="+dest);
	if (!args.contains("update"))
	{
		if ( dest == "" )
		  cmd += " -dest "+settings->getDestinationName();
	  else
		  cmd += " -dest "+ dest;

	 	cmd += " -force-defaults ";

  	if ( installDialog && installDialog->_force_depends )
  	{
	  	if (installDialog->_force_depends->isChecked())
  			 cmd += " -force-depends ";
		  if (installDialog->_force_reinstall->isChecked())
  			 cmd += " -force-reinstall ";
	  	if (installDialog->_force_remove->isChecked())
  			 cmd += " -force-removal-of-essential-packages ";
	  }
	} //!args.contains("update")

  out( "Starting to "+ args+"\n");
  qApp->processEvents();
  cmd += args;
  out( "running:\n"+cmd+"\n" );
  qDebug("running:"+cmd);
  qApp->processEvents();
  FILE *fp;
  char line[130];
  QString lineStr, lineStrOld;
  sleep(1);
  cmd +=" 2>&1";
  fp = popen(  (const char *) cmd, "r");
  if ( fp == NULL ) {
     qDebug("Could not execute '" + cmd + "'! err=%d", fp);
     out("\nError while executing "+ cmd+"\n\n");
     ret = false;
  } else {
     while ( fgets( line, sizeof line, fp) != NULL)
     {
        lineStr = line;
        lineStr=lineStr.left(lineStr.length()-1);
        //Configuring opie-oipkg...Done
        if (lineStr.contains("Done")) ret = true;
        if (lineStr!=lineStrOld)
	        out(lineStr);
        lineStrOld = lineStr;
			  qApp->processEvents();
     }
  }
  pclose(fp);
  qDebug(QString(ret?"success\n":"failure\n"));
  return ret;
}

void PmIpkg::makeLinks(Package *pack)
{
	qDebug("PmIpkg::makeLinks "+ pack->name());
 	QString pn = pack->name();
  linkPackage( pack->packageName(), pack->dest() );
}

QStringList* PmIpkg::getList( QString packFileName, QString d )
{
	QString dest = settings->getDestinationUrlByName( d );
 	dest = dest==""?d:dest;
	//  if (dest == "/" ) return 0;
	{
    Config cfg( "oipkg", Config::User );
    cfg.setGroup( "Common" );
    statusDir = cfg.readEntry( "statusDir", "" );
	}
 	QString packFileDir = dest+"/"+statusDir+"/info/"+packFileName+".list";
 	QFile f( packFileDir );
  qDebug("Try to open %s", packFileDir.latin1());
 	if ( ! f.open(IO_ReadOnly) )
  {
  	out( "Could not open:\n"+packFileDir );
	  f.close();
   	packFileDir = "/"+statusDir+"/info/"+packFileName+".list";
   	f.setName( packFileDir );
	  qDebug("Try to open %s", packFileDir.latin1());
	 	if ( ! f.open(IO_ReadOnly) )
  	{
	   	qDebug(" Panik!  Could not open"+packFileDir);
 		 	out( "Could not open:\n"+packFileDir+"\n Panik!" );
     	return (QStringList*)0;
    }
  }
  QStringList *fileList = new QStringList();
  QTextStream t( &f );
  while ( !t.eof() )
    {
      *fileList += t.readLine();
    }	
  f.close();
  return fileList;
}

void PmIpkg::linkPackage( QString packFileName, QString dest )
{
	if (dest == "root" || dest == "/" ) return;
	QStringList *fileList = getList( packFileName, dest );
 	processFileList( fileList, dest );
  delete fileList;
}

void PmIpkg::processFileList( QStringList *fileList, QString d )
{
	if (!fileList) return;
  for (uint i=0; i < fileList->count(); i++)
    {
      QString dest = settings->getDestinationUrlByName( d );
		 	dest = dest==""?d:dest;
      processLinkDir( (*fileList)[i], dest );
    }	
}


void PmIpkg::processLinkDir( QString file, QString dest )
{
	qDebug("PmIpkg::processLinkDir "+file+" to "+ dest);
	if ( dest == "???" || dest == "" ) return;
  QString destFile = file;
	file = dest+"/"+file;
 	if (file == dest) return;
//  if (linkOpp==createLink) out( "\ncreating links\n" );
//  if (linkOpp==removeLink) out( "\nremoving links\n" );  				
  QFileInfo fileInfo( file );
  if ( fileInfo.isDir() )
  {
    qDebug("process dir "+file);
  	QDir destDir( destFile );
    if (linkOpp==createLink) destDir.mkdir( destFile, true );
    QDir d( file );
//    d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
    const QFileInfoList *list = d.entryInfoList();
    QFileInfoListIterator it( *list );
    QFileInfo *fi;
    while ( (fi=it.current()) )
    {
    	qDebug("processLinkDir "+fi->absFilePath());
  		processLinkDir( fi->absFilePath(), dest );      			
			++it;
    }
  } else
  if ( fileInfo.isFile() )
  {
    const char *instFile = strdup( (file).latin1() );
    const char *linkFile = strdup(  (destFile).latin1());
    if( linkOpp==createLink )
		{
    	qDebug("linking: "+file+" -> "+destFile );
    	symlink( instFile, linkFile );
  	}
  }  else  {
    const char *linkFile = strdup(  (destFile).latin1());
    if( linkOpp==removeLink )
    {
      QFileInfo toRemoveLink( destFile );
    	if ( !QFile::exists( file ) && toRemoveLink.isSymLink() )
     	{
	      qDebug("removing "+destFile+" no "+file);
     		unlink( linkFile ); 	
      }
    }
  }
}

void PmIpkg::loadList( PackageList *pl )
{   	
  for( Package *pack = pl->first();pack ; (pack = pl->next())  )
  {
  	if ( pack && (pack->name() != "") && pack)
		{
		  if ( pack->toInstall() )
	      to_install.append( pack );
		  if ( pack->toRemove() )
	      to_remove.append( pack );
		}
  }
}

void PmIpkg::commit()
{   	
  int sizecount = 0;
  installDialog = new InstallDialog(settings,0,0,true); 	
  installDialog->toRemoveItem->setOpen( true );
	installDialog->toInstallItem->setOpen( true );
  for (uint i=0; i < to_remove.count(); i++)
  {
  	sizecount += 1;
    installDialog->toRemoveItem->insertItem( new PackageListItem(installDialog->ListViewPackages, to_remove.at(i),settings) );
 	} 	
  for (uint i=0; i < to_install.count(); i++)
  {
  	sizecount += to_install.at(i)->size().toInt();
    installDialog->toInstallItem->insertItem( new PackageListItem(installDialog->ListViewPackages, to_install.at(i),settings) );
  }
  runwindow->progress->setTotalSteps(sizecount);
  qDebug("Install size %i",sizecount);
  installDialog->showMaximized();
  installDialog->show();
  if ( installDialog->exec() )
  {
  	doIt();
	  runwindow->showMaximized();
  	runwindow->show();
  }
 	installDialog->close();
  delete installDialog;
  installDialog = 0;
  out(tr("\nAll done."));
}

void PmIpkg::doIt()
{
	runwindow->progress->setProgress(0);
	show();
 	remove();
  install();
}


void PmIpkg::remove()
{
 	if ( to_remove.count() == 0 ) return;

	out(tr("Removing")+"\n"+tr("please wait")+"\n\n");

	QStringList *fileList;
  for (uint i=0; i < to_remove.count(); i++)
  {
		if ( to_remove.at(i)->link() )fileList = getList( to_remove.at(i)->name(), to_remove.at(i)->dest() );     	
    if ( runIpkg("remove " + to_remove.at(i)->installName(), to_remove.at(i)->dest() ))
    {
      runwindow->progress->setProgress( 1 );
     	linkOpp = removeLink;
    	to_remove.at(i)->processed();
     	qDebug("link "+QString::number(i));
		  if ( to_remove.at(i)->link() )
      		processFileList( fileList, to_remove.at(i)->dest() );
     	//if ( to_remove.at(i) ) to_remove.take( i );
          	
      out("\n");
 		}else{
     	out(tr("Error while removing ")+to_remove.at(i)->name()+"\n");
		  if ( to_remove.at(i)->link() )
      		processFileList( fileList, to_remove.at(i)->dest() );
    }
	  if ( to_remove.at(i)->link() )
      		processFileList( fileList, to_remove.at(i)->dest() );
	  if ( to_remove.at(i)->link() )delete fileList;
  }
  to_remove.clear();
  out("\n");
}


void PmIpkg::install()
{
 	if ( to_install.count() == 0 ) return;
	out(tr("Installing")+"\n"+tr("please wait")+"\n"); 	
  for (uint i=0; i < to_install.count(); i++)
  {
  	qDebug("install loop %i of %i installing %s",i,to_install.count(),to_install.at(i)->installName().latin1()); 
    if (to_install.at(i)->link())
  	{
  		// hack to have package.list
	   	// in "dest"/usr/lib/ipkg/info/
  	  QString rds = settings->getDestinationUrlByName("root");
  	  QString lds = settings->getDestinationUrlByName(to_install.at(i)->dest());
    	QString listFile = "usr/lib/ipkg/lists/"+to_install.at(i)->name()+".list";
     	rds += listFile;
	  	lds += listFile;
	  	const char *rd = rds.latin1();
  		const char *ld = lds.latin1();
   		qDebug("linking: "+rds+" -> "+lds );
	   	symlink( rd, ld );
  	}
	  if ( runIpkg("install " + to_install.at(i)->installName(), to_install.at(i)->dest() ))
		{    	
    	runwindow->progress->setProgress( to_install.at(i)->size().toInt() + runwindow->progress->progress());
     	to_install.at(i)->processed();
	    linkOpp = createLink;
		  if ( to_install.at(i)->link() )
  	  	makeLinks( to_install.at(i) );
   //   to_install.take( i );
      out("\n");
  	}else{
     	out(tr("Error while installing")+to_install.at(i)->name()+"\n");
	    linkOpp = createLink;
		  if ( to_install.at(i)->link() )
  	  	makeLinks( to_install.at(i) );
    }
   }
   out("\n");
   to_install.clear();
}

void PmIpkg::createLinks( const QString &dest )
{
	qDebug("PmIpkg::createLinks "+dest);
	linkOpp=createLink;
 	QString url = settings->getDestinationUrlByName( dest );
  url = url==""?dest:url;
	processLinkDir( "/opt", url );
	processLinkDir( "/usr", url );
}

void PmIpkg::removeLinks( const QString &dest )
{
	qDebug("PmIpkg::removeLinks "+dest);
	linkOpp=removeLink;
 	QString url = settings->getDestinationUrlByName( dest );
  url = url==""?dest:url;
	processLinkDir( "/opt", url );
	processLinkDir( "/usr", url );
}

void PmIpkg::update()
{
	show();
	runIpkg( "update" );
}

void PmIpkg::out( QString o )
{
	runwindow->outPut->append(o);	
 	runwindow->outPut->setCursorPosition(runwindow->outPut->numLines() + 1,0,FALSE);
}




void PmIpkg::show()
{
	if (!runwindow->isVisible())
 	{
	  runwindow->showMaximized();
	  runwindow->show();
  }
  runwindow->outPut->setText("");
}

void PmIpkg::installFile(const QString &fileName, const QString &dest)
{

	to_install.clear();
  to_remove.clear();
	qDebug("PmIpkg::installFile "+ fileName);
 	Package *p = new Package(fileName,settings);
  if ( dest!="") p->setDest( dest );
	to_install.append( p );
	commit();
 	delete p;
}

void PmIpkg::removeFile(const QString &fileName, const QString &dest)
{

	to_install.clear();
  to_remove.clear();
	qDebug("PmIpkg::removeFile "+ fileName);
 	Package *p = new Package(fileName,settings);
  if ( dest!="") p->setDest( dest );
	to_remove.append( p );
	commit();
 	delete p;
}


void PmIpkg::clearLists()
{
	to_remove.clear();
 	to_install.clear();
}


