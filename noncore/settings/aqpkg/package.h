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
    void setPackageName( const QString &name );
    void setVersion( const QString &v );
    void setStatus( const QString &s );
    void setDescription( const QString &d );
    void setFilename( const QString &f );
	void setPackageStoredLocally( bool local )	{ packageStoredLocally = local; }
    void setInstalledToRoot( bool root )        { installedToRoot = root; }
    void setInstalledTo( Destination *d )       { installedTo = d; }
    void setDependancies( QString &deps )       { dependancies = deps; }
    void setPackageSize( const QString &size )         { packageSize = size; }
    void setSection( const QString &sect)              { section = sect; }

    Package *getLocalPackage()       { return localPackage; }
    QString getPackageName()         { return packageName; }
    QString getVersion()             { return version; }
    QString getStatus()              { return status; }
    QString getDescription()         { return description; }
    QString getFilename()            { return filename; }
    QString getDependancies()        { return dependancies; }
    QString getPackageSize()         { return packageSize; }
    QString getSection()             { return section; }
    bool    getNewVersionAvailable() { return differentVersionAvailable; }

    bool isInstalled();
	bool isPackageStoredLocally()	{ return packageStoredLocally; }
    bool isInstalledToRoot()     { return installedToRoot; }
    QString getInstalledVersion();
    QString getInstalledPackageName();
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
    QString dependancies;
    QString packageSize;
    QString section;

    Destination *installedTo;
};

#endif
