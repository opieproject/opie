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

#ifndef OIPKG_H
#define OIPKG_H

extern "C" {
#include <libipkg.h>
};

#include <qobject.h>

#include <qpe/config.h>

#include "oconfitem.h"
#include "opackage.h"

// Ipkg execution options (m_ipkgExecOptions)
#define FORCE_DEPENDS                           0x0001
#define FORCE_REMOVE                            0x0002
#define FORCE_REINSTALL                         0x0004
#define FORCE_OVERWRITE                         0x0008

class OConfItemList;

class OIpkg : public QObject
{
    Q_OBJECT

public:
    OIpkg( Config *config = 0x0, QObject *parent = 0x0, const char *name = 0x0 );
    ~OIpkg();

    OConfItemList *configItems();
    OConfItemList *servers();
    OConfItemList *destinations();
    OConfItemList *options();

    int  ipkgExecOptions() { return m_ipkgExecOptions; }
    int  ipkgExecVerbosity() { return m_ipkgExecVerbosity; }

    void setConfigItems( OConfItemList *configList );
    void setIpkgExecOptions( int options ) { m_ipkgExecOptions = options; }
    void setIpkgExecVerbosity( int verbosity ) { m_ipkgExecVerbosity = verbosity; }

    void saveSettings();

    OPackageList *availablePackages( const QString &server = QString::null );
    OPackageList *installedPackages( const QString &destName = QString::null,
                                     const QString &destPath = QString::null );

    bool executeCommand( OPackage::Command command = OPackage::NotDefined, QStringList *parameters = 0x0,
                         const QString &destination = QString::null, const QObject *receiver = 0x0,
                         const char *slotOutput = 0x0, bool rawOutput = true );
    void abortCommand();

    void ipkgMessage( char *msg );
    void ipkgStatus( char *status );
    void ipkgList( char *filelist );

private:
    Config        *m_config;            // Pointer to application configuration file
    args_t         m_ipkgArgs;          // libipkg configuration arguments
    OConfItemList *m_confInfo;          // Contains info from all Ipkg configuration files
    int            m_ipkgExecOptions;   // Bit-mapped flags for Ipkg execution options
    int            m_ipkgExecVerbosity; // Ipkg execution verbosity level

    void           loadConfiguration();
    OConfItemList *filterConfItems( OConfItem::Type typefilter = OConfItem::NotDefined );

signals:
    void signalIpkgMessage( char *msg );
    void signalIpkgStatus( char *status );
    void signalIpkgList( char *filelist );
};

#endif
