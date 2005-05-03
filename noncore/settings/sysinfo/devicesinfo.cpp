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
#include <opie2/olayout.h>
#include <opie2/olistview.h>
#include <qpe/qpeapplication.h>
using namespace Opie::Core;
using namespace Opie::Ui;

/* QT */
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
}


//=================================================================================================
DevicesInfo::DevicesInfo( QWidget* parent,  const char* name, WFlags fl )
            :QWidget( parent, name, fl )
{
    OAutoBoxLayout *layout = new OAutoBoxLayout( this );
    layout->setSpacing( 4 );
    layout->setMargin( 4 );
    view = new DevicesView( this );
    layout->addWidget( view );
}

DevicesInfo::~DevicesInfo()
{
}


Category::Category( DevicesView* parent, const QString& name )
         :OListViewItem( parent, name )
{
    odebug << "Category '" << name << "' inserted. Scanning for devices..." << oendl;
}

Category::~Category()
{
}

//=================================================================================================
Device::Device( Category* parent, const QString& name )
         :OListViewItem( parent, name )
{
}

Device::~Device()
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
        new OListViewItem( this, "ERROR: /proc/cpuinfo not found or unaccessible" );
        return;
    }
    QTextStream cpuinfo( &cpuinfofile );

    int cpucount = 0;
    while ( !cpuinfo.atEnd() )
    {
        QString line = cpuinfo.readLine();
        odebug << "got line '" << line << "'" << oendl;
        if ( line.startsWith( "processor" ) )
        {
            new OListViewItem( this, QString( "CPU #%1" ).arg( cpucount++ ) );
        }
        else
        {
            continue;
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
            OInputDevice* dev = it.current();
        new OListViewItem( this, dev->identity() );
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
    QString fileName;
    if ( QFile::exists( "/var/run/stab" ) ) { fileName = "/var/run/stab"; }
    else if ( QFile::exists( "/var/state/pcmcia/stab" ) ) { fileName = "/var/state/pcmcia/stab"; }
    else { fileName = "/var/lib/pcmcia/stab"; }
    QFile cardinfofile( fileName );
    if ( !cardinfofile.exists() || !cardinfofile.open( IO_ReadOnly ) )
    {
        new OListViewItem( this, "ERROR: pcmcia info file not found or unaccessible" );
        return;
    }
    QTextStream cardinfo( &cardinfofile );
    while ( !cardinfo.atEnd() )
    {
        QString line = cardinfo.readLine();
        odebug << "got line '" << line << "'" << oendl;
        if ( line.startsWith("Socket") )
        {
            new OListViewItem( this, line );
        }
        else
        {
            continue;
        }
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
        new OListViewItem( this, "ERROR: /proc/bus/usb/devices not found or unaccessible" );
        return;
    }
    QTextStream usbinfo( &usbinfofile );

    int _bus, _level, _parent, _port, _count, _device, _channels, _power;
    float _speed;
    QString _manufacturer, _product, _serial;

    int usbcount = 0;
    while ( !usbinfo.atEnd() )
    {
        QString line = usbinfo.readLine();
        odebug << "got line '" << line << "'" << oendl;
        if ( line.startsWith("T:") )
        {
            sscanf(line.local8Bit().data(), "T:  Bus=%2d Lev=%2d Prnt=%2d Port=%d Cnt=%2d Dev#=%3d Spd=%3f MxCh=%2d", &_bus, &_level, &_parent, &_port, &_count, &_device, &_speed, &_channels);

            new OListViewItem( this, QString( "USB Device #%1" ).arg( usbcount++ ) );
        }
        else
        {
            continue;
        }
    }
}

