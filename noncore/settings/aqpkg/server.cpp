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

#include "server.h"
#include "datamgr.h"

#include <qfile.h>
#include <qtextstream.h>

#ifdef QWS
#include <qpe/global.h>
#include <qpe/applnk.h>
#include <qlist.h>
#endif

#include "utils.h"

#include "global.h"

Server :: Server( const char *name, const char *url )
{
    serverName = name;
    serverUrl = url;
    packageFile = IPKG_DIR;
    active = true;
    packageFile.append( "lists/" );
    packageFile.append( serverName );
}

Server :: ~Server()
{
	cleanUp();
}

void Server :: cleanUp()
{
    packageList.clear();
}

void Server :: readStatusFile(  QList<Destination> &destList )
{
    cleanUp();

    Destination *dest;
    QListIterator<Destination> dit( destList );
    bool rootRead = false;
    for ( ; dit.current(); ++dit )
    {
        dest = dit.current();
        bool installingToRoot = false;
        
        QString path = dest->getDestinationPath();
        if ( path.right( 1 ) != "/" )
            path.append( "/" );
            
        if ( path == "/" )
        {
            rootRead = true;
            installingToRoot = true;
        }   
        
        packageFile = path;
        packageFile.append( "usr/lib/ipkg/status" );
        readPackageFile( 0, false, installingToRoot, &( *dest ) );
    }

    // Ensure that the root status file is read
    if ( !rootRead )
    {
        packageFile = "/usr/lib/ipkg/status";
        readPackageFile( 0, false, true );
    }
}

void Server :: readLocalIpks( Server *local )
{
    cleanUp();

#ifdef QWS
    // First, get any local IPKGs in the documents area
    // Only applicable to Qtopie/Opie

    DocLnkSet files;
    Global::findDocuments( &files, "application/ipkg" );

    // Now add the items to the list
    QListIterator<DocLnk> it( files.children() );

    for ( ; it.current() ; ++it )
    {
        // OK, we have a local IPK file, I think the standard naming conventions
        // for these are packagename_version_arm.ipk
        QString file = (*it)->file();

        // Changed to display the filename (excluding the path)
        QString packageName = Utils::getPackageNameFromIpkFilename( file );      
        QString ver = Utils::getPackageVersionFromIpkFilename( file );
        Package *package = new Package( packageName );
        package->setVersion( ver );
        package->setFilename( file );
        package->setPackageStoredLocally( true );
        packageList.append( package );
    }
#else
	QString names[] = { "advancedfm_0.9.1-20020811_arm.ipk", "libopie_0.9.1-20020811_arm.ipk", "libopieobex_0.9.1-20020811.1_arm.ipk", "opie-addressbook_0.9.1-20020811_arm.ipk" };
	for ( int i = 0 ; i < 4 ; ++i )
	{
		// OK, we have a local IPK file, I think the standard naming conventions
		// for these are packagename_version_arm.ipk
		QString file = names[i];
		int p = file.find( "_" );
		QString tmp = file.mid( 0, p );
		packageList.push_back( Package( tmp ) );
		int p2 = file.find( "_", p+1 );
		tmp = file.mid( p+1, p2-(p+1) );
		packageList.back().setVersion( tmp );
		packageList.back().setPackageStoredLocally( true );
	}
#endif

    // build local packages
    buildLocalPackages( local );
}

void Server :: readPackageFile( Server *local, bool clearAll, bool installingToRoot, Destination *dest )
{
    QFile f( packageFile );
    if ( !f.open( IO_ReadOnly ) )
        return;
    QTextStream t( &f );

    QString line;
    QString key;
    QString value;
    int pos;

    if ( clearAll )
        cleanUp();
    Package *currPackage = 0;

    bool newPackage = true;
    while ( !t.eof() )
    {
        line = t.readLine();
        
        pos = line.find( ':', 0 );
        if ( pos > -1 )
            key = line.mid( 0, pos ).stripWhiteSpace();
        else
            key = QString::null;
        value = line.mid( pos+1, line.length()-pos ).stripWhiteSpace();

        if ( key == "Package" && newPackage )
        {
            newPackage = false;

            currPackage = getPackage( value );
            if ( !currPackage )
            {
                Package *package = new Package( value );
                packageList.append( package );
                currPackage = package;
                currPackage->setInstalledTo( dest );
                if ( installingToRoot )
                    currPackage->setInstalledToRoot( true );
            }
            else
            {
                if ( currPackage->isInstalled() )
                    currPackage->setInstalledTo( dest );
            }
        }
        else if ( key == "Version" )
        {
            if ( currPackage )
                currPackage->setVersion( value );
        }
        else if ( key == "Status" )
        {
            if ( currPackage )
                currPackage->setStatus( value );
        }
        else if ( key == "Description" )
        {
            if ( currPackage )
                currPackage->setDescription( value );
        }
        else if ( key == "Filename" )
        {
            if ( currPackage )
                currPackage->setFilename( value );
        }
        else if ( key == "Size" )
        {
            if ( currPackage )
                currPackage->setPackageSize( value );
        }
        else if ( key == "Section" )
        {
            if ( currPackage )
                currPackage->setSection( value );

            DataManager::setAvailableCategories( value );
        }
        else if ( key == QString::null )
        {
            newPackage = true;
        }
    }

    f.close();

    // build local packages
    buildLocalPackages( local );
}

void Server :: buildLocalPackages( Server *local )
{
    Package *curr;
    QListIterator<Package> it( packageList );
    
    QList<Package> *locallist = &local->getPackageList();
    
    for ( ; it.current(); ++it )
    {
        curr = it.current();
        QString name = curr->getPackageName();

        // If the package name is an ipk name, then convert the filename to a package name
        if ( name.find( ".ipk" ) != -1 )
            name = Utils::getPackageNameFromIpkFilename( curr->getFilename() );

        if ( local )
        {
            Package *p = local->getPackage( name );
            curr->setLocalPackage( p );
            if ( p )
            {
                // Replace local version 
                if ( curr->getVersion() > p->getVersion() )
                {
                    int pos = locallist->at();
                    locallist->remove( p );
                    locallist->insert( pos, curr );
                }
                
                // Set some default stuff like size and things
                if ( p->getInstalledVersion() == curr->getVersion() )
                {
                    p->setPackageSize( curr->getPackageSize() );
                    p->setSection( curr->getSection() );
                    p->setDescription( curr->getDescription() );
                }
            }
            
        }
        else
            curr->setLocalPackage( 0 );
    }

}

Package *Server :: getPackage( QString &name )
{
    return getPackage( (const char *)name );
}

Package *Server :: getPackage( const char *name )
{
    Package *ret = 0;

    QListIterator<Package> it( packageList );
    for ( ; it.current(); ++it )
    {
		if ( it.current()->getPackageName() == name )
			ret = it.current();
    }

    return ret;
}

QString Server :: toString()
{
    QString ret = QString( "Server\n   name - %1\n   url - %2\n" ).arg( serverName ).arg( serverUrl );
                       
    QListIterator<Package> it( packageList );
    for ( ; it.current(); ++it )
    {
        ret.append( QString( "\n   %1" ).arg( it.current()->toString() ) );
    }


    return ret;
}

QList<Package> &Server::getPackageList()
{
    return packageList;
}

