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

#ifndef SERVER_H
#define SERVER_H

#include <qlist.h>
#include <qstring.h>

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

    void readStatusFile( QList<Destination> &v );
    void readLocalIpks( Server *local );
    void readPackageFile( Server *local = 0, bool clearAll = true, bool installedToRoot= false, Destination *dest = 0 );
    void buildLocalPackages( Server *local );
    Package *getPackage( const char *name );
    Package *getPackage( QString &name );
    QString toString();
    QList<Package> &getPackageList();
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


  QList<Package> packageList;
};

#endif
