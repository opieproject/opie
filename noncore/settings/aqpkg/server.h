/***************************************************************************
                          server.h  -  description
                             -------------------
    begin                : Mon Aug 26 2002
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
#ifndef SERVER_H
#define SERVER_H

#include <qstring.h>

#include <vector>
using namespace std;

#include "package.h"
#include "destination.h"

class Server
{
public:
    Server() {}
    Server( const char *name, const char *url );
    Server( const char *name, const char *url, const char *file );
    ~Server();

    void cleanUp();

    void readStatusFile( vector<Destination> &v );
    void readLocalIpks( Server *local );
    void readPackageFile( Server *local = 0, bool clearAll = true, bool installedToRoot= false );
    void buildLocalPackages( Server *local );
    Package *getPackage( const char *name );
    Package *getPackage( QString &name );
    QString toString();
    vector<Package> &getPackageList();
    bool isServerActive() { return active; }
    
    void setServerName( const QString &name ) { serverName = name; }
    void setServerUrl( const QString &url )   { serverUrl = url; }
    void setActive( bool val )             { active = val; }
    QString &getServerName() { return serverName; }
    QString &getServerUrl()  { return serverUrl; }

protected:

private:
  QString serverName;
  QString serverUrl;
  QString packageFile;
  bool active;


  vector<Package> packageList;
};

#endif
