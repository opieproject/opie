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
// L.J.Potter added changes Fri 02-15-2002

#ifndef WAV_PLUGIN_H 
#define WAV_PLUGIN_H


#include <qstring.h>
#include <qapplication.h>
#include "../mediaplayerplugininterface.h"


// #define OLD_MEDIAPLAYER_API


class WavPluginData;


class WavPlugin : public MediaPlayerDecoder {

public:
    WavPlugin();
    ~WavPlugin();

    const char *pluginName() { return "WavPlugin"; }
    const char *pluginComment() { return "This is a simple plugin for playing wav files"; }
    double pluginVersion() { return 1.0; }

    bool isFileSupported( const QString& );
    bool open( const QString& );
    bool close();
    bool isOpen();
    const QString &fileInfo() { return strInfo = ""; }

    // If decoder doesn't support audio then return 0 here
    int audioStreams();
    int audioChannels( int stream ); 
    int audioFrequency( int stream );
    int audioBitsPerSample( int stream );
    int audioSamples( int stream );
    bool audioSetSample( long sample, int stream );
    long audioGetSample( int stream );
#ifdef OLD_MEDIAPLAYER_API
    bool audioReadMonoSamples( short *output, long samples, long& samplesRead, int stream );
    bool audioReadStereoSamples( short *output, long samples, long& samplesRead, int stream );
    bool audioReadSamples( short *output, int channel, long samples, int stream );
    bool audioReReadSamples( short *output, int channel, long samples, int stream );
#else
    bool audioReadSamples( short *output, int channels, long samples, long& samplesRead, int stream );
#endif

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
    WavPluginData *d;
    QString strInfo;

};


#endif
