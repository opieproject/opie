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
            if ( lineStr.startsWith( "src" ) || lineStr.startsWith( "#src" ) )
            {
                char alias[20];
                char url[100];
                sscanf( lineStr, "%*[^ ] %s %s", alias, url );
                Server s( alias, url );
                serverList.push_back( s );

                if ( lineStr.startsWith( "src" ) )
               		setActiveServer( alias );
            }
            else if ( lineStr.startsWith( "dest" ) )
            {
                char alias[20];
                char path[50];
                sscanf( lineStr, "%*[^ ] %s %s", alias, path );
                Destination d( alias, path );
                destList.push_back( d );
            }
        }
    }
    fclose( fp );

    // Go through the server destination list and add root, cf and card if they
    // don't already exist
	/* AQ - commented out as if you don't have a CF or SD card in then
	 * this causes IPKG to try to create directories on non existant devices
	 * (which of course fails), gives a nasty error message and can cause ipkg
	 * to seg fault.
	 *
    vector<Destination>::iterator dit;
    bool foundRoot = false;
    bool foundCF = false;
    bool foundCard = false;
    for ( dit = destList.begin() ; dit != destList.end() ; ++dit )
    {
        if ( dit->getDestinationPath() == "/" )
            foundRoot = true;
        if ( dit->getDestinationPath() == "/mnt/cf" )
            foundCF = true;
        if ( dit->getDestinationPath() == "/mnt/card" )
            foundCard = true;
    }

    // If running on a Zaurus (arm) then if we didn't find root, CF or card
    // destinations, add them as default
#ifdef QWS
#ifndef X86
    if ( !foundRoot )
        destList.push_back( Destination( "root", "/" ) );
    if ( !foundCF )
        destList.push_back( Destination( "cf", "/mnt/cf" ) );
    if ( !foundCF )
        destList.push_back( Destination( "card", "/mnt/card" ) );
#endif
#endif
*/    
    vector<Server>::iterator it;
    for ( it = serverList.begin() ; it != serverList.end() ; ++it )
        reloadServerData( it->getServerName() );
}

void DataManager :: reloadServerData( const char *serverName )
{
    Server *s = getServer( serverName );
    // Now we've read the config file in we need to read the servers
    // The local server is a special case. This holds the contents of the
    // status files the number of which depends on how many destinations
    // we've set up
    // The other servers files hold the contents of the server package list
    if ( s->getServerName() == LOCAL_SERVER )
        s->readStatusFile( destList );
    else if ( s->getServerName() == LOCAL_IPKGS )
    	s->readLocalIpks( getServer( LOCAL_SERVER ) );
    else
        s->readPackageFile( getServer( LOCAL_SERVER ) );

}

void DataManager :: writeOutIpkgConf()
{
    QString ipkg_conf = IPKG_CONF;
    ofstream out( ipkg_conf );

    out << "# Written by NetworkPackageManager Package Manager" << endl;

    // Write out servers
    vector<Server>::iterator it = serverList.begin();
    while ( it != serverList.end() )
    {
        QString alias = it->getServerName();
        // Don't write out local as its a dummy
        if ( alias != LOCAL_SERVER && alias != LOCAL_IPKGS )
        {
            QString url = it->getServerUrl();;

            if ( !activeServer || alias != activeServer )
                out << "#";
            out << "src " << alias << " " << url << endl;
        }

        it++;
    }

    // Write out destinations
    vector<Destination>::iterator it2 = destList.begin();
    while ( it2 != destList.end() )
    {
        out << "dest " << it2->getDestinationName() << " " << it2->getDestinationPath() << endl;
        it2++;
    }

    out.close();
}
