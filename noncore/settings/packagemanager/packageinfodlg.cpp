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

#include <qlayout.h>
#include <qpushbutton.h>

#include <qpe/resource.h>

#include <opie/otabwidget.h>

PackageInfoDlg::PackageInfoDlg( QWidget *parent, OPackageManager *pm, const QString &package )
    : QWidget( 0x0 )
    , m_packman( pm )
    , m_information( this )
    , m_files( this )
{
    // Initialize UI
    if ( parent )
        parent->setCaption( package );

    QVBoxLayout *layout = new QVBoxLayout( this, 4, 0 );

    OTabWidget *tabWidget = new OTabWidget( this );
    layout->addWidget( tabWidget );

    QPushButton *btn = new QPushButton( Resource::loadPixmap( "enter" ), tr( "Close" ), this );
    layout->addWidget( btn );
    connect( btn, SIGNAL(clicked()), this, SLOT(slotBtnClose()) );

    // Information tab
    m_information.reparent( tabWidget, QPoint( 0, 0 ) );
    m_information.setReadOnly( true );
    tabWidget->addTab( &m_information, "UtilsIcon", tr( "Information" ) );

    // Files tab
    QWidget *filesWidget = new QWidget( tabWidget );
    QVBoxLayout *filesLayout = new QVBoxLayout( filesWidget, 4, 0 );
    m_files.reparent( filesWidget, QPoint( 0, 0 ) );
    m_files.setReadOnly( true );
    filesLayout->addWidget( &m_files );

    btn = new QPushButton( Resource::loadPixmap( "packagemanager/apply" ), tr( "Retrieve file list" ),
                           filesWidget );
    filesLayout->addWidget( btn );
// TODO    connect( btn, SIGNAL(clicked()), this, SLOT(slotFileScan()) );
    tabWidget->addTab( filesWidget, "binary", tr( "Files" ) );

    tabWidget->setCurrentTab( tr( "Information" ) );
}

void PackageInfoDlg::slotBtnClose()
{
    emit closeInfoDlg();
}
