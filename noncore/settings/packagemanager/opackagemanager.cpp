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

#include "opackagemanager.h"
#include "oipkgconfigdlg.h"

#include <qpe/qpeapplication.h>

#include <ctype.h>

OPackageManager::OPackageManager( Config *config, QObject *parent, const char *name )
    : QObject( parent, name )
    , m_config( config )
    , m_ipkg( m_config, this )
    , m_packages( 9973 )
    , m_categories()
{
    m_packages.setAutoDelete( true );
}

void OPackageManager::loadAvailablePackages()
{
    m_packages.clear();

    OConfItemList *serverList = m_ipkg.servers();

    if ( serverList )
    {
        // Initialize status messaging
        emit initStatus( serverList->count() );
        int serverCount = 0;

        bool categoryAdded = false;

        for ( OConfItemListIterator serverIt( *serverList ); serverIt.current(); ++serverIt )
        {
            OConfItem *server = serverIt.current();

            // Process server only if it is active
            if ( server->active() )
            {
                // Update status
                QString status = tr( "Reading available packages:\n\t" );
                status.append( server->name() );
                emit statusText( status );
                ++serverCount;
                emit statusBar( serverCount );
                qApp->processEvents();

                OPackageList *packageList = m_ipkg.availablePackages( server->name() );
                if ( packageList )
                {
                    for ( OPackageListIterator packageIt( *packageList ); packageIt.current(); ++packageIt )
                    {
                        OPackage *package = packageIt.current();

                        // Load package info
                        if ( !m_packages.find( package->name() ) )
                            m_packages.insert( package->name(), package );
                        else
                        {
                            // If new package is newer version, replace existing package
                            OPackage *currPackage = m_packages[package->name()];
                            if ( compareVersions( package->version(), currPackage->version() ) == 1 )
                                m_packages.replace( package->name(), package );
                        }

                        // Add category to list if it doesn't already exist
                        if ( m_categories.grep( package->category() ).isEmpty() )
                        {
                            m_categories << package->category();
                            categoryAdded = true;
                        }
                    }
                }
            }
        }
        delete serverList;

        // Sort category list if categories were added
        if ( categoryAdded )
            m_categories.sort();
    }
}

void OPackageManager::loadInstalledPackages()
{
    OConfItemList *destList = m_ipkg.destinations();

    if ( destList )
    {
        // Initialize status messaging
        emit initStatus( destList->count() );
        int destCount = 0;

        bool categoryAdded = false;

        for ( OConfItemListIterator destIt( *destList ); destIt.current(); ++destIt )
        {
            OConfItem *destination = destIt.current();

            // Process destination only if it is active
            if ( destination->active() )
            {
                // Update status
                QString status = tr( "Reading installed packages:\n\t" );
                status.append( destination->name() );
                emit statusText( status );
                ++destCount;
                emit statusBar( destCount );
                qApp->processEvents();

                OPackageList *packageList = m_ipkg.installedPackages( destination->name(),
                                                                      destination->value() );
                if ( packageList )
                {
                    for ( OPackageListIterator packageIt( *packageList ); packageIt.current(); ++packageIt )
                    {
                        OPackage *package = packageIt.current();
                        OPackage *currPackage = m_packages[package->name()];
                        if ( currPackage )
                        {
                            // Package is in a current feed, update installed version, destination
                            currPackage->setVersionInstalled( package->versionInstalled() );
                            currPackage->setDestination( package->destination() );

                            delete package;
                        }
                        else
                        {
                            // Package isn't in a current feed, add to list
                            m_packages.insert( package->name(), package );

                            // Add category to list if it doesn't already exist
                            if ( m_categories.grep( package->category() ).isEmpty() )
                            {
                                m_categories << package->category();
                                categoryAdded = true;
                            }
                        }
                    }
                }
            }
        }
        delete destList;

        // Sort category list if categories were added
        if ( categoryAdded )
            m_categories.sort();
    }
}

OPackageList *OPackageManager::packages()
{
    // TODO - look to see if list is loaded, if not, load available & installed

    OPackageList *pl = new OPackageList;

    for ( QDictIterator<OPackage> packageIt( m_packages ); packageIt.current(); ++packageIt )
        pl->append( packageIt.current() );

    return pl;
}

OPackageList *OPackageManager::filterPackages( const QString &name,const QString &server,
                                  const QString &destination, Status status, const QString &category )
{
    // TODO - look to see if list is loaded, if not, load available & installed

    OPackageList *pl = new OPackageList;
    for ( QDictIterator<OPackage> packageIt( m_packages ); packageIt.current(); ++packageIt )
    {
        OPackage *package = packageIt.current();

        bool nameMatch = ( name.isNull() || package->name().contains( name ) );
        bool serverMatch = ( server.isNull() || package->source() == server );
        bool destinationMatch = ( destination.isNull() || package->destination() == destination );
        bool statusMatch;
        switch ( status )
        {
            case All : statusMatch = true;
                break;
            case NotInstalled : statusMatch = package->versionInstalled().isNull();
                break;
            case Installed : statusMatch = !package->versionInstalled().isNull();
                break;
            case Updated : statusMatch = ( !package->versionInstalled().isNull() &&
                                  compareVersions( package->version(), package->versionInstalled() ) == 1 );
                break;
            default : statusMatch = true;
                break;
        };
        bool categoryMatch = ( category.isNull() || package->category() == category );

        if ( nameMatch && serverMatch && destinationMatch && statusMatch && categoryMatch )
            pl->append( packageIt.current() );
    }

    return pl;
}

QStringList *OPackageManager::servers()
{
    QStringList *sl = new QStringList();

    OConfItemList *serverList = m_ipkg.servers();
    if ( serverList )
    {
        for ( OConfItemListIterator serverIt( *serverList ); serverIt.current(); ++serverIt )
        {
            OConfItem *server = serverIt.current();

            // Add only active servers
            if ( server->active() )
                *sl << server->name();
        }
    }

    return sl;
}

QStringList *OPackageManager::destinations()
{
    QStringList *dl = new QStringList();

    OConfItemList *destList = m_ipkg.destinations();
    if ( destList )
    {
        for ( OConfItemListIterator destIt( *destList ); destIt.current(); ++destIt )
        {
            OConfItem *destination = destIt.current();

            // Add only active destinations
            if ( destination->active() )
                *dl << destination->name();
        }
    }

    return dl;
}

OConfItem *OPackageManager::findConfItem( OConfItem::Type type, const QString &name )
{
    OConfItem *confItem = 0x0;
    OConfItemList *confList = m_ipkg.configItems();
    if ( confList )
    {
        for ( OConfItemListIterator confIt( *confList ); confIt.current(); ++confIt )
        {
            OConfItem *conf = confIt.current();

            // Add only active confinations
            if ( conf->type() == type && conf->name() == name )
            {
                confItem = conf;
                break;
            }
        }
    }

    return confItem;

}

OPackage *OPackageManager::findPackage( const QString &name )
{
    return m_packages[ name ];
}

int OPackageManager::compareVersions( const QString &ver1, const QString &ver2 )
{
    // TODO - should this be in OIpkg???

    int epoch1, epoch2;
    QString version1, revision1;
    QString version2, revision2;

    parseVersion( ver1, &epoch1, &version1, &revision1 );
    parseVersion( ver2, &epoch2, &version2, &revision2 );

    if ( epoch1 > epoch2 )
        return 1;
    else if ( epoch1 < epoch2 )
        return -1;

    int r = verrevcmp( version1.latin1(), version2.latin1() );
    if (r)
        return r;

    r = verrevcmp( revision1.latin1(), revision2.latin1() );
    return r;
}

bool OPackageManager::configureDlg( bool installOptions )
{
    OIpkgConfigDlg dlg( &m_ipkg, installOptions, static_cast<QWidget *>(parent()) );
    return ( QPEApplication::execDialog( &dlg ) == QDialog::Accepted );
}

void OPackageManager::saveSettings()
{
    m_ipkg.saveSettings();
}

bool OPackageManager::executeCommand( OPackage::Command command, QStringList *packages,
                                      const QString &destination, const QObject *receiver,
                                      const char *slotOutput, bool rawOutput )
{
    return m_ipkg.executeCommand( command, packages, destination, receiver, slotOutput, rawOutput );
}

void OPackageManager::parseVersion( const QString &verstr, int *epoch, QString *version,
                                    QString *revision )
{
    *epoch = 0;
    *revision = QString::null;

    // Version string is in the format "ee:vv-rv", where ee=epoch, vv=version, rv=revision

    // Get epoch
    int colonpos = verstr.find( ':' );
    if ( colonpos > -1 )
    {
        *epoch = verstr.left( colonpos ).toInt();
    }

    // Get version and revision
    int hyphenpos = verstr.find( '-', colonpos + 1 );
    int verlen = verstr.length();
    if ( hyphenpos > -1 )
    {
        *version = verstr.mid( colonpos + 1, hyphenpos - colonpos - 1 );
        *revision = verstr.right( verlen - hyphenpos - 1 );
    }
    else
    {
       *version = verstr.right( verlen - colonpos );
    }
}

/*
 * libdpkg - Debian packaging suite library routines
 * vercmp.c - comparison of version numbers
 *
 * Copyright (C) 1995 Ian Jackson <iwj10@cus.cam.ac.uk>
 */
int OPackageManager::verrevcmp( const char *val, const char *ref )
{
     int vc, rc;
     long vl, rl;
     const char *vp, *rp;
     const char *vsep, *rsep;

     if (!val) val= "";
     if (!ref) ref= "";
     for (;;) {
      vp= val;  while (*vp && !isdigit(*vp)) vp++;
      rp= ref;  while (*rp && !isdigit(*rp)) rp++;
      for (;;) {
           vc= (val == vp) ? 0 : *val++;
           rc= (ref == rp) ? 0 : *ref++;
           if (!rc && !vc) break;
           if (vc && !isalpha(vc)) vc += 256; /* assumes ASCII character set */
           if (rc && !isalpha(rc)) rc += 256;
           if (vc != rc) return vc - rc;
      }
      val= vp;
      ref= rp;
      vl=0;  if (isdigit(*vp)) vl= strtol(val,(char**)&val,10);
      rl=0;  if (isdigit(*rp)) rl= strtol(ref,(char**)&ref,10);
      if (vl != rl) return vl - rl;

      vc = *val;
      rc = *ref;
      vsep = strchr(".-", vc);
      rsep = strchr(".-", rc);
      if (vsep && !rsep) return -1;
      if (!vsep && rsep) return +1;

      if (!*val && !*ref) return 0;
      if (!*val) return -1;
      if (!*ref) return +1;
     }
}
