/****************************************************************************
* libtremorplugin.h
*
* Copyright (C) 2002 Latchesar Ionkov <lucho@ionkov.net>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/

#ifndef LIBTREMOR_PLUGIN_H 
#define LIBTREMOR_PLUGIN_H


#include <qstring.h>
#include <qpe/mediaplayerplugininterface.h>


class LibTremorPluginData;


class LibTremorPlugin : public MediaPlayerDecoder {

public:
    LibTremorPlugin();
    ~LibTremorPlugin();

    const char *pluginName() { return "LibTremorPlugin"; }
    const char *pluginComment() { return "This is the Tremor library that has been wrapped as a plugin"; }
    double pluginVersion() { return 1.0; }

    bool isFileSupported( const QString& );
    bool open( const QString& );
    bool close();
    bool isOpen();
    const QString &fileInfo();

    // If decoder doesn't support audio then return 0 here
    int audioStreams();
    int audioChannels( int stream ); 
    int audioFrequency( int stream );
    int audioSamples( int stream );
    bool audioSetSample( long sample, int stream );
    long audioGetSample( int stream );
    bool audioReadSamples( short *output, int channels, long samples, long& samplesRead, int stream );


    // If decoder doesn't support video then return 0 here
    int videoStreams() { return 0; }
    int videoWidth( int ) { return 0; }
    int videoHeight( int ) { return 0; }
    double videoFrameRate( int ) { return 0.0; }
    int videoFrames( int ) { return 0; }
    bool videoSetFrame( long, int ) { return FALSE; }
    long videoGetFrame( int ) { return 0; }
    bool videoReadFrame( unsigned char **, int, int, int, int, ColorFormat, int ) { return FALSE; }
    bool videoReadScaledFrame( unsigned char **, int, int, int, int, int, int, ColorFormat, int ) { return FALSE; }
    bool videoReadYUVFrame( char *, char *, char *, int, int, int, int, int ) { return FALSE; }

    // Profiling
    double getTime();

    // Ignore if these aren't supported
    bool setSMP( int ) { return FALSE; }
    bool setMMX( bool ) { return FALSE; }

    // Capabilities
    bool supportsAudio() { return TRUE; }
    bool supportsVideo() { return FALSE; }
    bool supportsYUV() { return FALSE; }
    bool supportsMMX() { return TRUE; }
    bool supportsSMP() { return FALSE; }
    bool supportsStereo() { return TRUE; }
    bool supportsScaling() { return FALSE; }

    long getPlayTime() { return -1; }

private:
    LibTremorPluginData *d;

};


#endif
