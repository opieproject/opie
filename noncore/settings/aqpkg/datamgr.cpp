/***************************************************************************
                          datamgr.cpp  -  description
                             -------------------
    begin                : Thu Aug 29 2002
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
#include <iostream>
using namespace std;

#ifdef QWS
#include <qpe/config.h>
#endif

#include <stdio.h>

#include "datamgr.h"
#include "global.h"


DataManager::DataManager()
{
	activeServer = "";
}

DataManager::~DataManager()
{
}

Server *DataManager :: getServer( const char *name )
{
	Server *s = 0;
	vector<Server>::iterator it = serverList.begin();
    while ( it != serverList.end() && s == 0 )
    {
		if ( it->getServerName() == name )
			s = &(*it);

		++it;
	}

	return s;
}

Destination *DataManager :: getDestination( const char *name )
{
	Destination *d = 0;
	vector<Destination>::iterator it = destList.begin();
    while ( it != destList.end() && d == 0 )
    {
		if ( it->getDestinationName() == name )
			d = &(*it);

		++it;
	}

	return d;
}

void DataManager :: loadServers()
{
    // First add our local server - not really a server but
    // the local config (which packages are installed)
    serverList.push_back( Server( LOCAL_SERVER, "" ) );
    serverList.push_back( Server( LOCAL_IPKGS, "" ) );

#ifdef QWS
    Config cfg( "aqpkg" );
    cfg.setGroup( "destinations" );
#endif

    // Read file from /etc/ipkg.conf
    QString ipkg_conf = IPKG_CONF;
    FILE *fp;
    fp = fopen( ipkg_conf, "r" );
    char line[130];
    QString lineStr;
    if ( fp == NULL )
    {
        cout << "Couldn't open " << ipkg_conf << "! err = " << fp << endl;
        return;
    }
    else
    {
        while ( fgets( line, sizeof line, fp) != NULL )
        {
            lineStr = line;
            if ( lineStr.startsWith( "src" ) || lineStr.startsWith( "#src" ) || lineStr.startsWith( "# src" ) )
            {
                char alias[20];
                char url[100];


                // Looks a little wierd but read up to the r of src (throwing it away),
                // then read up to the next space and throw that away, the alias
                // is next.
                // Should Handle #src, # src, src, and combinations of
                sscanf( lineStr, "%*[^r]%*[^ ] %s %s", alias, url );
                Server s( alias, url );
                if ( lineStr.startsWith( "src" ) )
               		s.setActive( true );
                else
               		s.setActive( false );

                serverList.push_back( s );

            }
            else if ( lineStr.startsWith( "dest" ) )
            {
                char alias[20];
                char path[50];
                sscanf( lineStr, "%*[^ ] %s %s", alias, path );
                Destination d( alias, path );
                bool linkToRoot = true;
#ifdef QWS
                QString key = alias;
                key += "_linkToRoot";
                linkToRoot = cfg.readBoolEntry( key, true );
#endif
                d.linkToRoot( linkToRoot );
                
                destList.push_back( d );
            }
        }
    }
    fclose( fp );

    reloadServerData( );
}

void DataManager :: reloadServerData( )
{
	vector<Server>::iterator it = serverList.begin();
    for ( it = serverList.begin() ; it != serverList.end() ; ++it )
    {
    	// Now we've read the config file in we need to read the servers
    	// The local server is a special case. This holds the contents of the
    	// status files the number of which depends on how many destinations
    	// we've set up
    	// The other servers files hold the contents of the server package list
    	if ( it->getServerName() == LOCAL_SERVER )
        	it->readStatusFile( destList );
    	else if ( it->getServerName() == LOCAL_IPKGS )
    		it->readLocalIpks( getServer( LOCAL_SERVER ) );
    	else
        	it->readPackageFile( getServer( LOCAL_SERVER ) );
	}
}

void DataManager :: writeOutIpkgConf()
{
    QString ipkg_conf = IPKG_CONF;
    ofstream out( ipkg_conf );

    out << "# Written by AQPkg" << endl;
    out << "# Must have one or more source entries of the form:" << endl;
    out << "#" << endl;
    out << "#   src <src-name> <source-url>" << endl;
    out << "#" << endl;
    out << "# and one or more destination entries of the form:" << endl;
    out << "#" << endl;
    out << "#   dest <dest-name> <target-path>" << endl;
    out << "#" << endl;
    out << "# where <src-name> and <dest-names> are identifiers that" << endl;
    out << "# should match [a-zA-Z0-9._-]+, <source-url> should be a" << endl;
    out << "# URL that points to a directory containing a Familiar" << endl;
    out << "# Packages file, and <target-path> should be a directory" << endl;
    out << "# that exists on the target system." << endl << endl;

    // Write out servers
    vector<Server>::iterator it = serverList.begin();
    while ( it != serverList.end() )
    {
        QString alias = it->getServerName();
        // Don't write out local as its a dummy
        if ( alias != LOCAL_SERVER && alias != LOCAL_IPKGS )
        {
            QString url = it->getServerUrl();;

            if ( !it->isServerActive() )
                out << "#";
            out << "src " << alias << " " << url << endl;
        }

        it++;
    }

    out << endl;

    // Write out destinations
    vector<Destination>::iterator it2 = destList.begin();
    while ( it2 != destList.end() )
    {
        out << "dest " << it2->getDestinationName() << " " << it2->getDestinationPath() << endl;
        it2++;
    }

    out.close();
}
