/*
                             This file is part of the Opie Project

                             (C) 2003 Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
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

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/soundcard.h>
#include <sys/stat.h>

#include <qstringlist.h>

/*======================================================================================
 * OSoundSystem
 *======================================================================================*/

OSoundSystem* OSoundSystem::_instance = 0;

OSoundSystem::OSoundSystem()
{
    qDebug( "OSoundSystem::OSoundSystem()" );
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
        qDebug( "OSoundSystem: /dev/sound not existing. No sound devices available" );
        return;
    }
    QTextStream s( &f );
    s.readLine();
    s.readLine();
    while ( !s.atEnd() )
    {
        s >> str;
        str.truncate( str.find( ':' ) );
        qDebug( "OSoundSystem: found interface '%s'", (const char*) str );
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
    qDebug( "OSoundCard::OSoundCard()" );
    init();
}


OSoundCard::~OSoundCard()
{
}


void OSoundCard::init()
{
    _audio = new OAudioInterface( this, "/dev/dsp" );
    _mixer = new OMixerInterface( this, "/dev/mixer" );
}


/*======================================================================================
 * OAudioInterface
 *======================================================================================*/

OAudioInterface::OAudioInterface( QObject* parent, const char* name )
                 :QObject( parent, name )
{
    qDebug( "OAudioInterface::OAudioInterface()" );
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
                 :QObject( parent, name )
{
    qDebug( "OMixerInterface::OMixerInterface()" );
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
        qWarning( "can't open mixer." );
        return;
    }

    // construct the device capabilities
    int devmask = 0;
    if ( ioctl( _fd, SOUND_MIXER_READ_DEVMASK, &devmask ) != -1 )
    {
        if ( devmask & ( 1 << SOUND_MIXER_VOLUME ) )    _channels.insert( "PlayVolume", SOUND_MIXER_VOLUME );
        if ( devmask & ( 1 << SOUND_MIXER_BASS ) )      _channels.insert( "PlayBass", SOUND_MIXER_BASS );
        if ( devmask & ( 1 << SOUND_MIXER_TREBLE ) )    _channels.insert( "PlayTreble", SOUND_MIXER_TREBLE );
        if ( devmask & ( 1 << SOUND_MIXER_SYNTH ) )     _channels.insert( "PlaySynth", SOUND_MIXER_SYNTH );
        if ( devmask & ( 1 << SOUND_MIXER_PCM ) )       _channels.insert( "PlayPCM", SOUND_MIXER_PCM );
        if ( devmask & ( 1 << SOUND_MIXER_SPEAKER ) )   _channels.insert( "PlaySpeaker", SOUND_MIXER_SPEAKER );
        if ( devmask & ( 1 << SOUND_MIXER_LINE ) )      _channels.insert( "PlayLine", SOUND_MIXER_LINE );
        if ( devmask & ( 1 << SOUND_MIXER_MIC ) )       _channels.insert( "PlayMic", SOUND_MIXER_MIC );
        if ( devmask & ( 1 << SOUND_MIXER_CD ) )        _channels.insert( "PlayCD", SOUND_MIXER_CD );
        if ( devmask & ( 1 << SOUND_MIXER_IMIX ) )      _channels.insert( "PlayInputMix", SOUND_MIXER_IMIX );
        if ( devmask & ( 1 << SOUND_MIXER_ALTPCM ) )    _channels.insert( "PlayAltPCM", SOUND_MIXER_ALTPCM );
        if ( devmask & ( 1 << SOUND_MIXER_RECLEV ) )    _channels.insert( "PlayRecord", SOUND_MIXER_RECLEV );
        if ( devmask & ( 1 << SOUND_MIXER_IGAIN ) )     _channels.insert( "PlayInputGain", SOUND_MIXER_IGAIN );
        if ( devmask & ( 1 << SOUND_MIXER_OGAIN ) )     _channels.insert( "PlayOutputGain", SOUND_MIXER_OGAIN );
        //qDebug( "devmask available and constructed." );
    }

    devmask = 0;
    if ( ioctl( _fd, SOUND_MIXER_READ_RECMASK, &devmask ) != -1 )
    {
        if ( devmask & ( 1 << SOUND_MIXER_VOLUME ) )    _channels.insert( "RecVolume", SOUND_MIXER_VOLUME );
        if ( devmask & ( 1 << SOUND_MIXER_BASS ) )      _channels.insert( "RecBass", SOUND_MIXER_BASS );
        if ( devmask & ( 1 << SOUND_MIXER_TREBLE ) )    _channels.insert( "RecTreble", SOUND_MIXER_TREBLE );
        if ( devmask & ( 1 << SOUND_MIXER_SYNTH ) )     _channels.insert( "RecSynth", SOUND_MIXER_SYNTH );
        if ( devmask & ( 1 << SOUND_MIXER_PCM ) )       _channels.insert( "RecPCM", SOUND_MIXER_PCM );
        if ( devmask & ( 1 << SOUND_MIXER_SPEAKER ) )   _channels.insert( "RecSpeaker", SOUND_MIXER_SPEAKER );
        if ( devmask & ( 1 << SOUND_MIXER_LINE ) )      _channels.insert( "RecLine", SOUND_MIXER_LINE );
        if ( devmask & ( 1 << SOUND_MIXER_MIC ) )       _channels.insert( "RecMic", SOUND_MIXER_MIC );
        if ( devmask & ( 1 << SOUND_MIXER_CD ) )        _channels.insert( "RecCD", SOUND_MIXER_CD );
        if ( devmask & ( 1 << SOUND_MIXER_IMIX ) )      _channels.insert( "RecInputMix", SOUND_MIXER_IMIX );
        if ( devmask & ( 1 << SOUND_MIXER_ALTPCM ) )    _channels.insert( "RecAltPCM", SOUND_MIXER_ALTPCM );
        if ( devmask & ( 1 << SOUND_MIXER_RECLEV ) )    _channels.insert( "RecRecord", SOUND_MIXER_RECLEV );
        if ( devmask & ( 1 << SOUND_MIXER_IGAIN ) )     _channels.insert( "RecInputGain", SOUND_MIXER_IGAIN );
        if ( devmask & ( 1 << SOUND_MIXER_OGAIN ) )     _channels.insert( "RecOutputGain", SOUND_MIXER_OGAIN );
        //qDebug( "recmask available and constructed." );
    }

/*    ChannelIterator it;
    for ( it = _channels.begin(); it != _channels.end(); ++it )
    {
        qDebug( "Channel %s available (bit %d)", (const char*) it.key(), it.data() );
        qDebug( " +--- Volume: %02d | %02d", volume( it.key() ) & 0xff, volume( it.key() ) >> 8 );
    }
*/
}

QStringList OMixerInterface::allChannels() const
{
    ChannelIterator it = _channels.begin();
    QStringList channels;
    while ( it != _channels.end() )
    {
        channels += it.key();
        it++;
    }
    return channels;
}


QStringList OMixerInterface::recChannels() const
{
    qWarning( "NYI" );
}


QStringList OMixerInterface::playChannels() const
{
    qWarning( "NYI" );
}


bool OMixerInterface::hasChannel( const QString& channel )
{
    return _channels.contains( channel );
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
            qWarning( "Can't set volume: %s", strerror( errno ) );
        }
        else
        {
            if ( result & 0xff != left )
            {
                qWarning( "Device adjusted volume from %d to %d", left, result & 0xff );
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
            qWarning( "Can't get volume: %s", strerror( errno ) );
        }
        else return volume;
    }
    return -1;
}


