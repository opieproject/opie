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

#include <qfile.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qscrollview.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qwhatsthis.h>

#include "versioninfo.h"

#include <opie/odevice.h>

using namespace Opie;

VersionInfo::VersionInfo( QWidget *parent, const char *name, WFlags f )
    : QWidget( parent, name, f )
{
    setMinimumSize( 200, 150 );

    QVBoxLayout *tmpvb = new QVBoxLayout( this );
    QScrollView *sv = new QScrollView( this );
    tmpvb->addWidget( sv, 0, 0 );
    sv->setResizePolicy( QScrollView::AutoOneFit );
    sv->setFrameStyle( QFrame::NoFrame );
    QWidget *container = new QWidget( sv->viewport() );
    sv->addChild( container );

    QVBoxLayout *vb = new QVBoxLayout( container, 3 );

    QString kernelVersionString;
    QFile file( "/proc/version" );
    if ( file.open( IO_ReadOnly ) )
    {
        QTextStream t( &file );
        QString v;
        t >> v; t >> v; t >> v;
        v = v.left( 20 );
        kernelVersionString = "<qt>" + tr( "<b>Linux Kernel</b><p>Version: " );
        kernelVersionString.append( v );
        kernelVersionString.append( "<br>" );
        t >> v;
        kernelVersionString.append( tr( "Compiled by: " ) );
        kernelVersionString.append( v );
        kernelVersionString.append("</qt>");
        file.close();
    }

    QString palmtopVersionString = "<qt>" + tr( "<b>Opie</b><p>Version: " );
    palmtopVersionString.append( QPE_VERSION );
    palmtopVersionString.append( "<br>" );
#ifdef QPE_VENDOR
    QString builder = QPE_VENDOR;
#else
    QString builder = "Unknown";
#endif
    palmtopVersionString.append( tr( "Compiled by: " ) );
    palmtopVersionString.append(  builder );
    palmtopVersionString.append( "<br>" );
    palmtopVersionString.append( tr( "Built on: " ) );
    palmtopVersionString.append( __DATE__  );
    palmtopVersionString.append( "</qt>" );

    QHBoxLayout *hb1 = new QHBoxLayout( vb );
    hb1->setSpacing( 2 );

    QLabel *palmtopLogo = new QLabel( container );
    QImage logo1 = Resource::loadImage( "logo/opielogo" );
    logo1 = logo1.smoothScale( 50, 55 );
    QPixmap logo1Pixmap;
    logo1Pixmap.convertFromImage( logo1 );
    palmtopLogo->setPixmap( logo1Pixmap );
    palmtopLogo->setFixedSize( 60, 60 );
    hb1->addWidget( palmtopLogo, 0, Qt::AlignTop + Qt::AlignLeft );

    QLabel *palmtopVersion = new QLabel( container );
    palmtopVersion->setText( palmtopVersionString  );
    hb1->addWidget( palmtopVersion, 1, Qt::AlignTop + Qt::AlignLeft );


    QHBoxLayout *hb2 = new QHBoxLayout( vb );
    hb1->setSpacing( 2 );

    QLabel *linuxLogo = new QLabel( container );
    QImage logo2 = Resource::loadImage( "logo/tux-logo" );
    logo2 = logo2.smoothScale( 55, 60 );
    QPixmap logo2Pixmap;
    logo2Pixmap.convertFromImage( logo2 );
    linuxLogo->setPixmap( logo2Pixmap );
    linuxLogo->setFixedSize( 60, 60 );
    hb2->addWidget( linuxLogo, 0, Qt::AlignTop + Qt::AlignLeft );

    QLabel *kernelVersion = new QLabel( container );
    kernelVersion->setText( kernelVersionString );
    hb2->addWidget( kernelVersion, 1, Qt::AlignTop + Qt::AlignLeft );


    QHBoxLayout *hb3 = new QHBoxLayout( vb );
    hb3->setSpacing( 2 );

    QLabel *palmtopLogo3 = new QLabel( container );

    OModel model = ODevice::inst()->model();
    QString modelPixmap = "sysinfo/";
    if ( model == Model_Zaurus_SLC7x0 )
        modelPixmap += "zaurusc700";
    else if ( model >= Model_Zaurus_SL5000 && model <= Model_Zaurus_SLB600 )
        modelPixmap += "zaurus5500";
    else if ( model >= Model_iPAQ_H31xx && model <= Model_iPAQ_H5xxx )
        modelPixmap += "ipaq3600";
    else if ( model >= Model_SIMpad_CL4 && model <= Model_SIMpad_TSinus )
        modelPixmap += "simpad";
    else
        modelPixmap += "pda";

    QImage logo3 = Resource::loadImage( modelPixmap );

    int width = logo3.width();
    int height = logo3.height();
    float aspect = float( height ) / width;
    logo3 = logo3.smoothScale( 50, 50.0 * aspect );

    QPixmap logo3Pixmap;
    logo3Pixmap.convertFromImage( logo3 );
    palmtopLogo3->setPixmap( logo3Pixmap );
    palmtopLogo3->setFixedSize( 60, 100 );
    hb3->addWidget( palmtopLogo3, 0, Qt::AlignTop + Qt::AlignLeft );

    QString systemString = "<qt><b>";
    systemString.append( ODevice::inst()->systemString() );
    systemString.append( "</b>" );
    systemString.append( tr( "<p>Version: " ) );
    systemString.append( ODevice::inst()->systemVersionString() );
    systemString.append( tr( "<br>Model: " ) );
    systemString.append( ODevice::inst()->modelString() );
    systemString.append( tr( "<br>Vendor: " ) );
    systemString.append( ODevice::inst()->vendorString() );
    systemString.append("</qt>");

    QLabel *systemVersion = new QLabel( container );
    systemVersion->setText( systemString );
    hb3->addWidget( systemVersion, 1, Qt::AlignTop + Qt::AlignLeft );

    QWhatsThis::add( this, tr( "This page shows the current versions of Opie, the Linux kernel and distribution running on this handheld device." ) );
}

VersionInfo::~VersionInfo()
{
}

