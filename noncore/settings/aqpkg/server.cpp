/***************************************************************************
                          server.cpp  -  description
                             -------------------
    begin                : Mon Aug 26 2002
    copyright            : (C) 2002 by Andy Qua
    email                : andy.qua@blueyonder.co.uk
    description          : This class holds details about a server
                         : e.g. all the packages that contained on the server
                         :      the installation status
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
using namespace std;

#include "server.h"
#include "datamgr.h"

#ifdef QWS
#include <qpe/global.h>
#include <qpe/applnk.h>
#include <qlist.h>
#endif

#include "utils.h"

#include "global.h"

Server :: Server( const char *name, const char *url )
{
    serverName = name;
    serverUrl = url;
    packageFile = IPKG_DIR;
    packageFile += "lists/" + serverName;
}

Server :: ~Server()
{
	cleanUp();
}

void Server :: cleanUp()
{
    packageList.clear();
}

void Server :: readStatusFile(  vector<Destination> &destList )
{
    cleanUp();

    vector<Destination>::iterator dit;
    bool rootRead = false;
    for ( dit = destList.begin() ; dit != destList.end() ; ++dit )
    {
        bool installingToRoot = false;
        
        QString path = dit->getDestinationPath();
        if ( path.right( 1 ) != "/" )
            path += "/";
            
        if ( path == "/" )
        {
            rootRead = true;
            installingToRoot = true;
        }   
        
        packageFile = path + "usr/lib/ipkg/status";
        readPackageFile( 0, false, installingToRoot, dit );
    }

    // Ensure that the root status file is read
    if ( !rootRead )
    {
        cout << "Reading status file " << "/usr/lib/ipkg/status" << endl;
        packageFile = "/usr/lib/ipkg/status";
        readPackageFile( 0, false, true );
    }
}

void Server :: readLocalIpks( Server *local )
{
    cleanUp();

#ifdef QWS
    // First, get any local IPKGs in the documents area
    // Only applicable to Qtopie/Opie

	DocLnkSet files;
	Global::findDocuments( &files, "application/ipkg" );

	// Now add the items to the list
	QListIterator<DocLnk> it( files.children() );

	for ( ; it.current() ; ++it )
	{
        // OK, we have a local IPK file, I think the standard naming conventions
        // for these are packagename_version_arm.ipk
        QString file = (*it)->file();

        // Changed to display the filename (excluding the path)
        QString packageName = Utils::getFilenameFromIpkFilename( file );
        QString ver = Utils::getPackageVersionFromIpkFilename( file );
        packageList.push_back( Package( packageName ) );
        packageList.back().setVersion( ver );
        packageList.back().setFilename( file );
        packageList.back().setPackageStoredLocally( true );
        
	}
#else
	QString names[] = { "advancedfm_0.9.1-20020811_arm.ipk", "libopie_0.9.1-20020811_arm.ipk", "libopieobex_0.9.1-20020811.1_arm.ipk", "opie-addressbook_0.9.1-20020811_arm.ipk" };
	for ( int i = 0 ; i < 4 ; ++i )
	{
		// OK, we have a local IPK file, I think the standard naming conventions
		// for these are packagename_version_arm.ipk
		QString file = names[i];
		int p = file.find( "_" );
		QString tmp = file.mid( 0, p );
		packageList.push_back( Package( tmp ) );
		int p2 = file.find( "_", p+1 );
		tmp = file.mid( p+1, p2-(p+1) );
		packageList.back().setVersion( tmp );
		packageList.back().setPackageStoredLocally( true );
	}
#endif

    // build local packages
    buildLocalPackages( local );
}

void Server :: readPackageFile( Server *local, bool clearAll, bool installingToRoot, Destination *dest )
{
    ifstream in( packageFile );
	if ( !in.is_open() )
        return;

    char line[1001];
    char k[21];
    char v[1001];
    QString key;
    QString value;

    if ( clearAll )
        cleanUp();
    Package *currPackage = 0;

    bool newPackage = true;
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
        if ( key == "Package" && newPackage )
        {
            newPackage = false;

            currPackage = getPackage( value );
            if ( !currPackage )
            {
    			packageList.push_back( Package( value ) );
	    		currPackage = &(packageList.back());
                currPackage->setInstalledTo( dest );

                if ( installingToRoot )
                    currPackage->setInstalledToRoot( true );
            }
            else
            {
                if (currPackage->getStatus().find( "deinstall" ) != -1 )
                    currPackage->setInstalledTo( dest );
            }
        }
        else if ( key == "Version" )
        {
            if ( currPackage )
                currPackage->setVersion( value );
        }
        else if ( key == "Status" )
        {
            if ( currPackage )
                currPackage->setStatus( value );
        }
        else if ( key == "Description" )
        {
            if ( currPackage )
                currPackage->setDescription( value );
        }
        else if ( key == "Filename" )
        {
            if ( currPackage )
                currPackage->setFilename( value );
        }
        else if ( key == "Size" )
        {
            if ( currPackage )
                currPackage->setPackageSize( value );
        }
        else if ( key == "Section" )
        {
            if ( currPackage )
                currPackage->setSection( value );

            DataManager::setAvailableCategories( value );
        }
        else if ( key == "" )
        {
            newPackage = true;
        }
    } while ( !in.eof() );

    in.close();

    // build local packages
    buildLocalPackages( local );
}

void Server :: buildLocalPackages( Server *local )
{
    for ( unsigned int i = 0 ; i < packageList.size() ; ++i )
    {
		QString name = packageList[i].getPackageName();

        // If the package name is an ipk name, then convert the filename to a package name
        if ( name.find( ".ipk" ) != -1 )
            name = Utils::getPackageNameFromIpkFilename( packageList[i].getFilename() );

        if ( local )
        {
            Package *p = local->getPackage( name );
            packageList[i].setLocalPackage( p );
            if ( p )
            {
                // Set some default stuff like size and things
                if ( p->getInstalledVersion() == packageList[i].getVersion() )
                {
                    p->setPackageSize( packageList[i].getPackageSize() );
                    p->setSection( packageList[i].getSection() );
                    p->setDescription( packageList[i].getDescription() );
                }
            }
            
        }
        else
            packageList[i].setLocalPackage( 0 );
    }

}

Package *Server :: getPackage( QString &name )
{
    return getPackage( (const char *)name );
}

Package *Server :: getPackage( const char *name )
{
    Package *ret = 0;

    for ( unsigned int i = 0 ; i < packageList.size() && ret == 0; ++i )
    {
		if ( packageList[i].getPackageName() == name )
			ret = &packageList[i];
	}

    return ret;
}

QString Server :: toString()
{
    QString ret = "Server\n   name - " + serverName +
                       "\n   url - " + serverUrl +
                       "\n";

    for ( unsigned int i = 0 ; i < packageList.size() ; ++i )
        ret += "\n   " + packageList[i].toString();


    return ret;
}

vector<Package> &Server::getPackageList()
{
    return packageList;
}

