/*
                             This file is part of the OPIE Project

               =.            Copyright (c)  2002 Andy Qua <andy.qua@blueyonder.co.uk>
             .=l.                                Dan Williams <drw@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

#include <stdio.h>
#include <unistd.h>

#ifdef QWS
#include <qpe/qpeapplication.h>
#else
#include <qapplication.h>
#endif
#include <qdir.h>
#include <qtextstream.h>

#include <opie/oprocess.h>

#include "utils.h"
#include "ipkg.h"
#include "global.h"

Ipkg :: Ipkg()
{
    proc = 0;
}

Ipkg :: ~Ipkg()
{
}

// Option is what we are going to do - install, upgrade, download, reinstall
// package is the package name to install - either a fully qualified path and ipk
//   file (if stored locally) or just the name of the package (for a network package)
// packageName is the package name - (for a network package this will be the same as
//    package parameter)
// dest is the destination alias (from ipk.conf)
// destDir is the dir that the destination alias points to (used to link to root)
// flags is the ipkg options flags
// dir is the directory to run ipkg in (defaults to "")
void Ipkg :: runIpkg()
{
    error = false;
    QStringList commands;

    QDir::setCurrent( "/tmp" );

    if ( runtimeDir != "" )
    {
        commands << "cd ";
        commands << runtimeDir;
        commands << ";";
    }
    commands << "ipkg" << "-V" << QString::number( infoLevel ) << "-force-defaults";

    // only set the destination for an install operation
    if ( option == "install" )
        commands << "-dest" << destination;


    if ( option != "update" && option != "download" )
    {
        if ( flags & FORCE_DEPENDS )
            commands << "-force-depends";
        if ( flags & FORCE_REINSTALL  )
            commands << "-force-reinstall";
        if ( flags & FORCE_REMOVE )
            commands << "-force-removal-of-essential-packages";
        if ( flags & FORCE_OVERWRITE )
            commands << "-force-overwrite";
        if ( infoLevel == 3 )
            commands << "-verbose_wget";

        // Handle make links
        // Rules - If make links is switched on, create links to root
        // if destDir is NOT /
        if ( flags & MAKE_LINKS )
        {
            // If destDir == / turn off make links as package is being insalled
            // to root already.
            if ( destDir == "/" )
                flags ^= MAKE_LINKS;
        }
    }

#ifdef X86
    commands << "-f";
    commands << IPKG_CONF;
#endif


    if ( option == "reinstall" )
        commands << "install";
    else
        commands << option;
    if ( package != "" )
        commands << package;


    if ( package != "" )
        emit outputText( tr( "Dealing with package %1" ).arg( package) );

    qApp->processEvents();

    // If we are removing, reinstalling or upgrading packages and make links option is selected
    // create the links
    if ( option == "remove" || option == "reinstall" || option == "upgrade" )
    {
        createLinks = false;
        if ( flags & MAKE_LINKS )
        {
            emit outputText( tr( "Removing symbolic links...\n" ) );
            linkPackage( Utils::getPackageNameFromIpkFilename( package ), destination, destDir );
            emit outputText( QString( " " ) );
        }
    }

    // Execute command
    dependantPackages = new QList<QString>;
    dependantPackages->setAutoDelete( true );

    executeIpkgCommand( commands, option );

}

void Ipkg :: createSymLinks()
{
    if ( option == "install" || option == "reinstall" || option == "upgrade" )
    {
        // If we are not removing packages and make links option is selected
        // create the links
        createLinks = true;
        if ( flags & MAKE_LINKS )
        {
            emit outputText( " " );
            emit outputText( tr( "Creating symbolic links for %1." ).arg( package) );

            linkPackage( Utils::getPackageNameFromIpkFilename( package ), destination, destDir );

            // link dependant packages that were installed with this release
            QString *pkg;
            for ( pkg = dependantPackages->first(); pkg != 0; pkg = dependantPackages->next() )
            {
                if ( *pkg == package )
                    continue;
                emit outputText( " " );
                emit outputText( tr( "Creating symbolic links for %1" ).arg( *pkg ) );
                linkPackage( Utils::getPackageNameFromIpkFilename( *pkg ), destination, destDir );
            }
        }
    }

    delete dependantPackages;

    emit outputText( tr("Finished") );
    emit outputText( "" );
}

void Ipkg :: removeStatusEntry()
{
    QString statusFile = destDir;
    if ( statusFile.right( 1 ) != "/" )
        statusFile.append( "/" );
    statusFile.append( "usr/lib/ipkg/status" );
    QString outStatusFile = statusFile;
    outStatusFile.append( ".tmp" );

    emit outputText( "" );
    emit outputText( tr("Removing status entry...") );
    QString tempstr = tr("status file - ");
    tempstr.append( statusFile );
    emit outputText( tempstr );
    tempstr = tr("package - ");
    tempstr.append( package );
    emit outputText( tempstr );

    ifstream in( statusFile );
    ofstream out( outStatusFile );
    if ( !in.is_open() )
    {
        tempstr = tr("Couldn't open status file - ");
        tempstr.append( statusFile );
        emit outputText( tempstr );
        return;
    }

    if ( !out.is_open() )
    {
        tempstr = tr("Couldn't create tempory status file - ");
        tempstr.append( outStatusFile );
        emit outputText( tempstr );
        return;
    }

    char line[1001];
    char k[21];
    char v[1001];
    QString key;
    QString value;
    vector<QString> lines;
    int i = 0;
    do
    {
        in.getline( line, 1000 );
        if ( in.eof() )
            continue;

        k[0] = '\0';
        v[0] = '\0';

        sscanf( line, "%[^:]: %[^\n]", k, v );
        key = k;
        value = v;
        key = key.stripWhiteSpace();
        value = value.stripWhiteSpace();
        if ( key == "Package" && value == package )
        {
            // Ignore all lines up to next empty
            do
            {
                in.getline( line, 1000 );
                if ( in.eof() || QString( line ).stripWhiteSpace() == "" )
                    continue;
            } while ( !in.eof() && QString( line ).stripWhiteSpace() != "" );
        }

        lines.push_back( QString( line ) );
        out << line << endl;

        // Improve UI responsiveness
        i++;
        if ( ( i % 50 ) == 0 )
            qApp->processEvents();
    } while ( !in.eof() );

    // Write lines out
    vector<QString>::iterator it;
    for ( it = lines.begin() ; it != lines.end() ; ++it )
    {
        out << (const char *)(*it) << endl;

        // Improve UI responsiveness
        i++;
        if ( ( i % 50 ) == 0 )
            qApp->processEvents();
    }

    in.close();
    out.close();

    // Remove old status file and put tmp stats file in its place
    remove( statusFile );
    rename( outStatusFile, statusFile );
}

int Ipkg :: executeIpkgCommand( QStringList &cmd, const QString /*option*/ )
{
    // If one is already running - should never be but just to be safe
    if ( proc )
    {
        delete proc;
        proc = 0;
    }

    // OK we're gonna use OProcess to run this thing
    proc = new OProcess();
    aborted = false;


    // Connect up our slots
    connect(proc, SIGNAL(processExited(OProcess *)),
            this, SLOT( processFinished()));

    connect(proc, SIGNAL(receivedStdout(OProcess *, char *, int)),
            this, SLOT(commandStdout(OProcess *, char *, int)));

    connect(proc, SIGNAL(receivedStderr(OProcess *, char *, int)),
            this, SLOT(commandStderr(OProcess *, char *, int)));

    for ( QStringList::Iterator it = cmd.begin(); it != cmd.end(); ++it )
    {
         *proc << (*it).latin1();
    }

    // Start the process going
    finished = false;
    if(!proc->start(OProcess::NotifyOnExit, OProcess::All))
    {
        emit outputText( tr("Couldn't start ipkg process" ) );
    }
}

void Ipkg::commandStdout(OProcess*, char *buffer, int buflen)
{
    QString lineStr = buffer;
    if ( lineStr[buflen-1] == '\n' )
        buflen --;
    lineStr = lineStr.left( buflen );
    emit outputText( lineStr );

    // check if we are installing dependant packages
    if ( option == "install" || option == "reinstall" )
    {
        // Need to keep track of any dependant packages that get installed
        // so that we can create links to them as necessary
        if ( lineStr.startsWith( "Installing " ) )
        {
            int start = lineStr.find( " " ) + 1;
            int end = lineStr.find( " ", start );
            QString *package = new QString( lineStr.mid( start, end-start ) );
            dependantPackages->append( package );
        }
    }
    else if ( option == "remove" && !( flags & FORCE_DEPENDS ) &&
              lineStr.find( "is depended upon by packages:" ) != -1 )
    {
        // Ipkg should send this to STDERR, but doesn't - so trap here
        error = true;
    }

    buffer[0] = '\0';
}

void Ipkg::commandStderr(OProcess*, char *buffer, int buflen)
{
    QString lineStr = buffer;
    if ( lineStr[buflen-1] == '\n' )
        buflen --;
    lineStr=lineStr.left( buflen );
    emit outputText( lineStr );
    buffer[0] = '\0';
    error = true;
}

void Ipkg::processFinished()
{
    // Finally, if we are removing a package, remove its entry from the <destdir>/usr/lib/ipkg/status file
    // to workaround an ipkg bug which stops reinstall to a different location

    if ( !error && option == "remove" )
        removeStatusEntry();

    delete proc;
    proc = 0;
    finished = true;

	emit ipkgFinished();
}


void Ipkg :: abort()
{
    if ( proc )
    {
        proc->kill();
        aborted = true;
    }
}

void Ipkg :: linkPackage( const QString &packFileName, const QString &dest, const QString &destDir )
{
    if ( dest == "root" || dest == "/" )
        return;

    qApp->processEvents();
    QStringList *fileList = getList( packFileName, destDir );
    qApp->processEvents();
    processFileList( fileList, destDir );
    delete fileList;
}

QStringList* Ipkg :: getList( const QString &packageFilename, const QString &destDir )
{
    QString packageFileDir = destDir;
    packageFileDir.append( "/usr/lib/ipkg/info/" );
    packageFileDir.append( packageFilename );
    packageFileDir.append( ".list" );
    QFile f( packageFileDir );

    if ( !f.open(IO_ReadOnly) )
    {
        // Couldn't open from dest, try from /
        f.close();

        packageFileDir = "/usr/lib/ipkg/info/";
        packageFileDir.append( packageFilename );
        packageFileDir.append( ".list" );
        f.setName( packageFileDir );
        if ( ! f.open(IO_ReadOnly) )
        {
            QString tempstr = tr("Could not open :");
            tempstr.append( packageFileDir );
            emit outputText( tempstr );
            return (QStringList*)0;
        }
    }
    QStringList *fileList = new QStringList();
    QTextStream t( &f );
    while ( !t.eof() )
        *fileList += t.readLine();

    f.close();
    return fileList;
}

void Ipkg :: processFileList( const QStringList *fileList, const QString &destDir )
{
    if ( !fileList || fileList->isEmpty() )
        return;

    QString baseDir = ROOT;

    if ( createLinks == true )
    {
        for ( uint i=0; i < fileList->count(); i++ )
        {
            processLinkDir( (*fileList)[i], baseDir, destDir );
            qApp->processEvents();
        }
    }
    else
    {
        for ( int i = fileList->count()-1; i >= 0 ; i-- )
        {
            processLinkDir( (*fileList)[i], baseDir, destDir );
            qApp->processEvents();
        }
    }
}

void Ipkg :: processLinkDir( const QString &file, const QString &destDir, const QString &baseDir )
{

    QString sourceFile = baseDir;
    sourceFile.append( file );

    QString linkFile = destDir;
    if ( file.startsWith( "/" ) && destDir.right( 1 ) == "/" )
    {
        linkFile.append( file.mid( 1 ) );
    }
    else
    {
        linkFile.append( file );
    }
    QString text;
    if ( createLinks )
    {
        // If this file is a directory (ends with a /) and it doesn't exist,
        // we need to create it
        if ( file.right(1) == "/" )
        {
            QFileInfo f( linkFile );
            if ( !f.exists() )
            {
                QString tempstr = tr("Creating directory ");
                tempstr.append( linkFile );
                emit outputText( tempstr );
                QDir d;
                d.mkdir( linkFile, true );
            }
//            else
//                emit outputText( QString( "Directory " ) + linkFile + " already exists" );

        }
        else
        {
            int rc = symlink( sourceFile, linkFile );
            text = ( rc == 0 ? tr( "Linked %1 to %2" ) : tr( "Failed to link %1 to %2" ) ).
                    arg( sourceFile ).
                    arg( linkFile );
            emit outputText( text );
        }
    }
    else
    {
        QFileInfo f( linkFile );
        if ( f.exists() )
        {
            if ( f.isFile() )
            {
                QFile f( linkFile );
                bool rc = f.remove();

                text = ( rc ? tr( "Removed %1" ) : tr( "Failed to remove %1" ) ).arg( linkFile );
                emit outputText( text );
            }
            else if ( f.isDir() )
            {
                QDir d;
                bool rc = d.rmdir( linkFile, true );
                if ( rc )
                {
                    text = ( rc ? tr( "Removed " ) : tr( "Failed to remove " ) ).arg( linkFile );
                    emit outputText( text );
                }
            }
        }
    }
}
