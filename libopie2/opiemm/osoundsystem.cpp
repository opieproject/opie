/*
                             This file is part of the Opie Project
                             (C) 2003-2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
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

#include <opie2/osoundsystem.h>
#include <opie2/odebug.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/soundcard.h>
#include <sys/stat.h>

static const char* device_label[] = SOUND_DEVICE_LABELS;
static int max_device_nr = sizeof device_label / sizeof (const char*);

using namespace Opie::Core;
using namespace Opie::MM;
/*======================================================================================
 * OSoundSystem
 *======================================================================================*/

OSoundSystem* OSoundSystem::_instance = 0;

OSoundSystem::OSoundSystem()
{
    odebug << "OSoundSystem::OSoundSystem()" << oendl;
    synchronize();
}

void OSoundSystem::synchronize()
{
    // gather available interfaces by inspecting /dev
    //FIXME: we could use SIOCGIFCONF here, but we aren't interested in virtual (e.g. eth0:0) devices
    //FIXME: Use SIOCGIFCONF anway, because we can disable listing of aliased devices

    _interfaces.clear();
    _interfaces.insert( "soundcard", new OSoundCard( this, "soundcard" ) );
    
    /*

    QString str;
    QFile f( "/dev/sound" );
    bool hasFile = f.open( IO_ReadOnly );
    if ( !hasFile )
    {
        odebug << "OSoundSystem: /dev/sound not existing. No sound devices available" << oendl;
        return;
    }
    QTextStream s( &f );
    s.readLine();
    s.readLine();
    while ( !s.atEnd() )
    {
        s >> str;
        str.truncate( str.find( ':' ) );
        odebug << "OSoundSystem: found interface '" <<  str << "'" << oendl;
        OAudioInterface* iface;
        iface = new OAudioInterface( this, (const char*) str );

        _interfaces.insert( str, iface );
        s.readLine();
    }
*/
}


int OSoundSystem::count() const
{
    return _interfaces.count();
}


OSoundCard* OSoundSystem::card( const QString& iface ) const
{
    return _interfaces[iface];
}


OSoundSystem* OSoundSystem::instance()
{
    if ( !_instance ) _instance = new OSoundSystem();
    return _instance;
}


OSoundSystem::CardIterator OSoundSystem::iterator() const
{
    return OSoundSystem::CardIterator( _interfaces );
}


/*======================================================================================
 * OSoundCard
 *======================================================================================*/

OSoundCard::OSoundCard( QObject* parent, const char* name )
                 :QObject( parent, name ), _audio( 0 ), _mixer( 0 )
{
    odebug << "OSoundCard::OSoundCard()" << oendl;
    init();
}


OSoundCard::~OSoundCard()
{
}


void OSoundCard::init()
{
#ifdef QT_QWS_DEVFS
    _audio = new OAudioInterface( this, "/dev/sound/dsp" );
    _mixer = new OMixerInterface( this, "/dev/sound/mixer" );
#else
    _audio = new OAudioInterface( this, "/dev/dsp" );
    _mixer = new OMixerInterface( this, "/dev/mixer" );
#endif
}


/*======================================================================================
 * OAudioInterface
 *======================================================================================*/

OAudioInterface::OAudioInterface( QObject* parent, const char* name )
                 :QObject( parent, name ), _sfd(0)
{
    odebug << "OAudioInterface::OAudioInterface()" << oendl;
    init();
}


OAudioInterface::~OAudioInterface()
{
}


void OAudioInterface::init()
{


}


/*======================================================================================
 * OMixerInterface
 *======================================================================================*/

OMixerInterface::OMixerInterface( QObject* parent, const char* name )
                 :QObject( parent, name ), _devmask( 0 ), _recmask( 0 ), _stmask( 0 )
{
    odebug << "OMixerInterface::OMixerInterface()" << oendl;
    init();
}


OMixerInterface::~OMixerInterface()
{
}


void OMixerInterface::init()
{
    // open the device
    _fd = ::open( name(), O_RDWR );
    if ( _fd == -1 )
    {
        owarn << "OMixerInterface::init(): Can't open mixer " << name() << oendl;
        return;
    }

    // construct the device capabilities
    if ( ioctl( _fd, SOUND_MIXER_READ_CAPS, &_capmask ) != -1 )
    {
        odebug << "OMixerInterface::init() - capmask = " << _capmask << oendl;
    }
    if ( ioctl( _fd, SOUND_MIXER_READ_DEVMASK, &_devmask ) != -1 )
    {
        odebug << "OMixerInterface::init() - devmask = " << _devmask << oendl;
    }
    if ( ioctl( _fd, SOUND_MIXER_READ_RECMASK, &_recmask ) != -1 )
    {
        odebug << "OMixerInterface::init() - recmask = " << _recmask << oendl;
    }
    if ( ioctl( _fd, SOUND_MIXER_READ_STEREODEVS, &_stmask ) != -1 )
    {
        odebug << "OMixerInterface::init() - stereomask = " << _stmask << oendl;
    }
    for ( int i = 0; i < max_device_nr; ++i )
    {
        if ( _devmask & ( 1 << i ) ) 
        {
            _channels.insert( QString( device_label[i] ).stripWhiteSpace(), i );
            odebug << "OMixerInterface::init() - channel '" << device_label[i] << "'" << oendl;
        }
    }
}    
    

QStringList OMixerInterface::allChannels() const
{
    ChannelIterator it = _channels.begin();
    QStringList channels;
    while ( it != _channels.end() )
    {
        channels += it++.key();
    }
    return channels;
}


QStringList OMixerInterface::recChannels() const
{
    ChannelIterator it = _channels.begin();
    QStringList channels;
    while ( it != _channels.end() )
    {
        if ( _recmask & ( 1 << _channels[it.key()] ) ) channels += it++.key();
    }
    return channels;
}


QStringList OMixerInterface::playChannels() const
{
    return allChannels();
}


bool OMixerInterface::hasMultipleRecording() const
{
    return !( _capmask & SOUND_CAP_EXCL_INPUT );
}


bool OMixerInterface::hasChannel( const QString& channel ) const
{
    return _channels.contains( channel );
}


bool OMixerInterface::isStereo( const QString& channel ) const
{
    return _channels.contains( channel ) && ( _stmask & ( 1 << _channels[channel] ) );
}


bool OMixerInterface::isRecordable( const QString& channel ) const
{
    return _channels.contains( channel ) && ( _recmask & ( 1 << _channels[channel] ) );
}


void OMixerInterface::setVolume( const QString& channel, int left, int right )
{
    int volume = left;
    volume |= ( right == -1 ) ? left << 8 : right << 8;

    if ( _channels.contains( channel ) )
    {
        int result = ioctl( _fd, MIXER_WRITE( _channels[channel] ), &volume );
        if ( result == -1 )
        {
            owarn << "Can't set volume: " << strerror( errno ) << oendl;
        }
        else
        {
            if ( result & 0xff != left )
            {
                owarn << "Device adjusted volume from " << left << " to " << (result & 0xff) << oendl;
            }
        }
    }
}


int OMixerInterface::volume( const QString& channel ) const
{
    int volume;

    if ( _channels.contains( channel ) )
    {
        if ( ioctl( _fd, MIXER_READ( _channels[channel] ), &volume ) == -1 )
        {
            owarn << "Can't get volume: " << strerror( errno ) << oendl;
        }
        else return volume;
    }
    return -1;
}
