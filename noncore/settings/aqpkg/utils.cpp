/***************************************************************************
                          utils.cpp  -  description
                             -------------------
    begin                : Sat Sep 7 2002
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

#include <stdio.h>
#include <sys/vfs.h>
//#include <mntent.h>
 
#include "utils.h"
#include "global.h"

Utils :: Utils()
{
}

Utils :: ~Utils()
{
}


QString Utils :: getPathfromIpkFilename( const QString &file )
{
    int p = file.findRev( "/" );
    QString path = "";
    if ( p != -1 )
    	path = file.left( p );

    return path;
    
}

QString Utils :: getFilenameFromIpkFilename( const QString &file )
{
    int p = file.findRev( "/" );
    QString name = file;
    if ( p != -1 )
    	name = name.mid( p + 1 );
     

    return name;
}

QString Utils :: getPackageNameFromIpkFilename( const QString &file )
{
    int p = file.findRev( "/" );
    QString name = file;
    if ( p != -1 )
        name = name.mid( p + 1 );
    p = name.find( "_" );
    QString packageName = name.mid( 0, p );
    return packageName;
}

QString Utils :: getPackageVersionFromIpkFilename( const QString &file )
{
    int p = file.findRev( "/" );
    QString name = file;
    if ( p != -1 )
    	name = name.mid( p + 1 );
    p = name.find( "_" ) + 1;
    int p2 = name.find( "_", p );
    QString version = name.mid( p, p2 - p );
    return version;
}


bool Utils :: getStorageSpace( const char *path, long *blockSize, long *totalBlocks, long *availBlocks )
{
    bool ret = false;

//    qDebug( "Reading from path %s", path );
    struct statfs fs;
    if ( !statfs( path, &fs ) )
    {
        *blockSize = fs.f_bsize;
        *totalBlocks = fs.f_blocks;
        *availBlocks = fs.f_bavail;
        ret = true;
    }

    return ret;
}

