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

#ifdef QWS
#include <qpe/config.h>
#include <qpe/qpeapplication.h>
#else
#include <qapplication.h>
#endif

#include <qfile.h>
#include <qtextstream.h>

#include "datamgr.h"
#include "global.h"

QString LOCAL_SERVER;
QString LOCAL_IPKGS;


QString DataManager::availableCategories = "";
DataManager::DataManager()
    : QObject( 0x0, 0x0 )
{
    activeServer = "";
    availableCategories = "#";

    serverList.setAutoDelete( TRUE );
    destList.setAutoDelete( TRUE );
}

DataManager::~DataManager()
{
}

Server *DataManager :: getServer( const char *name )
{
    QListIterator<Server> it( serverList );
    while ( it.current() && it.current()->getServerName() != name )
    {
		++it;
	}

	return it.current();
}

Destination *DataManager :: getDestination( const char *name )
{
    QListIterator<Destination> it( destList );
    while ( it.current() && it.current()->getDestinationName() != name )
    {
		++it;
	}

	return it.current();
}

void DataManager :: loadServers()
{
    // First add our local server - not really a server but
    // the local config (which packages are installed)
    serverList.append( new Server( LOCAL_SERVER, "" ) );
    serverList.append( new Server( LOCAL_IPKGS, "" ) );

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
                Server *s = new Server( alias, url );
                if ( lineStr.startsWith( "src" ) )
                    s->setActive( true );
                else
                    s->setActive( false );

                serverList.append( s );

            }
            else if ( lineStr.startsWith( "dest" ) )
            {
                char alias[20];
                char path[50];
                sscanf( lineStr, "%*[^ ] %s %s", alias, path );
                Destination *d = new Destination( alias, path );
                bool linkToRoot = true;
#ifdef QWS
                QString key = alias;
                key += "_linkToRoot";
                linkToRoot = cfg.readBoolEntry( key, true );
#endif
                d->linkToRoot( linkToRoot );

                destList.append( d );
            }
            else if ( lineStr.startsWith( "option" ) || lineStr.startsWith( "#option" ) )
            {
                char type[20];
                char val[100];
                sscanf( lineStr, "%*[^ ] %s %s", type, val );
                if ( stricmp( type, "http_proxy" ) == 0 )
                {
                    httpProxy = val;
                    if ( lineStr.startsWith( "#" ) )
                        httpProxyEnabled = false;
                    else
                        httpProxyEnabled = true;
                }
                if ( stricmp( type, "ftp_proxy" ) == 0 )
                {
                    ftpProxy = val;
                    if ( lineStr.startsWith( "#" ) )
                        ftpProxyEnabled = false;
                    else
                        ftpProxyEnabled = true;
                }
                if ( stricmp( type, "proxy_username" ) == 0 )
                    proxyUsername = val;
                if ( stricmp( type, "proxy_password" ) == 0 )
                    proxyPassword = val;
            }
        }
    }
    fclose( fp );

    reloadServerData( );
}

void DataManager :: reloadServerData( )
{
    emit progressSetSteps( serverList.count() );
    emit progressSetMessage( tr( "Reading configuration..." ) );

    QString serverName;
    int i = 0;

    Server *server;
    QListIterator<Server> it( serverList );
    for ( ; it.current(); ++it )
    {
        server = it.current();
        serverName = server->getServerName();
        i++;
        emit progressUpdate( i );
        qApp->processEvents();

    	// Now we've read the config file in we need to read the servers
    	// The local server is a special case. This holds the contents of the
    	// status files the number of which depends on how many destinations
    	// we've set up
    	// The other servers files hold the contents of the server package list
    	if ( serverName == LOCAL_SERVER )
        	server->readStatusFile( destList );
    	else if ( serverName == LOCAL_IPKGS )
            server->readLocalIpks( getServer( LOCAL_SERVER ) );
    	else
            server->readPackageFile( getServer( LOCAL_SERVER ) );
	}
}

void DataManager :: writeOutIpkgConf()
{
    QFile f( IPKG_CONF );
    if ( !f.open( IO_WriteOnly ) )
    {
        return;
    }

    QTextStream t( &f );
/*
    QString ipkg_conf = IPKG_CONF;
    ofstream out( ipkg_conf );
*/
    t << "# Written by AQPkg\n";
    t << "# Must have one or more source entries of the form:\n";
    t << "#\n";
    t << "#   src <src-name> <source-url>\n";
    t << "#\n";
    t << "# and one or more destination entries of the form:\n";
    t << "#\n";
    t << "#   dest <dest-name> <target-path>\n";
    t << "#\n";
    t << "# where <src-name> and <dest-names> are identifiers that\n";
    t << "# should match [a-zA-Z0-9._-]+, <source-url> should be a\n";
    t << "# URL that points to a directory containing a Familiar\n";
    t << "# Packages file, and <target-path> should be a directory\n";
    t << "# that exists on the target system.\n\n";

    // Write out servers
    Server *server;
    QListIterator<Server> it( serverList );
    while ( it.current() )
    {
        server = it.current();
        QString alias = server->getServerName();
        // Don't write out local as its a dummy
        if ( alias != LOCAL_SERVER && alias != LOCAL_IPKGS )
        {
            QString url = server->getServerUrl();;

            if ( !server->isServerActive() )
                t << "#";
            t << "src " << alias << " " << url << endl;
        }

        ++it;
    }

    t << endl;

    // Write out destinations
    QListIterator<Destination> it2( destList );
    while ( it2.current() )
    {
        t << "dest " << it2.current()->getDestinationName() << " " << it2.current()->getDestinationPath() << endl;
        ++it2;
    }

    t << endl;
    t << "# Proxy Support\n";

    if ( !httpProxyEnabled && httpProxy == "" )
        t << "#option http_proxy http://proxy.tld:3128\n";
    else
    {
        if ( !httpProxyEnabled )
            t << "#";
        t << "option http_proxy " << httpProxy << endl;
    }

    if ( !ftpProxyEnabled && ftpProxy == "" )
        t << "#option ftp_proxy http://proxy.tld:3128\n";
    else
    {
        if ( !ftpProxyEnabled )
            t << "#";
        t << "option ftp_proxy " << ftpProxy << endl;
    }
    if ( proxyUsername == "" || (!httpProxyEnabled && !ftpProxyEnabled) )
        t << "#option proxy_username <username>\n";
    else
        t << "option proxy_username " << proxyUsername << endl;
    if ( proxyPassword == "" || (!httpProxyEnabled && !ftpProxyEnabled) )
        t << "#option proxy_password <password>\n\n";
    else
        t << "option proxy_password " << proxyPassword << endl<< endl;

    t << "# Offline mode (for use in constructing flash images offline)\n";
    t << "#option offline_root target\n";

    f.close();
}


void DataManager :: setAvailableCategories( QString section )
{
    QString sectstr = "#";
    sectstr.append( section.lower() );
    sectstr.append( "#" );
    if ( availableCategories.find( sectstr ) == -1 )
    {
        availableCategories.append( section );
        availableCategories.append( "#" );
    }
}
