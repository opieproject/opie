/***************************************************************************
                          ipkg.cpp  -  description
                             -------------------
    begin                : Sat Aug 31 2002
    copyright            : (C) 2002 by Andy Qua
    email                : andy.qua@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <fstream>
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

#include "utils.h"
#include "ipkg.h"
#include "global.h"

Ipkg :: Ipkg()
{
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
bool Ipkg :: runIpkg( )
{
    bool ret = false;

    QDir::setCurrent( "/tmp" );
    QString cmd = "";

    if ( runtimeDir != "" )
    {
        cmd += "cd ";
        cmd += runtimeDir;
        cmd += " ; ";
    }
    cmd += "ipkg -force-defaults";
    if ( option != "update" && option != "download" )
    {
        cmd += " -dest "+ destination;

        if ( flags & FORCE_DEPENDS )
            cmd += " -force-depends";
        if ( flags & FORCE_REINSTALL  )
            cmd += " -force-reinstall";
        if ( flags & FORCE_REMOVE )
            cmd += " -force-removal-of-essential-packages";
        if ( flags & FORCE_OVERWRITE )
            cmd += " -force-overwrite";

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
    cmd += " -f ";
    cmd += IPKG_CONF;
#endif


    if ( option == "reinstall" )
        cmd += " install";
    else
        cmd += " " + option;
    if ( package != "" )
        cmd += " " + package;
    cmd += " 2>&1";


    if ( package != "" )
        emit outputText( QString( "Dealing with package " ) + package );

    qApp->processEvents();

    // If we are removing packages and make links option is selected
    // create the links
    if ( option == "remove" || option == "reinstall" )
    {
        createLinks = false;
        if ( flags & MAKE_LINKS )
        {
            emit outputText( QString( "Removing symbolic links...\n" ) );
            linkPackage( Utils::getPackageNameFromIpkFilename( package ), destination, destDir );
            emit outputText( QString( " " ) );
        }
    }
    
    emit outputText( cmd );

    // Execute command
    dependantPackages = new QList<QString>;
    dependantPackages->setAutoDelete( true );

    ret = executeIpkgCommand( cmd, option );

    if ( option == "install" || option == "reinstall" )
    {
        // If we are not removing packages and make links option is selected
        // create the links
        createLinks = true;
        if ( flags & MAKE_LINKS )
        {
            emit outputText( " " );
            emit outputText( QString( "Creating symbolic links for " )+ package );
            
            linkPackage( Utils::getPackageNameFromIpkFilename( package ), destination, destDir );

            // link dependant packages that were installed with this release
            QString *pkg;
            for ( pkg = dependantPackages->first(); pkg != 0; pkg = dependantPackages->next() )
            {
                if ( *pkg == package )
                    continue;
                emit outputText( " " );
                emit outputText( QString( "Creating symbolic links for " )+ (*pkg) );
                linkPackage( Utils::getPackageNameFromIpkFilename( *pkg ), destination, destDir );
            }
        }
    }

    delete dependantPackages;
    
//    emit outputText( QString( "Finished - status=" ) + (ret ? "success" : "failure") );
    emit outputText( "Finished" );
    emit outputText( "" );
    return ret;
}


int Ipkg :: executeIpkgCommand( QString &cmd, const QString option )
{
    FILE *fp = NULL;
    char line[130];
    QString lineStr, lineStrOld;
    int ret = false;

    fp = popen( (const char *) cmd, "r");
    if ( fp == NULL )
    {
        cout << "Couldn't execute " << cmd << "! err = " << fp << endl;
        QString text;
        text.sprintf( "Couldn't execute %s! See stdout for error code", (const char *)cmd );
        emit outputText( text );
    }
    else
    {
        while ( fgets( line, sizeof line, fp) != NULL )
        {
            lineStr = line;
            lineStr=lineStr.left( lineStr.length()-1 );

            if ( lineStr != lineStrOld )
            {
                //See if we're finished
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
                
                if ( option == "update" )
                {
                    if (lineStr.contains("Updated list"))
                        ret = true;
                }
                else if ( option == "download" )
                {
                    if (lineStr.contains("Downloaded"))
                        ret = true;
                }
                else
                {
                    if (lineStr.contains("Done"))
                        ret = true;
                }

                emit outputText( lineStr );
            }
            lineStrOld = lineStr;
            qApp->processEvents();
        }
        pclose(fp);
    }

    return ret;
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
    QString packageFileDir = destDir+"/usr/lib/ipkg/info/"+packageFilename+".list";
    QFile f( packageFileDir );

    cout << "Try to open " << packageFileDir << endl;
    if ( !f.open(IO_ReadOnly) )
    {
        // Couldn't open from dest, try from /
        cout << "Could not open:" << packageFileDir << endl;
        f.close();
        
        packageFileDir = "/usr/lib/ipkg/info/"+packageFilename+".list";
        f.setName( packageFileDir );
//        cout << "Try to open " << packageFileDir.latin1() << endl;
        if ( ! f.open(IO_ReadOnly) )
        {
            cout << "Could not open:" << packageFileDir << endl;
            emit outputText( QString( "Could not open :" ) + packageFileDir );
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

    QString sourceFile = baseDir + file;
    
    QString linkFile = destDir;
    if ( file.startsWith( "/" ) && destDir.right( 1 ) == "/" )
    {
        linkFile += file.mid( 1 );
    }   
    else
    {
        linkFile += file;
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
                emit outputText( QString( "Creating directory " ) + linkFile );
                QDir d;
                d.mkdir( linkFile, true );
            }
            else
                emit outputText( QString( "Directory " ) + linkFile + " exists" );
            
        }
        else
        {
            int rc = symlink( sourceFile, linkFile );
            text = (rc == 0 ? "Linked " : "Failed to link ");
            text += sourceFile + " to " + linkFile;
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

                text = (rc ? "Removed " : "Failed to remove ");
                text += linkFile;
                emit outputText( text );
            }
            else if ( f.isDir() )
            {
                QDir d;
                bool rc = d.rmdir( linkFile, true );
                text = (rc ? "Removed " : "Failed to remove ");
                text += linkFile;
                emit outputText( text );
            }
        }
    }
    
}
