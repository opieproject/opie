/*
                             This file is part of the Opie Project

                             Copyright (C)2004, 2005 Dan Williams <drw@handhelds.org>
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

#include "oconfitem.h"
#include "opackage.h"

#include <qpe/config.h>

#include <qobject.h>

// Ipkg execution options (m_ipkgExecOptions)
#define FORCE_DEPENDS                           0x0001
#define FORCE_REMOVE                            0x0002
#define FORCE_REINSTALL                         0x0004
#define FORCE_OVERWRITE                         0x0008
#define FORCE_RECURSIVE                         0x0010
#define FORCE_VERBOSE_WGET                      0x0020

class OConfItemList;

class OIpkg : public QObject
{
    Q_OBJECT

public:
    OIpkg( Config *config = 0l, QObject *parent = 0l, const char *name = 0l );
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

    OConfItem    *findConfItem( OConfItem::Type type = OConfItem::NotDefined,
                                const QString &name = QString::null );

    bool executeCommand( OPackage::Command command = OPackage::NotDefined,
                         const QStringList &parameters = QStringList(),
                         const QString &destination = QString::null,
                         const QObject *receiver = 0l,
                         const char *slotOutput = 0l,
                         bool rawOutput = true );
    void abortCommand();

    void ipkgMessage( char *msg );
    void ipkgStatus( char *status );
    void ipkgList( char *filelist );

private:
    Config        *m_config;            // Pointer to application configuration file
    OConfItemList *m_confInfo;          // Contains info from all Ipkg configuration files
    int            m_ipkgExecOptions;   // Bit-mapped flags for Ipkg execution options
    int            m_ipkgExecVerbosity; // Ipkg execution verbosity level
    QString        m_rootPath;          // Directory path where the 'root' destination is located

    void           loadConfiguration();
    OConfItemList *filterConfItems( OConfItem::Type typefilter = OConfItem::NotDefined );
    const QString &rootPath();
    void           linkPackageDir( const QString &dest = QString::null );
    void           unlinkPackage( const QString &package = QString::null,
                                  OConfItemList *destList = 0l );

signals:
    void signalIpkgMessage( const QString &msg );
    void signalIpkgStatus( const QString &status );
    void signalIpkgList( const QString &filelist );
};

#endif
