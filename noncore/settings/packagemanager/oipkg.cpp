/*
                            This file is part of the Opie Project

                             Copyright (c)  2003 Dan Williams <drw@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "oipkg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <qdir.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qtextstream.h>

const QString IPKG_CONF        = "/etc/ipkg.conf";      // Fully-qualified name of Ipkg primary configuration file
const QString IPKG_CONF_DIR    = "/etc/ipkg";           // Directory of secondary Ipkg configuration files
const QString IPKG_PKG_PATH    = "/usr/lib/ipkg/lists"; // Directory containing server package lists
const QString IPKG_STATUS_PATH = "usr/lib/ipkg/status"; // Destination status file location

OIpkg *oipkg;

// Ipkg callback functions

int fsignalIpkgMessage( ipkg_conf_t */*conf*/, message_level_t /*level*/, char *msg )
{
    oipkg->ipkgMessage( msg );
    return 0;
}

char *fIpkgResponse( char */*question*/ )
{
    return 0x0;
}

int fIpkgStatus( char */*name*/, int /*status*/, char *desc, void */*userdata*/ )
{
    oipkg->ipkgStatus( desc );
    return 0;
}

int fIpkgFiles( char */*name*/, char *desc, char */*version*/, pkg_state_status_t /*status*/,
                void */*userdata*/ )
{
printf( "*****List*****\n%s\n", desc );
    oipkg->ipkgList( desc );
    return 0;
}

OIpkg::OIpkg( Config *config, QObject *parent, const char *name )
    : QObject( parent, name )
    , m_config( config )
    , m_confInfo( NULL )
    , m_ipkgExecOptions( 0 )
    , m_ipkgExecVerbosity( 1 )
{
    oipkg = this;

    // Initialize libipkg
    if ( ipkg_init( &fsignalIpkgMessage, &fIpkgResponse, &m_ipkgArgs ) )
        QMessageBox::critical( 0, tr( "OIpkg" ), tr( "Error initialing libipkg" ) );

    // Default ipkg run-time arguments
    m_ipkgArgs.noaction = false;
    m_ipkgArgs.force_defaults = true;
}

OIpkg::~OIpkg()
{
    // Upon destruction, ensure that items in config list are deleted with list
    if ( m_confInfo )
        m_confInfo->setAutoDelete( true );

    // Free up libipkg resources
    if ( ipkg_deinit( &m_ipkgArgs ) )
        QMessageBox::critical( 0, tr( "OIpkg" ), tr( "Error freeing libipkg" ) );
}

OConfItemList *OIpkg::configItems()
{
    // Retrieve all configuration items
    return filterConfItems();
}

OConfItemList *OIpkg::servers()
{
    // Retrieve only servers
    return filterConfItems( OConfItem::Source );
}

OConfItemList *OIpkg::destinations()
{
    // Retrieve only destinations
    return filterConfItems( OConfItem::Destination );
}

OConfItemList *OIpkg::options()
{
    // Retrieve only destinations
    return filterConfItems( OConfItem::Option );
}

void OIpkg::setConfigItems( OConfItemList *configList )
{
    if ( m_confInfo )
        delete m_confInfo;

    m_confInfo = configList;
}

void OIpkg::saveSettings()
{
    // Save Ipkg execution options to application configuration file
    if ( m_config )
    {
        m_config->setGroup( "Ipkg" );
        m_config->writeEntry( "ExecOptions", m_ipkgExecOptions );
        m_config->writeEntry( "Verbosity", m_ipkgExecVerbosity );
    }
}

OPackageList *OIpkg::availablePackages( const QString &server )
{
    // Load Ipkg configuration info if not already cached
    if ( !m_confInfo )
        loadConfiguration();

    // Build new server list (caller is responsible for deleting)
    OPackageList *pl = new OPackageList;

    // Open package list file
    QFile f( IPKG_PKG_PATH + "/" + server );
    if ( !f.open( IO_ReadOnly ) )
        return NULL;
    QTextStream t( &f );

    // Process all information in package list file
    OPackage *package = NULL;
    QString line = t.readLine();
    while ( !t.eof() )
    {
        // Determine key/value pair
        int pos = line.find( ':', 0 );
        QString key;
        if ( pos > -1 )
            key = line.mid( 0, pos );
        else
            key = QString::null;
        QString value = line.mid( pos+2, line.length()-pos );

        // Allocate new package and insert into list
        if ( package == NULL  && !key.isEmpty() )
        {
            package = new OPackage( value );
            package->setSource( server );
            pl->append( package );
        }

        // Update package data
        if ( key == "Package" )
                package->setName( value );
        else if ( key == "Version" )
            package->setVersion( value );
        else if ( key == "Section" )
            package->setCategory( value );
            //DataManager::setAvailableCategories( value );
        else if ( key.isEmpty() && value.isEmpty() )
            package = NULL;

        // Skip past all description lines
        if ( key == "Description" )
        {
            line = t.readLine();
            while ( !line.isEmpty() && line.find( ':', 0 ) == -1 && !t.eof() )
                line = t.readLine();
        }
        else
            line = t.readLine();
    }

    f.close();

    return pl;
}

OPackageList *OIpkg::installedPackages( const QString &destName, const QString &destPath )
{
    // Load Ipkg configuration info if not already cached
    if ( !m_confInfo )
        loadConfiguration();

    // Build new server list (caller is responsible for deleting)
    OPackageList *pl = new OPackageList;

    // Open status file
    QString path = destPath;
    if ( path.right( 1 ) != "/" )
        path.append( "/" );
    path.append( IPKG_STATUS_PATH );

    QFile f( path );
    if ( !f.open( IO_ReadOnly ) )
        return NULL;
    QTextStream t( &f );

    // Process all information in status file
    bool newPackage = false;
    QString line = t.readLine();
    QString name;
    QString version;
    QString status;

    while ( !t.eof() )
    {
        // Determine key/value pair
        int pos = line.find( ':', 0 );
        QString key;
        if ( pos > -1 )
            key = line.mid( 0, pos );
        else
            key = QString::null;
        QString value = line.mid( pos+2, line.length()-pos );

        // Allocate new package and insert into list
        if ( newPackage  && !key.isEmpty() )
        {
            // Add to list only if it has a valid name and is installed
            if ( !name.isNull() && status.contains( " installed" ) )
            {
                pl->append( new OPackage( name, QString::null, version, QString::null, destName ) );
                name = QString::null;
                version = QString::null;
                status = QString::null;

                newPackage = false;
            }
        }

        // Update package data
        if ( key == "Package" )
            name = value;
        else if ( key == "Version" )
            version = value;
        else if ( key == "Status" )
            status = value;
        else if ( key.isEmpty() && value.isEmpty() )
            newPackage = true;

        // Skip past all description lines
        if ( key == "Description" )
        {
            line = t.readLine();
            while ( !line.isEmpty() && line.find( ':', 0 ) == -1 && !t.eof() )
                line = t.readLine();
        }
        else
            line = t.readLine();
    }

    f.close();

    return pl;
}

bool OIpkg::executeCommand( OPackage::Command command, QStringList *parameters, const QString &destination,
                            const QObject *receiver, const char *slotOutput, bool rawOutput )
{
    if ( command == OPackage::NotDefined )
        return false;

    // Set ipkg run-time options/arguments
    m_ipkgArgs.force_depends = ( m_ipkgExecOptions & FORCE_DEPENDS );
    m_ipkgArgs.force_reinstall = ( m_ipkgExecOptions & FORCE_REINSTALL );
    // TODO m_ipkgArgs.force_remove = ( m_ipkgExecOptions & FORCE_REMOVE );
    m_ipkgArgs.force_overwrite = ( m_ipkgExecOptions & FORCE_OVERWRITE );
    m_ipkgArgs.verbosity = m_ipkgExecVerbosity;
    if ( m_ipkgArgs.dest )
        free( m_ipkgArgs.dest );
    if ( !destination.isNull() )
    {
        int len = destination.length() + 1;
        m_ipkgArgs.dest = (char *)malloc( len );
        strncpy( m_ipkgArgs.dest, destination, destination.length() );
        m_ipkgArgs.dest[ len - 1 ] = '\0';
    }
    else
        m_ipkgArgs.dest = 0x0;

    // Connect output signal to widget

    if ( rawOutput )
    {
//        if ( slotOutput )
//            connect( this, SIGNAL(signalIpkgMessage(char *)), receiver, slotOutput );
    }
    else
    {
        // TODO - connect to local slot and parse output before emitting signalIpkgMessage
    }

    switch( command )
    {
        case OPackage::Update : {
                connect( this, SIGNAL(signalIpkgMessage(char *)), receiver, slotOutput );
                ipkg_lists_update( &m_ipkgArgs );
            };
            break;
        case OPackage::Upgrade : {
                connect( this, SIGNAL(signalIpkgMessage(char *)), receiver, slotOutput );
                ipkg_packages_upgrade( &m_ipkgArgs );
            };
            break;
        case OPackage::Install : {
                connect( this, SIGNAL(signalIpkgMessage(char *)), receiver, slotOutput );
                for ( QStringList::Iterator it = parameters->begin(); it != parameters->end(); ++it )
                {
                    ipkg_packages_install( &m_ipkgArgs, (*it) );
                }
            };
            break;
        case OPackage::Remove : {
                connect( this, SIGNAL(signalIpkgMessage(char *)), receiver, slotOutput );
                for ( QStringList::Iterator it = parameters->begin(); it != parameters->end(); ++it )
                {
                    ipkg_packages_remove( &m_ipkgArgs, (*it), true );
                }
            };
            break;
        case OPackage::Download : {
                connect( this, SIGNAL(signalIpkgMessage(char *)), receiver, slotOutput );
                for ( QStringList::Iterator it = parameters->begin(); it != parameters->end(); ++it )
                {
                    ipkg_packages_download( &m_ipkgArgs, (*it) );
                }
            };
            break;
        case OPackage::Info : {
                connect( this, SIGNAL(signalIpkgStatus(char *)), receiver, slotOutput );
                ipkg_packages_info( &m_ipkgArgs, (*parameters->begin()), &fIpkgStatus, 0x0 );
            };
            break;
        case OPackage::Files : {
                connect( this, SIGNAL(signalIpkgList(char *)), receiver, slotOutput );
                ipkg_package_files( &m_ipkgArgs, (*parameters->begin()), &fIpkgFiles, 0x0 );
            };
            break;
        default : break;
    };

    return true;
}

void OIpkg::ipkgMessage( char *msg )
{
    emit signalIpkgMessage( msg );
}

void OIpkg::ipkgStatus( char *status )
{
    emit signalIpkgStatus( status );
}

void OIpkg::ipkgList( char *filelist )
{
    emit signalIpkgList( filelist );
}

void OIpkg::loadConfiguration()
{
    if ( m_confInfo )
        delete m_confInfo;

    // Load configuration item list
    m_confInfo = new OConfItemList();

    QStringList confFiles;
    QDir confDir( IPKG_CONF_DIR );
    if ( confDir.exists() )
    {
        confDir.setNameFilter( "*.conf" );
        confDir.setFilter( QDir::Files );
        confFiles = confDir.entryList( "*.conf", QDir::Files );
        confFiles << IPKG_CONF;

        for ( QStringList::Iterator it = confFiles.begin(); it != confFiles.end(); ++it )
        {
            // Create absolute file path if necessary
            QString absFile = (*it);
            if ( !absFile.startsWith( "/" ) )
                absFile.prepend( QString( IPKG_CONF_DIR ) + "/" );

            // Read in file
            QFile f( absFile );
            if ( f.open( IO_ReadOnly ) )
            {
                QTextStream s( &f );
                while ( !s.eof() )
                {

                    QString line = s.readLine().simplifyWhiteSpace();

                    // Parse line and save info to the conf options list
                    if ( !line.isEmpty() )
                    {
                        if ( !line.startsWith( "#" ) ||
                             line.startsWith( "#src" ) ||
                             line.startsWith( "#dest" ) ||
                             line.startsWith( "#arch" ) ||
                             line.startsWith( "#option" ) )
                        {
                            int pos = line.find( ' ', 1 );

                            // Type
                            QString typeStr = line.left( pos );
                            OConfItem::Type type;
                            if ( typeStr == "src" || typeStr == "#src" )
                                type = OConfItem::Source;
                            else if ( typeStr == "dest" || typeStr == "#dest" )
                                type = OConfItem::Destination;
                            else if ( typeStr == "option" || typeStr == "#option" )
                                type = OConfItem::Option;
                            else if ( typeStr == "arch" || typeStr == "#arch" )
                                type = OConfItem::Arch;
                            else
                                type = OConfItem::NotDefined;
                            ++pos;
                            int endpos = line.find( ' ', pos );

                            // Name
                            QString name = line.mid( pos, endpos - pos );

                            // Value
                            QString value = "";
                            if ( endpos > -1 )
                                value = line.right( line.length() - endpos - 1 );

                            // Active
                            bool active = !line.startsWith( "#" );

                            // Add to list
                            m_confInfo->append( new OConfItem( absFile, type, name, value, active ) );
                        }
                    }
                }

                f.close();
            }
        }
    }

    // Load Ipkg execution options from application configuration file
    if ( m_config )
    {
        m_config->setGroup( "Ipkg" );
        m_ipkgExecOptions = m_config->readNumEntry( "ExecOptions", m_ipkgExecOptions );
        m_ipkgExecVerbosity = m_config->readNumEntry( "Verbosity", m_ipkgExecVerbosity );
    }
}

OConfItemList *OIpkg::filterConfItems( OConfItem::Type typefilter )
{
    // Load Ipkg configuration info if not already cached
    if ( !m_confInfo )
        loadConfiguration();

    // Build new server list (caller is responsible for deleting)
    OConfItemList *sl = new OConfItemList;

    // If typefilter is empty, retrieve all items
    bool retrieveAll = ( typefilter == OConfItem::NotDefined );

    // Parse configuration info for servers
    OConfItemListIterator it( *m_confInfo );
    for ( ; it.current(); ++it )
    {
        OConfItem *item = it.current();
        if ( retrieveAll || item->type() == typefilter )
        {
            sl->append( item );
        }
    }

    return sl;
}
