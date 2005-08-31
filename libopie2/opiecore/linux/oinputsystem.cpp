/*
                             This file is part of the Opie Project
              =.             (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
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

#include "oinputsystem.h"
using namespace Opie::Core;

/* QT */
#include <qdir.h>
#include <qfile.h>

/* STD */
#include <errno.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define     BUFSIZE 256
#define     BIT_MASK( name, numbits )                                        \
    unsigned short  name[ ((numbits) - 1) / (sizeof( short ) * 8) + 1 ];    \
    memset( name, 0, sizeof( name ) )
#define     BIT_TEST( bitmask, bit )    \
    ( bitmask[ (bit) / sizeof(short) / 8 ] & (1u << ( (bit) % (sizeof(short) * 8))) )

/*======================================================================================
 * OInputSystem
 *======================================================================================*/

OInputSystem* OInputSystem::_instance = 0;

OInputSystem::OInputSystem() : QObject()
{
    qDebug( "OInputSystem::OInputSystem()" );
    synchronize();
}


void OInputSystem::synchronize()
{
    qDebug( "OInputSystem::synchronize()" );
    QDir devInput( "/dev/input/" );
    if ( devInput.exists() )
    {
        QStringList devInputFiles = devInput.entryList( QDir::System, QDir::Name );
        for ( QStringList::Iterator it = devInputFiles.begin(); it != devInputFiles.end(); ++it )
        {
            QString absPath = devInput.absFilePath( *it );
            bool isValid = OInputDevice::isValid( absPath );
            qDebug( "OInputSystem::synchronize() - checking if '%s' is a valid input system node... '%s' [%s]",
            (const char*) absPath, isValid ? "yes" : "no", isValid ? "(ok)" : strerror( errno ) );
            if ( isValid ) _devices.insert( *it, new OInputDevice( this, absPath ) );
        }
    }
    qDebug( "OInputSystem::synchronize() done" );
    if ( !_devices.count() )
    qWarning( "OInputSystem::no devices found" );
}


OInputSystem::~OInputSystem()
{
    qDebug( "OInputSystem::~OInputSystem()" );
}


int OInputSystem::count() const
{
    return _devices.count();
}


OInputDevice* OInputSystem::device( const QString& device ) const
{
    return _devices[device];
}


OInputSystem* OInputSystem::instance()
{
    if ( !_instance ) _instance = new OInputSystem();
    return _instance;
}


OInputSystem::DeviceIterator OInputSystem::iterator() const
{
    return OInputSystem::DeviceIterator( _devices );
}

/*======================================================================================
 * OInputDevice
 *======================================================================================*/

OInputDevice::OInputDevice( QObject* parent, const char* name ) : QObject( parent, name )
{
    qDebug( "OInputDevice::OInputDevice( '%s' )", name );

    _fd = ::open( name, O_RDONLY );
    if ( _fd == -1 )
    {
        qDebug( "OInputDevice::OInputDevice() - Warning: couldn't open %s (%s)", name, strerror( errno ) );
    }
}


OInputDevice::~OInputDevice()
{
    qDebug( "OInputDevice::~OInputDevice()" );
}


QString OInputDevice::identity() const
{
    char buf[BUFSIZE] = "<unknown>";
    ::ioctl( _fd, EVIOCGNAME(sizeof buf), buf );
    return buf;
}


QString OInputDevice::path() const
{
    char buf[BUFSIZE] = "<unknown>";
    ::ioctl( _fd, EVIOCGPHYS(sizeof buf), buf );
    return buf;
}


QString OInputDevice::uniq() const
{
    char buf[BUFSIZE] = "<unknown>";
    ::ioctl( _fd, EVIOCGUNIQ(sizeof buf), buf );
    return buf;
}


bool OInputDevice::hasFeature( Feature bit ) const
{
    BIT_MASK( features, EV_MAX );

    if( ioctl( _fd, EVIOCGBIT( 0, EV_MAX ), features) < 0 )
    {
        perror( "EVIOCGBIT" );
        return false;
    }
    else
        return BIT_TEST( features, bit );
}


bool OInputDevice::isHeld( Key bit ) const
{
    BIT_MASK( keys, KEY_MAX );

    if( ioctl( _fd, EVIOCGKEY( sizeof(keys) ), keys ) < 0 )
    {
        perror( "EVIOCGKEY" );
        return false;
    }
    else
    {
        return BIT_TEST( keys, bit );
    }
}


bool OInputDevice::isHeld( Switch bit ) const
{
    BIT_MASK( switches, SW_MAX );

    if( ioctl( _fd, EVIOCGSW( sizeof(switches) ), switches ) < 0 )
    {
        perror( "EVIOCGSW" );
        return false;
    }
    else
    {
        return BIT_TEST( switches, bit );
    }
}


QString OInputDevice::globalKeyMask() const
{
    BIT_MASK( keys, KEY_MAX );

    if( ioctl( _fd, EVIOCGKEY( sizeof(keys) ), keys ) < 0 )
    {
        perror( "EVIOCGKEY" );
        return QString::null;
    }
    else
    {
        QString keymask;
        for ( int i = 0; i < KEY_MAX; ++i )
        {
            if ( BIT_TEST( keys, i ) ) keymask.append( QString().sprintf( "%0d, ", i ) );
        }
        return keymask;

    }
}


QString OInputDevice::globalSwitchMask() const
{
    BIT_MASK( switches, SW_MAX );

    if( ioctl( _fd, EVIOCGSW( sizeof(switches) ), switches ) < 0 )
    {
        perror( "EVIOCGSW" );
        return QString::null;
    }
    else
    {
        QString switchmask;
        for ( int i = 0; i < SW_MAX; ++i )
        {
            if ( BIT_TEST( switches, i ) ) switchmask.append( QString().sprintf( "%0d, ", i ) );
        }
        return switchmask;

    }
}


bool OInputDevice::isValid( const QString& path )
{
    char buf[BUFSIZE] = "<unknown>";
    int fd = ::open( (const char*) path, O_RDONLY );
    if ( fd < 0 ) return false;
    int res = ::ioctl( fd, EVIOCGNAME(sizeof buf), buf );
    ::close( fd );
    return res >= 0;
}

