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
#ifndef MOD_PLUGIN_H
#define MOD_PLUGIN_H

//#include "../mediaplayerplugininterface.h"
#include <qpe/mediaplayerplugininterface.h>

#include "stdafx.h"
#include "sndfile.h"

class ModPlugin : public MediaPlayerDecoder,
                  public CSoundFile
{
public:
    ModPlugin();
    virtual ~ModPlugin();

    virtual const char *pluginName();
    virtual const char *pluginComment();
    virtual double pluginVersion();

    virtual bool isFileSupported( const QString &file );
    virtual bool open( const QString &file );
    virtual bool close();
    virtual bool isOpen();
    virtual const QString &fileInfo();

    virtual int audioStreams();
    virtual int audioChannels( int stream );
    virtual int audioFrequency( int stream );
    virtual int audioSamples( int stream );
    virtual bool audioSetSample( long sample, int stream );
    virtual long audioGetSample( int stream );
    virtual bool audioReadSamples( short *output, int channels, long samples,
                                   long &samplesRead, int stream );

    virtual int videoStreams();
    virtual int videoWidth( int stream );
    virtual int videoHeight( int stream );
    virtual double videoFrameRate( int stream );
    virtual int videoFrames( int stream );
    virtual bool videoSetFrame( long sample, int stream );
    virtual long videoGetFrame( int stream );
    virtual bool videoReadFrame( unsigned char **outputRows, int inX, int inY,
                                 int inW, int inH, ColorFormat colorModel, int stream );
    virtual bool videoReadScaledFrame( unsigned char **outputRows, int inX, int inY,
                                       int inW, int inH, int outW, int outH,
                                       ColorFormat colorModel, int stream );
    virtual bool videoReadYUVFrame( char *yOutput, char *uOutput, char *vOutput,
                                    int inX, int inY, int inW, int inH, int stream );

    virtual double getTime();

    virtual bool setSMP( int cpus );
    virtual bool setMMX( bool useMMX );

    virtual bool supportsAudio();
    virtual bool supportsVideo();
    virtual bool supportsYUV();
    virtual bool supportsMMX();
    virtual bool supportsSMP();
    virtual bool supportsStereo();
    virtual bool supportsScaling();

private:
    int m_songTime;
    int m_maxPosition;

    static const int s_channels = 2;
    static const int s_bytesPerSample = 2;
    static const int s_maxChannels = 128;
    static const int s_frequency = 44100;
    static const int s_audioBufferSize = 32768;
};

#endif
/* vim: et sw=4
 */
