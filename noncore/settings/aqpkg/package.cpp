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

#include "package.h"
#include "global.h"

#include <qobject.h>

Package::Package( QString &name )
{
    packageName = name;

    QString tempstr = QObject::tr( "N/A" );
    version = tempstr;
    description = tempstr;
    packageSize = tempstr;
    section = tempstr;

    localPackage = 0;
    installed = false;
    packageStoredLocally = false;
    installedToRoot = false;
    installed = false;
    installedTo = 0;
}

Package::Package( char *name )
{
    packageName = name;

    QString tempstr = QObject::tr( "N/A" );
    version = tempstr;
    description = tempstr;
    packageSize = tempstr;
    section = tempstr;

    localPackage = 0;
    installed = false;
    packageStoredLocally = false;
    installedToRoot = false;
    installed = false;
    installedTo = 0;
    differentVersionAvailable = false;
}

Package::~Package()
{
}

QString Package :: toString()
{
    QString ret = QObject::tr( "Package - %1\n              version - %2" ).
                    arg( getPackageName() ).
                    arg( getVersion() );

    if ( localPackage )
        ret.append( QObject::tr( "\n              inst version - %1" ).arg( localPackage->getVersion() ) );


    return ret;
}

void Package :: setStatus( const QString &s )
{
    QString state_status;
    int two, three;

    status = s.simplifyWhiteSpace( );

    two = status.find( " " ); // find second column
    three = status.find( " ", two + 1 ); // find third column

    state_status = status.mid( three ).stripWhiteSpace( );

    if ( state_status == "installed" )
        installed = true;
}

void Package :: setLocalPackage( Package *p )
{
    localPackage = p;

    if ( localPackage )
        if ( localPackage->getVersion() < getVersion() && !installed )
            differentVersionAvailable = true;
        else
            differentVersionAvailable = false;
}

void Package :: setVersion( const QString &v )
{
    version = v;
            differentVersionAvailable = false;

    if ( localPackage )
        if ( localPackage->getVersion() < getVersion() && !installed )
            differentVersionAvailable = true;
        else
            differentVersionAvailable = false;
}

void Package :: setPackageName( const QString &name )
{
    packageName = name;
}

void Package :: setDescription( const QString &d )
{
    description = d;
}

void Package :: setFilename( const QString &f )
{
	filename = f;
}


QString Package :: getInstalledVersion()
{
    if ( localPackage )
        return localPackage->getVersion();
    else
        return getVersion();
}

QString Package :: getInstalledPackageName()
{
    if ( localPackage )
        return localPackage->getPackageName();
    else
        return getPackageName();
}

bool Package :: isInstalled()
{
    return installed || ( localPackage && localPackage->isInstalled() );
}
