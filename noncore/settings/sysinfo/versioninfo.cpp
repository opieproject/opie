/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <qpe/resource.h>
#include <qpe/version.h>


#include <qlabel.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qimage.h>
#include <qtimer.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qlayout.h>
#include "versioninfo.h"

#include <opie/odevice.h>
using namespace Opie;

VersionInfo::VersionInfo( QWidget *parent, const char *name, WFlags f )
    : QWidget( parent, name, f )
{
    setMinimumSize( 200, 150 );

    QVBoxLayout *vb = new QVBoxLayout( this, 4 );
    
    QString kernelVersionString;
    QFile file( "/proc/version" );
    if ( file.open( IO_ReadOnly ) ) {
  QTextStream t( &file );
  QString v;
  t >> v; t >> v; t >> v;
  v = v.left( 20 );
  kernelVersionString = tr( "<b>Linux Kernel</b><p>Version: " ) + v + "<p>";
  t >> v;
  kernelVersionString += tr( "Compiled by: " ) + v;
  file.close();
    }

    QString palmtopVersionString;
    palmtopVersionString = tr( "<b>Opie</b><p>Version: " ) + QPE_VERSION + "<p>";
#ifdef QPE_VENDOR
    QString builder = QPE_VENDOR;
#else
    QString builder = "Unknown";
#endif    
    palmtopVersionString += tr( "Compiled by: " ) + builder + "<p>";
    palmtopVersionString += tr( "Built on: " ) + __DATE__;

    
    QHBoxLayout *hb1 = new QHBoxLayout( vb );
    hb1->setSpacing( 2 );

    QLabel *palmtopLogo = new QLabel( this );
    QImage logo1 = Resource::loadImage( "logo/opielogo" );
    logo1 = logo1.smoothScale( 50, 55 );
    QPixmap logo1Pixmap;
    logo1Pixmap.convertFromImage( logo1 );
    palmtopLogo->setPixmap( logo1Pixmap );
    palmtopLogo->setFixedSize( 60, 60 );
    hb1->addWidget( palmtopLogo, 0, Qt::AlignTop + Qt::AlignLeft );

    QLabel *palmtopVersion = new QLabel( this );
    palmtopVersion->setText( palmtopVersionString  );
    hb1->addWidget( palmtopVersion, 1, Qt::AlignTop + Qt::AlignLeft );


    QHBoxLayout *hb2 = new QHBoxLayout( vb );
    hb1->setSpacing( 2 );

    QLabel *linuxLogo = new QLabel( this );
    QImage logo2 = Resource::loadImage( "logo/tux-logo" );
    logo2 = logo2.smoothScale( 55, 60 );
    QPixmap logo2Pixmap;
    logo2Pixmap.convertFromImage( logo2 );
    linuxLogo->setPixmap( logo2Pixmap );
    linuxLogo->setFixedSize( 60, 60 );
    hb2->addWidget( linuxLogo, 0, Qt::AlignTop + Qt::AlignLeft );
    
    QLabel *kernelVersion = new QLabel( this );
    kernelVersion->setText( kernelVersionString );
    hb2->addWidget( kernelVersion, 1, Qt::AlignTop + Qt::AlignLeft );


    QHBoxLayout *hb3 = new QHBoxLayout( vb );
    hb3->setSpacing( 2 );

    QLabel *palmtopLogo3 = new QLabel( this );
    QImage logo3 = Resource::loadImage( "SystemInfo" );
    logo3 = logo3.smoothScale( 50, 55 );
    QPixmap logo3Pixmap;
    logo3Pixmap.convertFromImage( logo3 );
    palmtopLogo3->setPixmap( logo3Pixmap );
    palmtopLogo3->setFixedSize( 60, 60 );
    hb3->addWidget( palmtopLogo3, 0, Qt::AlignTop + Qt::AlignLeft );

//    QString systemString = tr( "<b>System</b><p>System: ") + ODevice::inst()->systemString()
    QString systemString = "<b>"+ ODevice::inst()->systemString()+"</b>"
        +tr("<p>Version: " ) + ODevice::inst()->systemVersionString()
        +tr("<p>Model: ") + ODevice::inst()->modelString()
        +tr("<p>Vendor: ") + ODevice::inst()->vendorString();

    QLabel *systemVersion = new QLabel( this );
    systemVersion->setText( systemString );
    hb3->addWidget( systemVersion, 1, Qt::AlignTop + Qt::AlignLeft );
    
}

VersionInfo::~VersionInfo()
{
}

