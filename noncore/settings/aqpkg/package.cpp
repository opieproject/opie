/***************************************************************************
                          package.cpp  -  description
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

#include "package.h"
#include "global.h"

Package::Package( QString &name )
{
    packageName = name;
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
    localPackage = 0;
    installed = false;
    packageStoredLocally = false;
    installedToRoot = false;
    installed = false;
    installedTo = 0;
}

Package::~Package()
{
}

QString Package :: toString()
{
    QString ret = "Package - " + getPackageName() +
           "\n              version - " + getVersion();

    if ( localPackage )
        ret += "\n              inst version - " + localPackage->getVersion();


    return ret;
}

void Package :: setStatus( QString &s )
{
    status = s;

    if ( status.find( "installed" ) != -1 )
        installed = true;
}

void Package :: setLocalPackage( Package *p )
{
    localPackage = p;

    if ( localPackage )
        if ( localPackage->getVersion() != getVersion() )
            differentVersionAvailable = true;
        else
            differentVersionAvailable = false;
}

void Package :: setVersion( QString &v )
{
    version = v;

    if ( localPackage )
        if ( localPackage->getVersion() != getVersion() )
            differentVersionAvailable = true;
        else
            differentVersionAvailable = false;
}

void Package :: setPackageName( QString &name )
{
    packageName = name;
}

void Package :: setDescription( QString &d )
{
    description = d;
}

void Package :: setFilename( QString &f )
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
