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
#include <qgroupbox.h>
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

  if (_force_depends)
  {
	  if (_force_depends->isChecked())
  		 cmd += " -force-depends ";
	  if (_force_reinstall->isChecked())
  		 cmd += " -force-reinstall ";
	  if (_force_remove->isChecked())
  		 cmd += " -force-removal-of-essential-packages ";
  }

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
    QTextStream t( &f );
    while ( !t.eof() )
    {
      line = t.readLine();
			if ( line != oldLine )
   	 	{
      	out( line +"<br>" );
				oldLine = line;
    	}
    }
  f.close();
  out( "Finished!<br>");
#endif

  return r;
}

void PmIpkg::makeLinks(Package *pack)
{
	pvDebug( 2, "PmIpkg::makeLinks "+ pack->name());
  linkPackage( pack->name(), pack->dest() );
}

QStringList* PmIpkg::getList( QString packFileName, QString d )
{
	QString dest = settings->getDestinationUrlByName( d );
 	dest = dest==""?d:dest;
  if (dest == "/" ) return 0;
	{
    Config cfg( "oipkg", Config::User );
    cfg.setGroup( "Common" );
    QString statusDir = cfg.readEntry( "statusDir", "" );
	}
 	packFileName = dest+"/"+statusDir+"/info/"+packFileName+".list";
 	QFile f( packFileName );
 	if ( ! f.open(IO_ReadOnly) )
  {
   	pvDebug(1," Panik!  Could not open");
  	out( "<b>Panik!</b> Could not open:<br>"+packFileName );
   	return (QStringList*)0;
  }
  QStringList *fileList = new QStringList();
  QTextStream t( &f );
  while ( !t.eof() )
    {
      *fileList += t.readLine();
    }	
  return fileList;
}

void PmIpkg::linkPackage( QString packFileName, QString dest )
{
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
	pvDebug( 7,"PmIpkg::processLinkDir "+file+" to "+ dest);
 	if (linkOpp==createLink) pvDebug( 2,"opp: createLink");
 	if (linkOpp==removeLink) pvDebug( 2,"opp:removeLink");
	if ( dest == "???" || dest == "" ) return;
  QString destFile = file;
	file = dest+"/"+file;
 	if (file == dest) return;
  QFileInfo fileInfo( file );
  if ( fileInfo.isDir() )
  {
  	QDir destDir( destFile );
    if (linkOpp==createLink) destDir.mkdir( destFile, true );
    QDir d( file );
    d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
    const QFileInfoList *list = d.entryInfoList();
    QFileInfoListIterator it( *list );
    QFileInfo *fi;
    while ( (fi=it.current()) )
    {
	    pvDebug(4, "process dir "+fi->absFilePath());
  		processLinkDir( fi->absFilePath(), dest );      			
			++it;
    }
//    if (linkOpp==removeLink)
//    {
//    	pvDebug(2,"remove  destDir "+ destFile );
//    	destDir.remove( destFile, true );
//    }
  } else
  if ( fileInfo.isFile() )
  {
    const char *instFile = strdup( (file).ascii() );
    const char *linkFile = strdup(  (destFile).ascii());
    if( linkOpp==createLink )
		{
    	pvDebug(4, "linking: "+file+" -> "+destFile );
    	symlink( instFile, linkFile );
  	}
  }  else  {
    const char *linkFile = strdup(  (destFile).ascii());
    if( linkOpp==removeLink )
    {
      pvDebug(4,"removing "+destFile+" no "+file);
      QFileInfo toRemoveLink( destFile );
    	if ( !QFile::exists( file ) && toRemoveLink.isSymLink() )
     		unlink( linkFile ); 	
    }
  }
}

void PmIpkg::commit( PackageList pl )
{   	
  int sizecount = 0;  	
  to_install.clear();
  to_remove.clear();
  QString rem="<b>"+tr("To remove:")+"</b><br>\n";
  QString inst="<b>"+tr("To install:")+"</b><br>\n";
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
  installDialog = new QDialog(0,0,true);
  QGridLayout *RunWindowLayout = new QGridLayout( installDialog );
  RunWindowLayout->setSpacing( 2 );
  RunWindowLayout->setMargin( 2 );

    PackageListView *plv = new PackageListView(installDialog, "install",settings);
    plv->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding, plv->sizePolicy().mayShrinkVertically()) );
    RunWindowLayout->addWidget( plv, 1, 0 );
		QCheckListItem *toRemoveItem;
		toRemoveItem= new QCheckListItem( plv, QObject::tr("To remove") );
  	toRemoveItem->setOpen( true );
    for (Package *it=to_remove.first(); it != 0; it=to_remove.next() )
	  {
      toRemoveItem->insertItem( new PackageListItem(plv, it,settings) );
   	} 	
		QCheckListItem *toInstallItem;
		toInstallItem = new QCheckListItem( plv, QObject::tr("To install") );
  	toInstallItem->setOpen( true );
    for (Package *it=to_install.first(); it != 0; it=to_install.next() )
    {
      toInstallItem->insertItem( new PackageListItem(plv, it,settings) );
    }

    QGroupBox *GroupBox1 = new QGroupBox( installDialog, "Ipkg" );
    GroupBox1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)3, GroupBox1->sizePolicy().hasHeightForWidth() ) );
    GroupBox1->setTitle( tr( "Ipkg options" ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 0 );
    GroupBox1->layout()->setMargin( 0 );
    QGridLayout *GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );
    GroupBox1Layout->setSpacing( 3 );
    GroupBox1Layout->setMargin( 3 );

    _force_depends = new QCheckBox( GroupBox1, "_force_depends" );
    _force_depends->setText( tr( "-force-depends" ) );
    _force_depends->setAutoResize( TRUE );
    GroupBox1Layout->addWidget( _force_depends, 0, 0 );
    _force_reinstall = new QCheckBox( GroupBox1, "_force_reinstall" );
    _force_reinstall->setText( tr( "-force-reinstall" ) );
    _force_reinstall->setAutoResize( TRUE );
    GroupBox1Layout->addWidget( _force_reinstall, 1, 0 );
    _force_remove = new QCheckBox( GroupBox1, "_force_remove" );
    _force_remove->setText( tr( "-force-removal-of-essential-packages" ) );
    _force_remove->setAutoResize( TRUE );
    GroupBox1Layout->addWidget( _force_remove, 1, 0 );
    RunWindowLayout->addWidget( GroupBox1 , 3, 0 );

//  connect( doItButton, SIGNAL( clicked() ),
//  					this, SLOT( doIt() ) );
//  connect( installButton, SIGNAL( clicked() ),
//  					this, SLOT( install() ) );
//  connect( removeButton, SIGNAL( clicked() ),
//  					this, SLOT( remove() ) );
//  connect( cancelButton, SIGNAL( clicked() ),
//  					installDialog, SLOT( close() ) );
  installDialog->showMaximized();
  if ( installDialog->exec() ) doIt();
 	installDialog->close();
  out(tr("<b>All done.</b>"));
}

void PmIpkg::doIt()
{
	show( true );
 	remove();
  install();
}


void PmIpkg::remove()
{
 	if ( to_remove.count() == 0 ) return;

	out("<b>"+tr("Removing")+"<br>"+tr("please wait")+"</b><br><hr>");

	QStringList *fileList;
   for (Package *it=to_remove.first(); it != 0; it=to_remove.next() )
   {
			if ( it->link() )fileList = getList( it->name(), it->dest() );     	
      if ( runIpkg("remove " + it->name()) == 0)
      {
        runwindow->progress->setProgress( 1 + runwindow->progress->progress() );
       	linkOpp = removeLink;
	    	if ( it->link() )
      	{
					out( "<br>removing links<br>" );
  				out( "for package "+it->name()+" in "+it->dest()+"<br>" );
      		processFileList( fileList, it->dest() );
        }
     		it->processed();
        out("<br><hr>");
  		}else{
      	out("<b>"+tr("Error while removing")+"</b><hr>"+it->name());
      }
     	pvDebug(2,"delete File List");
	  if ( it->link() )delete fileList;
    }
    out("<br>");
}


void PmIpkg::install()
{
 	if ( to_install.count() == 0 ) return;
	out("<b>"+tr("Installing")+"<br>"+tr("please wait")+"</b><br>");
 	for (Package *it=to_install.first(); it != 0; it=to_install.next() )
    {

      if ( runIpkg("install " + it->getPackageName(), it->dest() ) == 0 )
		  {    	
      	runwindow->progress->setProgress( it->size().toInt() + runwindow->progress->progress());
       	linkOpp = createLink;
	    	if ( it->link() )
      	{
					out( "<br>creating links<br>" );
  				out( "for package "+it->name()+" in "+it->dest()+"<br>" );
					makeLinks( it );
     		}
     		it->processed();
        out("<br><hr>");
  		}else{
      	out("<b>"+tr("Error while installing")+"</b><hr>"+it->name());
      }
    }
    out("<br>");
}

void PmIpkg::createLinks( const QString &dest )
{
	pvDebug(2,"PmIpkg::createLinks "+dest);
	linkOpp=createLink;
 	QString url = settings->getDestinationUrlByName( dest );
  url = url==""?dest:url;
	processLinkDir( "/", url );
}

void PmIpkg::removeLinks( const QString &dest )
{
	pvDebug(2,"PmIpkg::removeLinks "+dest);
	linkOpp=removeLink;
 	QString url = settings->getDestinationUrlByName( dest );
  url = url==""?dest:url;
	processLinkDir( "/", url );
}

void PmIpkg::update()
{
	show( false );
	runIpkg( "update" );
 	runwindow->close();
}

void PmIpkg::out( QString o )
{
	runwindow->outPut->append(o);	
 	//runwindow->outPut->setCursorPosition(0, runwindow->outPut->contentsHeight());
//	runwindow->outPut->setText( runwindow->outPut->text()+o );
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

void PmIpkg::installFile(const QString &fileName, const QString &dest)
{

	to_install.clear();
  to_remove.clear();
	pvDebug( 2,"PmIpkg::installFile "+ fileName);
 	Package *p = new Package(fileName,settings);
  if ( dest!="") p->setDest( dest );
	to_install.append( p );
	startDialog();
 	delete p;
}

void PmIpkg::removeFile(const QString &fileName, const QString &dest)
{

	to_install.clear();
  to_remove.clear();
	pvDebug( 2,"PmIpkg::removeFile "+ fileName);
 	Package *p = new Package(fileName,settings);
  if ( dest!="") p->setDest( dest );
	to_remove.append( p );
	startDialog();
 	delete p;
}

