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

#include "versioninfo.h"

/* OPIE */
#include <opie2/odevice.h>
#include <opie2/oresource.h>
#include <qpe/version.h>

/* QT */
#include <qfile.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qscrollview.h>
#include <qtextstream.h>
#include <qwhatsthis.h>

using namespace Opie::Core;

VersionInfo::VersionInfo( QWidget *parent, const char *name, WFlags f )
    : QWidget( parent, name, f )
{
    setMinimumSize( 200, 150 );

    QVBoxLayout *tmpvb = new QVBoxLayout( this );
    QScrollView *sv = new QScrollView( this );
    tmpvb->addWidget( sv, 0, 0 );
    sv->setResizePolicy( QScrollView::AutoOneFit );
    sv->setHScrollBarMode( QScrollView::AlwaysOff );
    sv->setVScrollBarMode( QScrollView::AlwaysOff );
    sv->setFrameStyle( QFrame::NoFrame );
    QWidget *container = new QWidget( sv->viewport() );
    sv->addChild( container );

    QVBoxLayout *vb = new QVBoxLayout( container, 3 );

    QString kernelVersionString;
    QFile file( "/proc/version" );
    if ( file.open( IO_ReadOnly ) )
    {
        QTextStream t( &file );
  QStringList strList;

  strList = QStringList::split(  " " , t.read(),  false );

        kernelVersionString = "<qt>" + tr( "<b>Linux Kernel</b><p>Version: " );
        kernelVersionString.append( strList[2] );
        kernelVersionString.append( "<br>" );
        kernelVersionString.append( tr( "Compiled by: " ) );
        kernelVersionString.append( strList[3] );
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
    palmtopVersionString.append( tr( "Built against Qt/E " ) );
    palmtopVersionString.append( QT_VERSION_STR );
    palmtopVersionString.append( "<br>" );
    palmtopVersionString.append( tr( "Built on: " ) );
    palmtopVersionString.append( __DATE__  );
    palmtopVersionString.append( "</qt>" );

    QHBoxLayout *hb1 = new QHBoxLayout( vb );
    hb1->setSpacing( 2 );

    QLabel *palmtopLogo = new QLabel( container );
    QImage logo1 = Opie::Core::OResource::loadImage( "logo/opielogo" );
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
    QImage logo2 = Opie::Core::OResource::loadImage( "logo/tux-logo" );
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
    OVendor vendor = ODevice::inst()->vendor();
    QString modelPixmap = "sysinfo/";

    switch ( vendor ) {
        /* SHARP */
        case Vendor_Sharp:
        {
            switch ( model ) {
                /* ZAURUS */
                case Model_Zaurus_SLC3100:
                case Model_Zaurus_SLC3000:
                case Model_Zaurus_SLC1000:
                case Model_Zaurus_SLC7x0:
                    modelPixmap += "zaurusc700";
                    break;
                case Model_Zaurus_SL5000:
                case Model_Zaurus_SL5500:
                case Model_Zaurus_SLA300:
                case Model_Zaurus_SLB600:
                    modelPixmap += "zaurus5500";
                    break;
                default:
                    modelPixmap += "pda";
                    break;
            }
            break;
        }
        /* HP */
        case Vendor_HP:
        {
            switch ( model ) {
                /* IPAQ */
                case Model_iPAQ_H31xx:
                case Model_iPAQ_H36xx:
                case Model_iPAQ_H37xx:
                    modelPixmap += "ipaq3600";
                    break;
                case Model_iPAQ_H38xx:
                case Model_iPAQ_H39xx:
                    modelPixmap += "ipaqh3800h3900";
                    break;
                case Model_iPAQ_H5xxx:
                    modelPixmap += "ipaqh5000";
                    break;
                case Model_iPAQ_H22xx:
                    modelPixmap += "ipaqh2200";
                    break;
                case Model_iPAQ_HX4700:
                    modelPixmap += "ipaqhx4700";
                    break;
                case Model_iPAQ_H4xxx:
                case Model_iPAQ_RX1950:
                    modelPixmap += "ipaqh4100";
                    break;
                case Model_iPAQ_H191x:
                case Model_iPAQ_H1940:
                    modelPixmap += "ipaqh1900";
                    break;
                /* JORNADA */
                case Model_Jornada_56x:
                    modelPixmap += "jornada56x";
                    break;
                case Model_Jornada_720:
                    modelPixmap += "jornada720";
                    break;
                default:
                    modelPixmap += "pda";
                    break;
            }
            break;
        }
        /* SIEMENS */
        case Vendor_SIEMENS:
        {
            switch ( model )
            {
                /* SIMPAD */
                case Model_SIMpad_CL4:
                case Model_SIMpad_SL4:
                case Model_SIMpad_SLC:
                case Model_SIMpad_TSinus:
                    modelPixmap += "simpad";
                    break;
                default:
                    modelPixmap += "pda";
                    break;
            }
            break;
        }
        /* HTC */
        case Vendor_HTC:
        {
            switch ( model ) {
                case Model_HTC_Universal:
                    modelPixmap += "htcuniversal";
                    break;
                case Model_HTC_Blueangel:
                    modelPixmap += "htcblueangel";
                    break;
                case Model_HTC_Apache:
                    modelPixmap += "htcapache";
                    break;
                case Model_HTC_Magician:
                    modelPixmap += "htcmagician";
                    break;
                case Model_HTC_Himalaya:
                    modelPixmap += "htchimalaya";
                    break;
                case Model_HTC_Alpine:
                    modelPixmap += "htcalpine";
                    break;
                case Model_HTC_Beetles:
                    modelPixmap += "htcbeetles";
                    break;
                default:
                    modelPixmap += "pda";
                    break;
            }
            break;
        }
        /* PALM */
        case Vendor_Palm:
        {
            switch ( model ) {
                case Model_Palm_TT:
                    modelPixmap += "palmtt";
                    break;
                case Model_Palm_TT2:
                    modelPixmap += "palmtt2";
                    break;
                case Model_Palm_TT3:
                    modelPixmap += "palmtt3";
                    break;
                case Model_Palm_TT5:
                    modelPixmap += "palmtt5";
                    break;
                case Model_Palm_TE:
                    modelPixmap += "palmte";
                    break;
                case Model_Palm_TE2:
                    modelPixmap += "palmte2";
                    break;
                case Model_Palm_TC:
                    modelPixmap += "palmtc";
                    break;
                case Model_Palm_LD:
                    modelPixmap += "palmld";
                    break;
                case Model_Palm_TX:
                    modelPixmap += "palmtx";
                    break;
                case Model_Palm_Z71:
                    modelPixmap += "palmz71";
                    break;
                case Model_Palm_Z72:
                    modelPixmap += "palmz72";
                    break;
                case Model_Palm_T600:
                    modelPixmap += "palmt600";
                    break;
                case Model_Palm_T650:
                    modelPixmap += "palmt650";
                    break;
                case Model_Palm_T680:
                    modelPixmap += "palmt680";
                    break;
                case Model_Palm_T700W:
                    modelPixmap += "palmt700w";
                    break;
                case Model_Palm_T700P:
                    modelPixmap += "palmt700p";
                    break;
                case Model_Palm_T750:
                    modelPixmap += "palmt750";
                    break;
                case Model_Palm_T755P:
                    modelPixmap += "palmt755p";
                    break;
                case Model_Palm_FOLEO:
                    modelPixmap += "palmfoleo";
                    break;
                default:
                    modelPixmap += "pda";
                    break;
            }
            break;
        }
        /* OTHER DEVICES */
        default:
            modelPixmap += "pda";
            break;
    }
    QImage logo3 = Opie::Core::OResource::loadImage( modelPixmap );

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

