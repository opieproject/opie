/***************************************************************************
                          package.h  -  description
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

#ifndef PACKAGE_H
#define PACKAGE_H

#include <stdlib.h>

/**
  *@author Andy Qua
  */

#include <qstring.h>
#include "destination.h"

class Package
{
public:
	Package( QString &name );
	Package( char *name );
	~Package();

    void setLocalPackage( Package *p );
    void setPackageName( QString &name );
    void setVersion( QString &v );
    void setStatus( QString &s );
    void setDescription( QString &d );
    void setFilename( QString &f );
	void setPackageStoredLocally( bool local )	{ packageStoredLocally = local; }
    void setInstalledToRoot( bool root )        { installedToRoot = root; }
    void setInstalledTo( Destination *d )       { installedTo = d; }

    Package *getLocalPackage()      { return localPackage; }
    QString getPackageName()        { return packageName; }
    QString getVersion()            { return version; }
    QString getStatus()             { return status; }
    QString getDescription()        { return description; }
    QString getFilename()			{ return filename; }

    bool isInstalled();
	bool isPackageStoredLocally()	{ return packageStoredLocally; }
    bool isInstalledToRoot()     { return installedToRoot; }
    QString getInstalledVersion();
    Destination *getInstalledTo()   { return installedTo; }

    QString toString();


private:
    Package *localPackage;

    QString packageName;
    QString version;
    QString status;
    QString description;
    QString filename;
	bool packageStoredLocally;
    bool installedToRoot;
    bool installed;
    bool differentVersionAvailable;

    Destination *installedTo;
};

#endif
