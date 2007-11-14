/*
                             This file is part of the Opie Project
                             Copyright (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the license.
     ._= =}       :
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

#include "obluetooth.h"

/* OPIE */
#include <opie2/odebug.h>
using namespace Opie::Core;

/* STD */
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>

namespace Opie {
namespace Bluez {

/*======================================================================================
 * OBluetooth
 *======================================================================================*/

OBluetooth* OBluetooth::_instance = 0;

OBluetooth::OBluetooth()
{
    synchronize();
}

OBluetooth* OBluetooth::instance()
{
    if ( !_instance ) _instance = new OBluetooth();
    return _instance;
}

OBluetooth::InterfaceIterator OBluetooth::iterator() const
{
    return OBluetooth::InterfaceIterator( _interfaces );
}

int OBluetooth::count() const
{
    return _interfaces.count();
}

OBluetoothInterface* OBluetooth::interface( const QString& iface ) const
{
    return _interfaces[iface];
}

void OBluetooth::synchronize()
{
    odebug << "OBluetooth::synchronize() - gathering available HCI devices" << oendl;
    _interfaces.clear();

    _fd = ::socket( AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI );
    if ( _fd == -1 )
    {
        owarn << "OBluetooth::synchronize() - can't open HCI control socket (" << strerror( errno ) << ")" << oendl;
        return;
    }

    struct hci_dev_list_req *dl;
    struct hci_dev_req *dr;
    struct hci_dev_info di;

    if (!(dl = (struct hci_dev_list_req*)malloc(HCI_MAX_DEV * sizeof(struct hci_dev_req) + sizeof(uint16_t))))
    {
        ofatal << "OBluetooth::synchronize() - can't allocate memory for HCI request" << oendl;
        return;
    }

    dl->dev_num = HCI_MAX_DEV;
    dr = dl->dev_req;

    if (::ioctl( _fd, HCIGETDEVLIST, (void *) dl) == -1)
    {
        owarn << "OBluetooth::synchronize() - can't complete HCIGETDEVLIST (" << strerror( errno ) << ")" << oendl;
	free(dl);
        return;
    }

    for ( int i = 0; i < dl->dev_num; ++i )
    {
        di.dev_id = ( dr + i )->dev_id;
        if ( ::ioctl( _fd, HCIGETDEVINFO, (void *) &di ) == -1 )
        {
            owarn << "OBluetooth::synchronize() - can't issue HCIGETDEVINFO on device " << i << " (" << strerror( errno ) << ") - skipping that device. " << oendl;
            continue;
        }
        odebug << "OBluetooth::synchronize() - found device #" << di.dev_id << oendl;
        _interfaces.insert( di.name, new OBluetoothInterface( this, di.name, (void*) &di, _fd ) );
    }
    free(dl);
}

/*======================================================================================
 * OBluetoothInterface
 *======================================================================================*/

class OBluetoothInterface::Private
{
  public:
    Private( struct hci_dev_info* di, int fd )
    {
        ::memcpy( &devinfo, di, sizeof(struct hci_dev_info) );
        ctlfd = fd;
    }
    void reloadInfo()
    {
        int result = ::ioctl( ctlfd, HCIGETDEVINFO, (void *) &devinfo );
        if ( result == -1 )
        {
            owarn << "OBluetoothInterface::Private - can't reload device info (" << strerror( errno ) << ")" << oendl;
        }
    }
    struct hci_dev_info devinfo;
    int ctlfd;
};

OBluetoothInterface::OBluetoothInterface( QObject* parent, const char* name, void* devinfo, int ctlfd )
                    :QObject( parent, name )
{
    d = new OBluetoothInterface::Private( (struct hci_dev_info*) devinfo, ctlfd );
}

OBluetoothInterface::~OBluetoothInterface()
{
}

QString OBluetoothInterface::macAddress() const
{
    return QString().sprintf( "%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X",
                              d->devinfo.bdaddr.b[5],
                              d->devinfo.bdaddr.b[4],
                              d->devinfo.bdaddr.b[3],
                              d->devinfo.bdaddr.b[2],
                              d->devinfo.bdaddr.b[1],
                              d->devinfo.bdaddr.b[0] );
}

bool OBluetoothInterface::setUp( bool b )
{
    int cmd = b ? HCIDEVUP : HCIDEVDOWN;
    int result = ::ioctl( d->ctlfd, cmd, d->devinfo.dev_id );
    if ( result == -1 && errno != EALREADY )
    {
        owarn << "OBluetoothInterface::setUp( " << b << " ) - couldn't change interface state (" << strerror( errno ) << ")" << oendl;
        return false;
    }
    else
    {
        d->reloadInfo();
        return true;
    }
}

bool OBluetoothInterface::isUp() const
{
    return hci_test_bit( HCI_UP, &d->devinfo.flags );
}

OBluetoothInterface::DeviceIterator OBluetoothInterface::neighbourhood()
{
    _devices.clear();
    struct hci_inquiry_req* ir;
    int nrsp = 255;

    char* mybuffer = static_cast<char*>( malloc( sizeof( *ir ) + ( sizeof( inquiry_info ) * (nrsp) ) ) );
    assert( mybuffer );

    ir = (struct hci_inquiry_req*) mybuffer;
    memset( ir, 0, sizeof( *ir ) + ( sizeof( inquiry_info ) * (nrsp) ) );

    ir->dev_id  = d->devinfo.dev_id;
    ir->num_rsp = nrsp;
    ir->length  = 8; // 10 seconds
    ir->flags   = 0;
    ir->lap[0] = 0x33; // GIAC
    ir->lap[1] = 0x8b;
    ir->lap[2] = 0x9e;

    int result = ::ioctl( d->ctlfd, HCIINQUIRY, mybuffer );
    if ( result == -1 )
    {
        owarn << "OBluetoothInterface::neighbourhood() - can't issue HCIINQUIRY (" << strerror( errno ) << ")" << oendl;
	free(mybuffer);
        return DeviceIterator( _devices );
    }

    inquiry_info* ii = (inquiry_info*)( ir+1 );

    for ( int i = 0; i < ir->num_rsp; ++i )
    {
        OBluetoothDevice* dev = new OBluetoothDevice( this, 0, ii );
        _devices.insert( dev->macAddress(), dev );
        ++ii;
    }

    free(mybuffer);

    return DeviceIterator( _devices );
}

/*======================================================================================
 * OBluetoothDevice
 *======================================================================================*/

class OBluetoothDevice::Private
{
  public:
    Private( inquiry_info* ii )
    {
        ::memcpy( &inqinfo, ii, sizeof(inquiry_info) );
    }
    inquiry_info inqinfo;
};

OBluetoothDevice::OBluetoothDevice( QObject* parent, const char* name, void* inqinfo )
                 :QObject( parent, name )
{
    odebug << "OBluetoothDevice::OBluetoothDevice() - '" << name << "'" << oendl;
    d = new OBluetoothDevice::Private( (inquiry_info*) inqinfo );
}

OBluetoothDevice::~OBluetoothDevice()
{
    odebug << "OBluetoothDevice::~OBluetoothDevice()" << oendl;
}

QString OBluetoothDevice::macAddress() const
{
    return QString().sprintf( "%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X",
    d->inqinfo.bdaddr.b[5],
    d->inqinfo.bdaddr.b[4],
    d->inqinfo.bdaddr.b[3],
    d->inqinfo.bdaddr.b[2],
    d->inqinfo.bdaddr.b[1],
    d->inqinfo.bdaddr.b[0] );
}

QString OBluetoothDevice::deviceClass() const
{
    int maj = d->inqinfo.dev_class[1] & 0x1f;
    int min = d->inqinfo.dev_class[0] >> 2;

    QString major = QString( "Unknown(%1)" ).arg( maj );
    QString minor = QString( "Unknown(%1)" ).arg( min );

    switch ( maj )
    {
        case 0: major = "Miscellaneous";
        break;

        case 1: major = "Computer";
        switch ( min )
        {
            case 0: minor = "Uncategorized";
            case 1: minor = "Desktop workstation";
            case 2: minor = "Server";
            case 3: minor = "Laptop";
            case 4: minor = "Handheld";
            case 5: minor = "Palm";
            case 6: minor = "Wearable";
        }
        break;

        case 2: major = "Phone";
        switch ( min )
        {
            case 0: minor = "Uncategorized"; break;
            case 1: minor = "Cellular"; break;
            case 2: minor = "Cordless"; break;
            case 3: minor = "Smart phone"; break;
            case 4: minor = "Wired modem or voice gateway"; break;
            case 5: minor = "Common ISDN Access"; break;
            case 6: minor = "Sim Card Reader"; break;
        }
        break;

        case 3: major = "LAN Access";
        break;

        case 4: major = "Audio/Video";
        switch ( min )
        {
            case 0: minor = "Uncategorized"; break;
            case 1: minor = "Device conforms to the Headset profile"; break;
            case 2: minor = "Hands-free"; break;
            case 3: minor = "Reserved(3)"; break;
            case 4: minor = "Microphone"; break;
            case 5: minor = "Loudspeaker"; break;
            case 6: minor = "Headphones"; break;
            case 7: minor = "Portable Audio"; break;
            case 8: minor = "Car Audio"; break;
            case 9: minor = "Set-top box"; break;
            case 10: minor = "HiFi Audio Device"; break;
            case 11: minor = "VCR"; break;
            case 12: minor = "Video Camera"; break;
            case 13: minor = "Camcorder"; break;
            case 14: minor = "Video Monitor"; break;
            case 15: minor = "Video Display and Loudspeaker"; break;
            case 16: minor = "Video Conferencing"; break;
            case 17: minor = "Reserved(17)"; break;
            case 18: minor = "Gaming/Toy"; break;
        }
        break;

        case 5: major = "Peripheral";
        switch ( min )
        {
            case 16: minor = "Keyboard"; break;
            case 32: minor = "Pointing Device"; break;
            case 48: minor = "Keyboard and Pointing Device"; break;
        }
        break;

        case 6: major = "Imaging";
        if (min & 4) minor = "Display";
        else if (min & 8) minor = "Camera";
        else if (min & 16) minor = "Scanner";
        else if (min & 32) minor = "Printer";
        break;

        case 63: major = "Uncategorized";
        break;
    }

    return QString( "%1:%2" ).arg( major ).arg( minor );
}

}
}
