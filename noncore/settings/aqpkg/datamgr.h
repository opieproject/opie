/***************************************************************************
                          datamgr.h  -  description
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

#ifndef DATAMGR_H
#define DATAMGR_H

#include <map>
using namespace std;

#include <qstring.h>

#include "server.h"
#include "destination.h"

#define LOCAL_SERVER			"Installed Pkgs"
#define LOCAL_IPKGS				"local IPKG"

/**
  *@author Andy Qua
  */


class DataManager
{
public:
    DataManager();
    ~DataManager();

    void setActiveServer( const QString &act )      { activeServer = act; }
    QString &getActiveServer( )      				{ return activeServer; }

    Server *getLocalServer()    					{ return &( *getServer( LOCAL_SERVER ) ); }
    vector<Server> &getServerList()   				{ return serverList; }
    vector<Server>::iterator getServer( const char *name );

    vector<Destination> &getDestinationList()  		{ return destList; }
    vector<Destination>::iterator getDestination( const char *name );

    void loadServers();
    void reloadServerData( );

    void writeOutIpkgConf();

    static QString getAvailableCategories() { return availableCategories; }
    static void setAvailableCategories( QString section );

    QString getHttpProxy()                          { return httpProxy; }
    QString getFtpProxy()                           { return ftpProxy; }
    QString getProxyUsername()                      { return proxyUsername; }
    QString getProxyPassword()                      { return proxyPassword; }

    bool getHttpProxyEnabled()                      { return httpProxyEnabled; }
    bool getFtpProxyEnabled()                       { return ftpProxyEnabled; }

    void setHttpProxy( QString proxy )              { httpProxy = proxy; }
    void setFtpProxy( QString proxy )               { ftpProxy = proxy; }
    void setProxyUsername( QString name )           { proxyUsername = name; }
    void setProxyPassword( QString pword )          { proxyPassword = pword; }

    void setHttpProxyEnabled( bool val )            { httpProxyEnabled = val; }
    void setFtpProxyEnabled( bool val )            { ftpProxyEnabled = val; }
private:
    static QString availableCategories;
    QString activeServer;
    QString httpProxy;
    QString ftpProxy;
    QString proxyUsername;
    QString proxyPassword;

    bool httpProxyEnabled;
    bool ftpProxyEnabled;

    vector<Server> serverList;
    vector<Destination> destList;
};

#endif
