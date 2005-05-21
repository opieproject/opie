/*
                            This file is part of the Opie Project
             =.             Copyright (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
           .=l.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
          .>+-=
_;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=         redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :          the terms of the GNU General Public
.="- .-=="i,     .._        License as published by the Free Software
- .   .-<_>     .<>         Foundation; version 2 of the License.
    ._= =}       :
   .%`+i>       _;_.
   .i_,=:_.      -<s.       This program is distributed in the hope that
    +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
   : ..    .:,     . . .    without even the implied warranty of
   =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
 _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;     General Public License for more
++=   -.     .`     .:      details.
:     =  ...= . :.=-
-.   .:....=;==+<;          You should have received a copy of the GNU
 -_. . .   )=.  =           General Public License along with
   --        :-=`           this application; see the file LICENSE.GPL.
                            If not, write to the Free Software Foundation,
                            Inc., 59 Temple Place - Suite 330,
                            Boston, MA 02111-1307, USA.
*/

#include "devicesinfo.h"
/* OPIE */
#include <opie2/odebug.h>
#include <opie2/oinputsystem.h>
#include <opie2/opcmciasystem.h>
#include <opie2/olayout.h>
#include <opie2/olistview.h>
#include <qpe/qpeapplication.h>
using namespace Opie::Core;
using namespace Opie::Ui;

/* QT */
#include <qobjectlist.h>
#include <qlistview.h>
#include <qcombobox.h>
#include <qfile.h>
#include <qpushbutton.h>
#include <qtextstream.h>
#include <qtextview.h>
#include <qtimer.h>
#include <qwhatsthis.h>

//=================================================================================================
DevicesView::DevicesView( QWidget* parent,  const char* name, WFlags fl )
            :Opie::Ui::OListView( parent, name, fl )
{
    addColumn( tr( "My Computer" ) );
    setAllColumnsShowFocus( true );
    setRootIsDecorated( true );
    QWhatsThis::add( this, tr( "This is a list of all the devices currently recognized on this device." ) );

    DevicesView* root = this;
    ( new CpuCategory( root ) )->populate();
    ( new InputCategory( root ) )->populate();
    ( new CardsCategory( root ) )->populate();
    ( new UsbCategory( root ) )->populate();

    connect( this, SIGNAL(selectionChanged(QListViewItem*)), this, SLOT(selectionChanged(QListViewItem*)) );
}

DevicesView::~DevicesView()
{
}


void DevicesView::selectionChanged( QListViewItem* item )
{
    odebug << "DevicesView::selectionChanged to '" << item->text( 0 ) << "'" << oendl;
    if ( item->parent() )
    {
        QWidget* details = ( static_cast<Device*>( item ) )->detailsWidget();
        ( static_cast<DevicesInfo*>( parent() ) )->setDetailsWidget( details );
    }
    else
    {
        odebug << "DevicesView::not a device node." << oendl;
    }
}


//=================================================================================================
DevicesInfo::DevicesInfo( QWidget* parent,  const char* name, WFlags fl )
            :QWidget( parent, name, fl ), details( 0 )
{
    layout = new OAutoBoxLayout( this );
    layout->setSpacing( 2 );
    layout->setMargin( 2 );
    view = new DevicesView( this );
    layout->addWidget( view, 100 );
    stack = new QWidgetStack( this );
    layout->addWidget( stack, 70 );
}


DevicesInfo::~DevicesInfo()
{
}


void DevicesInfo::setDetailsWidget( QWidget* w )
{
    if ( details )
    {
        qDebug( "hiding widget '%s' ('%s')", details->name(), details->className() );
        stack->removeWidget( w );
    }

    stack->addWidget( details = w, 40 );
    stack->raiseWidget( details );
}


//=================================================================================================
Category::Category( DevicesView* parent, const QString& name )
         :OListViewItem( parent, name )
{
    odebug << "Category '" << name << "' inserted. Scanning for devices..." << oendl;
}

Category::~Category()
{
}

//=================================================================================================
CpuCategory::CpuCategory( DevicesView* parent )
         :Category( parent, "1. Central Processing Unit" )
{
}

CpuCategory::~CpuCategory()
{
}

void CpuCategory::populate()
{
    odebug << "CpuCategory::populate()" << oendl;
    QFile cpuinfofile( "/proc/cpuinfo" );
    if ( !cpuinfofile.exists() || !cpuinfofile.open( IO_ReadOnly ) )
    {
        new CpuDevice( this, "ERROR: /proc/cpuinfo not found or unaccessible" );
        return;
    }
    QTextStream cpuinfo( &cpuinfofile );

    int cpucount = 0;
    CpuDevice* dev = 0;

    while ( !cpuinfo.atEnd() )
    {
        QString line = cpuinfo.readLine();
        odebug << "got line '" << line << "'" << oendl;
        if ( line.lower().startsWith( "processor" ) )
        {
            dev = new CpuDevice( this, QString( "CPU #%1" ).arg( cpucount++ ) );
            dev->addInfo( line );
        }
        else
        {
            if ( dev ) dev->addInfo( line );
        }
    }
}

//=================================================================================================
InputCategory::InputCategory( DevicesView* parent )
         :Category( parent, "2. Input Subsystem" )
{
}

InputCategory::~InputCategory()
{
}

void InputCategory::populate()
{
    odebug << "InputCategory::populate()" << oendl;
    OInputSystem* sys = OInputSystem::instance();
    OInputSystem::DeviceIterator it = sys->iterator();
    while ( it.current() )
    {
        new InputDevice( this, it.current()->identity() );
        ++it;
    }
}

//=================================================================================================
CardsCategory::CardsCategory( DevicesView* parent )
         :Category( parent, "3. Removable Cards" )
{
}

CardsCategory::~CardsCategory()
{
}

void CardsCategory::populate()
{
    odebug << "CardsCategory::populate()" << oendl;
    OPcmciaSystem* sys = OPcmciaSystem::instance();
    OPcmciaSystem::CardIterator it = sys->iterator();
    while ( it.current() )
    {
        new CardDevice( this, it.current()->identity() );
        ++it;
    }
}

//=================================================================================================
UsbCategory::UsbCategory( DevicesView* parent )
         :Category( parent, "4. Universal Serial Bus" )
{
}

UsbCategory::~UsbCategory()
{
}

void UsbCategory::populate()
{
    odebug << "UsbCategory::populate()" << oendl;
    QFile usbinfofile( "/proc/bus/usb/devices" );
    if ( !usbinfofile.exists() || !usbinfofile.open( IO_ReadOnly ) )
    {
        new UsbDevice( this, "ERROR: /proc/bus/usb/devices not found or unaccessible" );
        return;
    }
    QTextStream usbinfo( &usbinfofile );

    int _bus, _level, _parent, _port, _count, _device, _channels, _power;
    float _speed;
    QString _manufacturer, _product, _serial;

    int usbcount = 0;
    UsbDevice* lastDev = 0;
    UsbDevice* dev = 0;
    while ( !usbinfo.atEnd() )
    {
        QString line = usbinfo.readLine();
        odebug << "got line '" << line << "'" << oendl;
        if ( line.startsWith( "T:" ) )
        {
            sscanf(line.local8Bit().data(), "T:  Bus=%2d Lev=%2d Prnt=%2d Port=%d Cnt=%2d Dev#=%3d Spd=%3f MxCh=%2d", &_bus, &_level, &_parent, &_port, &_count, &_device, &_speed, &_channels);

            if ( !_level )
            {
                odebug << "adding new bus" << oendl;
                dev = new UsbDevice( this, QString( "Generic USB Hub Device" ) );
                lastDev = dev;
            }
            else
            {
                odebug << "adding new dev" << oendl;
                dev = new UsbDevice( lastDev, QString( "Generic USB Hub Device" ) );
                lastDev = dev;
            }
        }
        else if ( line.startsWith( "S:  Product" ) )
        {
            int dp = line.find( '=' );
            dev->setText( 0, dp != -1 ? line.right( line.length()-1-dp ) : "<unknown>" );
        }
        else
        {
            continue;
        }
    }
}


//=================================================================================================
Device::Device( Category* parent, const QString& name )
       :OListViewItem( parent, name )
{
    devinfo = static_cast<QWidget*>( listView()->parent() );
}

Device::Device( Device* parent, const QString& name )
       :OListViewItem( parent, name )
{
    devinfo = static_cast<QWidget*>( listView()->parent() );
}

Device::~Device()
{
}


QWidget* Device::detailsWidget()
{
    return details;
}

//=================================================================================================
CpuDevice::CpuDevice( Category* parent, const QString& name )
          :Device( parent, name )
{
    OListView* w = new OListView( devinfo );
    details = w;
    w->addColumn( "Info" );
    w->addColumn( "Value" );
    w->hide();
}

CpuDevice::~CpuDevice()
{
}

void CpuDevice::addInfo( const QString& info )
{
    int dp = info.find( ':' );
    if ( dp != -1 )
    {
        new OListViewItem( (OListView*) details, info.left( dp ), info.right( info.length()-dp ) );
    }
}

//=================================================================================================
CardDevice::CardDevice( Category* parent, const QString& name )
           :Device( parent, name )
{
    details = new QPushButton( name, devinfo );
    details->hide();
}

CardDevice::~CardDevice()
{
}

//=================================================================================================
InputDevice::InputDevice( Category* parent, const QString& name )
            :Device( parent, name )
{
    details = new QPushButton( name, devinfo );
    details->hide();
}

InputDevice::~InputDevice()
{
}

//=================================================================================================
UsbDevice::UsbDevice( Category* parent, const QString& name )
          :Device( parent, name )
{
    details = new QPushButton( name, devinfo );
    details->hide();
}

//=================================================================================================
UsbDevice::UsbDevice( UsbDevice* parent, const QString& name )
          :Device( parent, name )
{
    details = new QPushButton( name, devinfo );
    details->hide();
}

UsbDevice::~UsbDevice()
{
}
