/* This file is part of the KDE project
   Copyright (C) 2002 Simon Hausmann <hausmann@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "modplugin.h"

#include "memfile.h"

#include <qfileinfo.h>

ModPlugin::ModPlugin()
{
}

ModPlugin::~ModPlugin()
{
    close();
}

const char *ModPlugin::pluginName()
{
    return "ModPlugin";
}

const char *ModPlugin::pluginComment()
{
    return "LibModPlug based MOD/XM/S3M/IT module player plugin";
}

double ModPlugin::pluginVersion()
{
    return 1.0;
}

bool ModPlugin::isFileSupported( const QString &file )
{
    static const char * const patterns [] =
    {
        "669", "amf", "apun", "dsm", "far", "gdm", "imf", "it",
        "med", "mod", "mtm", "nst", "s3m", "stm", "stx", "ult",
        "uni", "xm"
    };
    static const uchar patternCount = sizeof( patterns ) / sizeof( patterns[ 0 ] );

    QString ext = QFileInfo( file ).extension( false /* complete */ ).lower();
    for ( uchar i = 0; i < patternCount; ++i )
        if ( QString::fromLatin1( patterns[ i ] ) == ext )
            return true;

    return false;
}

bool ModPlugin::open( const QString &_file )
{
    MemFile f( _file );
    if ( !f.open( IO_ReadOnly ) )
        return false;

    CSoundFile::SetWaveConfig( s_frequency, s_bytesPerSample * 8, s_channels );

    CSoundFile::SetWaveConfigEx( false, /* surround */
                                 true, /* no oversampling */
                                 false, /* reverb */
                                 true, /* high quality resampler */
                                 true, /* megabass ;) */
                                 true, /* noise reduction */
                                 false /* some eq stuff I didn't really understand..*/ );

    CSoundFile::SetResamplingMode( SRCMODE_POLYPHASE );

    QByteArray &rawData = f.data();
    if ( !Create( reinterpret_cast<BYTE *>( rawData.data() ), rawData.size() ) )
        return false;

    m_songTime = GetSongTime();
    m_maxPosition = GetMaxPosition();
    return true;
}

bool ModPlugin::close()
{
    return Destroy();
}

bool ModPlugin::isOpen()
{
    return m_nType != MOD_TYPE_NONE;
}

const QString &ModPlugin::fileInfo()
{
    return QString::null; // ###
}

int ModPlugin::audioStreams()
{
    return 1;
}

int ModPlugin::audioChannels( int )
{
    return s_channels;
}

int ModPlugin::audioFrequency( int )
{
    return s_frequency;
}

int ModPlugin::audioSamples( int )
{
    return m_songTime * s_frequency;
}

bool ModPlugin::audioSetSample( long sample, int )
{
    float position = ( sample / s_frequency ) * m_maxPosition / m_songTime;
    SetCurrentPos( (int)position );
    return true;
}

long ModPlugin::audioGetSample( int )
{
    return GetCurrentPos() * s_frequency;
}

bool ModPlugin::audioReadSamples( short *output, int /*channels*/, long samples,
                                        long &samplesRead, int )
{
    unsigned long totalAmountInBytes = samples * s_bytesPerSample * s_channels;
    samplesRead = Read( output, totalAmountInBytes );
    // it can happen that our prediction about the total amount of samples
    // is wrong. The mediaplayer can't handle the situation of returning 0 read bytes
    // very well. So instead let's fill up the remaining bytes with zeroes.
    if ( samplesRead == 0 ) 
    {
        memset( reinterpret_cast<char *>( output ), 0, totalAmountInBytes );
        samplesRead = samples;
    }

    return true;
}

int ModPlugin::videoStreams()
{
    return 0;
}

int ModPlugin::videoWidth( int )
{
    return 0;
}

int ModPlugin::videoHeight( int )
{
    return 0;
}

double ModPlugin::videoFrameRate( int )
{
    return 0;
}

int ModPlugin::videoFrames( int )
{
    return 0;
}

bool ModPlugin::videoSetFrame( long, int )
{
    return 0;
}

long ModPlugin::videoGetFrame( int )
{
    return 0;
}

bool ModPlugin::videoReadFrame( unsigned char **, int, int, int, int, 
                                      ColorFormat, int )
{
    return false;
}

bool ModPlugin::videoReadScaledFrame( unsigned char **, int, int, int, int, 
                                            int, int, ColorFormat, int )
{
    return false;
}

bool ModPlugin::videoReadYUVFrame( char *, char *, char *, int, int, int, 
                                         int, int )
{
    return false;
}

double ModPlugin::getTime()
{
    return 0.0; // ###
}

bool ModPlugin::setSMP( int )
{
    return false;
}

bool ModPlugin::setMMX( bool )
{
    return false;
}

bool ModPlugin::supportsAudio()
{
    return true;
}

bool ModPlugin::supportsVideo()
{
    return false;
}

bool ModPlugin::supportsYUV()
{
    return false;
}

bool ModPlugin::supportsMMX()
{
    return false;
}

bool ModPlugin::supportsSMP()
{
    return false;
}

bool ModPlugin::supportsStereo()
{
    return true;
}

bool ModPlugin::supportsScaling()
{
    return false;
}

/* vim: et sw=4
 */
