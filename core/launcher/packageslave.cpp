/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "packageslave.h"
#include <qtopia/qprocess.h>

#ifdef Q_WS_QWS
#include <qtopia/qcopenvelope_qws.h>
#endif

#ifdef Q_WS_QWS
#include <qcopchannel_qws.h>
#endif

#include <qtextstream.h>

#include <stdlib.h>
#include <sys/stat.h> // mkdir()

#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
#include <unistd.h>
#include <sys/vfs.h>
#include <mntent.h>
#elif defined(Q_OS_WIN32)
#include <windows.h>
#include <winbase.h>
#elif defined(Q_OS_MACX)
#include <unistd.h>
#endif


PackageHandler::PackageHandler( QObject *parent, char* name )
    : QObject( parent, name ), packageChannel( 0 ), currentProcess( 0 ), mNoSpaceLeft( FALSE )
{
    // setup qcop channel
#ifndef QT_NO_COP
    packageChannel = new QCopChannel( "QPE/Package", this );
    connect( packageChannel, SIGNAL( received(const QCString&,const QByteArray&) ),
	     this, SLOT( qcopMessage(const QCString&,const QByteArray&) ) );
#endif
}

void PackageHandler::qcopMessage( const QCString &msg, const QByteArray &data )
{
    QDataStream stream( data, IO_ReadOnly );

    if ( msg == "installPackage(QString)" ) {
        QString file;
        stream >> file;
        installPackage( file );
    } else if ( msg == "removePackage(QString)" ) {
	QString file;
        stream >> file;
        removePackage( file );
    } else if ( msg == "addPackageFiles(QString,QString)" ) {
	QString location, listfile;
        stream >> location >> listfile;
        addPackageFiles( location, listfile);
    } else if ( msg == "addPackages(QString)" ) {
	QString location;
        stream >> location;
        addPackages( location );
    } else if ( msg == "cleanupPackageFiles(QString)" ) {
	QString listfile;
        stream >> listfile;
	cleanupPackageFiles( listfile );
    } else if ( msg == "cleanupPackages(QString)" ) {
	QString location;
        stream >> location;
        cleanupPackages( location );
    } else if ( msg == "prepareInstall(QString,QString)" ) {
	QString size, path;
	stream >> size;
	stream >> path;
	prepareInstall( size, path );
    }
}

void PackageHandler::installPackage( const QString &package )
{
    if ( mNoSpaceLeft ) {
	mNoSpaceLeft = FALSE;
	// Don't emit that for now, I still couldn't test it (Wener)
	//sendReply( "installFailed(QString)", package );
	//return;
    }
    
    currentProcess = new QProcess( QStringList() << "ipkg" << "install" << package ); // No tr
    connect( currentProcess, SIGNAL( processExited() ), SLOT( iProcessExited() ) );
    connect( currentProcess, SIGNAL( readyReadStdout() ), SLOT( readyReadStdout() ) );
    connect( currentProcess, SIGNAL( readyReadStderr() ), SLOT( readyReadStderr() ) );
    currentPackage = package;

    currentProcessError="";
    sendReply( "installStarted(QString)", package );
    currentProcess->start();
}

void PackageHandler::removePackage( const QString &package )
{
    currentProcess = new QProcess( QStringList() << "ipkg" << "remove" << package ); // No tr
    connect( currentProcess, SIGNAL( processExited() ), SLOT( rmProcessExited() ) );
    connect( currentProcess, SIGNAL( readyReadStdout() ), SLOT( readyReadStdout() ) );
    connect( currentProcess, SIGNAL( readyReadStderr() ), SLOT( readyReadStderr() ) );
    currentPackage = package;

    currentProcessError="";
    sendReply( "removeStarted(QString)", package );
    currentProcess->start();
}

void PackageHandler::sendReply( const QCString& msg, const QString& arg )
{
#ifndef QT_NO_COP
    QCopEnvelope e( "QPE/Desktop", msg );
    e << arg;
#endif
}

void PackageHandler::addPackageFiles( const QString &location, 
				      const QString &listfile )
{
    QFile f(listfile);
#ifndef Q_OS_WIN32
    //make a copy so we can remove the symlinks later
    mkdir( ("/usr/lib/ipkg/info/"+location).ascii(), 0777 );
    system(("cp " + f.name() + " /usr/lib/ipkg/info/"+location).ascii());
#else
    QDir d;
    //#### revise 
    qDebug("Copy file at %s: %s",  __FILE__, __LINE__ );
    d.mkdir(("/usr/lib/ipkg/info/" + location).ascii());   
    system(("copy " + f.name() + " /usr/lib/ipkg/info/"+location).ascii());
#endif

	    
    if ( f.open(IO_ReadOnly) ) {   
	QTextStream ts(&f);

	QString s;
	while ( !ts.eof() ) {        // until end of file...
	    s = ts.readLine();       // line of text excluding '\n'
	    // for s, do link/mkdir.
	    if ( s.right(1) == "/" ) {
		qDebug("do mkdir for %s", s.ascii());
#ifndef Q_OS_WIN32
		mkdir( s.ascii(), 0777 );
		//possible optimization: symlink directories
		//that don't exist already. -- Risky.
#else
		d.mkdir( s.ascii());
#endif

	    } else {
#ifndef Q_OS_WIN32
		qDebug("do symlink for %s", s.ascii());
		symlink( (location + s).ascii(), s.ascii() );
#else
		qDebug("Copy file instead of a symlink for WIN32");
		if (!CopyFile((TCHAR*)qt_winTchar((location + s), TRUE), (TCHAR*)qt_winTchar(s, TRUE), FALSE))
		  qWarning("Unable to create symlinkfor %s", 
			   (location + s).ascii());
#endif
	    }
	}
	f.close();
    } 
}

void PackageHandler::addPackages( const QString &location )
{
    // get list of *.list in location/usr/lib/ipkg/info/*.list
    QDir dir(location + "/usr/lib/ipkg/info", "*.list",  // No tr
	     QDir::Name, QDir::Files);
    if ( !dir.exists() )
	return;
    
    QStringList packages = dir.entryList();
    for ( QStringList::Iterator it = packages.begin();
	  it != packages.end(); ++it ) {
	addPackageFiles( location, *it );
    }
}


void PackageHandler::cleanupPackageFiles( const QString &listfile  )
{
    QFile f(listfile);
	    
    if ( f.open(IO_ReadOnly) ) {   
	QTextStream ts(&f);

	QString s;
	while ( !ts.eof() ) {        // until end of file...
	    s = ts.readLine();       // line of text excluding '\n'
	    // for s, do link/mkdir.
	    if ( s.right(1) == "/" ) {
		//should rmdir if empty, after all files have been removed
	    } else {
#ifndef Q_OS_WIN32
		qDebug("remove symlink for %s", s.ascii());
		//check if it is a symlink first (don't remove /etc/passwd...)
		char buf[10]; //we don't care about the contents
		if ( ::readlink( s.ascii(),buf, 10 >= 0 ) )
		     ::unlink( s.ascii() );
#else		
		// ### revise
		qWarning("Unable to remove symlink %s:%s", __FILE__, __LINE__);
#endif
	    }
	}
	f.close();

        //remove the list file
	::unlink( listfile.ascii() );
    
    }   
}

void PackageHandler::cleanupPackages( const QString &location )
{
    // get list of *.list in location/usr/lib/ipkg/info/*.list
    QDir dir( "/usr/lib/ipkg/info/"+location, "*.list",  // No tr
	      QDir::Name, QDir::Files);
    if ( !dir.exists() )
	return;
    
    QStringList packages = dir.entryList();
    for ( QStringList::Iterator it = packages.begin();
	  it != packages.end(); ++it ) {
	cleanupPackageFiles( *it );
    }
    
    //remove the backup directory 
    //###
}

void PackageHandler::prepareInstall( const QString& size, const QString& path )
{
    // Check whether there will be enough space to install the next package.
    bool ok;
    unsigned int s = size.toUInt( &ok );
    
    if ( !ok )
	return;

    // Shamelessly stolen from the sysinfo application (Werner)
#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
    struct statfs fs;
    if ( statfs( path.latin1(), &fs ) == 0 )
	if ( s > fs.f_bsize * fs.f_bavail ) {
	    //qDebug("############### Not enough space left ###############");
	    mNoSpaceLeft = TRUE;
	}
#endif
}

void PackageHandler::iProcessExited()
{
    if ( currentProcess->normalExit() && currentProcess->exitStatus() == 0 )
	sendReply( "installDone(QString)", currentPackage );
    else {
#ifndef QT_NO_COP
	QCopEnvelope e( "QPE/Desktop", "installFailed(QString,int,QString)" );
	e << currentPackage << currentProcess->exitStatus()
	    << currentProcessError;
#endif
    }
    
    delete currentProcess;
    currentProcess = 0;

#ifndef QT_NO_COP
    QCopEnvelope e("QPE/System", "linkChanged(QString)");
    QString lf = QString::null;
    e << lf;
#endif
    unlink( currentPackage );
}

void PackageHandler::rmProcessExited()
{
    if ( currentProcess->normalExit() && currentProcess->exitStatus() == 0 )
	sendReply( "removeDone(QString)", currentPackage );
    else
	sendReply( "removeFailed(QString)", currentPackage );

#ifndef QT_NO_COP
    QCopEnvelope e("QPE/System", "linkChanged(QString)");
    QString lf = QString::null;
    e << lf;
#endif
}

void PackageHandler::readyReadStdout()
{
    while ( currentProcess->canReadLineStdout() ) {
	QString line = currentProcess->readLineStdout();
	currentProcessError.append("OUT:"+line);
	if ( line.contains( "Unpacking" ) ) // No tr
	    sendReply( "installStep(QString)", "one" ); // No tr
	else if ( line.contains( "Configuring" ) ) // No tr
	    sendReply( "installStep(QString)", "two" ); // No tr
    }
}

void PackageHandler::readyReadStderr()
{
    while ( currentProcess->canReadLineStderr() ) {
	QString line = currentProcess->readLineStderr();
	currentProcessError.append("ERR:"+line);
    }
}

void PackageHandler::redoPackages()
{
    //get list of filesystems

    //call cleanupPackages for the ones that have disappeared

    //call addPackageFiles for the new ones
}
