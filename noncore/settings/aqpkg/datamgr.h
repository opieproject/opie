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

#ifndef DATAMGR_H
#define DATAMGR_H

#include <map>
using namespace std;

#include <qlist.h>
#include <qobject.h>
#include <qstring.h>

#include "server.h"
#include "destination.h"

#define LOCAL_SERVER			"Installed Pkgs"
#define LOCAL_IPKGS				"local IPKG"

/**
  *@author Andy Qua
  */


class DataManager : public QObject
{
    Q_OBJECT
public:
    DataManager();
    ~DataManager();

    void setActiveServer( const QString &act )      { activeServer = act; }
    QString &getActiveServer( )      				{ return activeServer; }

    Server *getLocalServer()    					{ return ( getServer( LOCAL_SERVER ) ); }
    QList<Server> &getServerList()   				{ return serverList; }
    Server *getServer( const char *name );

    QList<Destination> &getDestinationList()  		{ return destList; }
    Destination *getDestination( const char *name );

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

    QList<Server> serverList;
    QList<Destination> destList;

signals:
    void progressSetSteps( int );
    void progressSetMessage( const QString & );
    void progressUpdate( int );
};

#endif
