/*
                            This file is part of the OPIE Project

               =.            Copyright (c)  2003 Dan Williams <drw@handhelds.org>
             .=l.
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

#include "packageinfodlg.h"
#include "opackage.h"
#include "opackagemanager.h"

#include <opie2/otabwidget.h>

#include <qpe/resource.h>

#include <qlayout.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>

PackageInfoDlg::PackageInfoDlg( QWidget *parent, OPackageManager *pm, const QString &package )
    : QWidget( 0x0 )
    , m_packman( pm )
    , m_information( this )
    , m_files( this )
{
    // Initialize UI
    if ( parent )
        parent->setCaption( package );

    QVBoxLayout *layout = new QVBoxLayout( this, 4, 2 );

    Opie::Ui::OTabWidget *tabWidget = new Opie::Ui::OTabWidget( this );
    layout->addWidget( tabWidget );

    // Information tab
    QWhatsThis::add( &m_information, tr( "This area contains information about the package." ) );
    m_information.reparent( tabWidget, QPoint( 0, 0 ) );
    m_information.setReadOnly( true );
    tabWidget->addTab( &m_information, "UtilsIcon", tr( "Information" ) );

    // Retrive package information
    m_package = m_packman->findPackage( package );
    if ( !m_package )
    {
        m_information.setText( tr( "Unable to retrieve package information." ) );
        return;
    }

    // Display package information
    if ( !m_package->information().isNull() )
        m_information.setText( m_package->information() );
    else
    {
        // Package information is not cached, retrieve it
        QStringList list( package );
        m_packman->executeCommand( OPackage::Info, &list, QString::null, this, SLOT(slotInfo(char*)), true );
    }

    // Files tab (display only if package is installed)
    if ( !m_package->versionInstalled().isNull() )
    {
        QWidget *filesWidget = new QWidget( tabWidget );
        QVBoxLayout *filesLayout = new QVBoxLayout( filesWidget, 2, 2 );
        QWhatsThis::add( &m_files, tr( "This area contains a list of files contained in this package." ) );
        m_files.reparent( filesWidget, QPoint( 0, 0 ) );
        m_files.setReadOnly( true );
        filesLayout->addWidget( &m_files );

        QPushButton *btn = new QPushButton( Resource::loadPixmap( "packagemanager/apply" ),
                                            tr( "Retrieve file list" ), filesWidget );
        QWhatsThis::add( btn, tr( "Tap here to retrieve list of files contained in this package." ) );
        filesLayout->addWidget( btn );
        connect( btn, SIGNAL(clicked()), this, SLOT(slotBtnFileScan()) );
        tabWidget->addTab( filesWidget, "binary", tr( "File list" ) );

        tabWidget->setCurrentTab( tr( "Information" ) );

        // If file list is already cached, display
        if ( !m_package->files().isNull() )
            m_files.setText( m_package->files() );
    }
    else
        m_files.hide();
}

PackageInfoDlg::~PackageInfoDlg()
{
    if ( !m_package )
        return;

    // Cache package information
    if ( !m_information.text().isNull() )
        m_package->setInformation( m_information.text() );

    // Cache package file list
    if ( !m_files.text().isNull() )
        m_package->setFiles( m_files.text() );
}

void PackageInfoDlg::slotBtnFileScan()
{
    m_files.clear();

    QStringList list( m_package->name() );
    m_packman->executeCommand( OPackage::Files, &list, QString::null, this, SLOT(slotFiles(char*)), true );
}

void PackageInfoDlg::slotInfo( char *info )
{
    m_information.append( info );
}

void PackageInfoDlg::slotFiles( char *filelist )
{
    QString str = filelist;

    // Skip first line of output ("Package xxx is installed...")
    if ( str.startsWith( "Package " ) )
        str = str.right( str.length() - str.find( '\n' ) - 1 );

    m_files.append( str );
}
