/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
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
#ifndef LIBMAD_PLUGIN_H 
#define LIBMAD_PLUGIN_H

#include <qstring.h>
//#include <qpe/mediaplayerplugininterface.h>
#include "../mediaplayerplugininterface.h"


// #define OLD_MEDIAPLAYER_API


class LibMadPluginData;


class LibMadPlugin : public MediaPlayerDecoder {

public:
    LibMadPlugin();
    ~LibMadPlugin();

    const char *pluginName() { return "LibMadPlugin"; }
    const char *pluginComment() { return "This is the libmad library that has been wrapped as a plugin"; }
    double pluginVersion() { return 1.0; }

    bool isFileSupported( const QString& );
    bool open( const QString& );
    bool close();
    bool isOpen();
    const QString &fileInfo() { return info; }

    // If decoder doesn't support audio then return 0 here
    int audioStreams();
    int audioChannels( int stream ); 
    int audioFrequency( int stream );
    int audioSamples( int stream );
    bool audioSetSample( long sample, int stream );
    int audioBitsPerSample(int) {return 0;}
    long audioGetSample( int stream );
#ifdef OLD_MEDIAPLAYER_API
    bool audioReadMonoSamples( short *output, long samples, long& samplesRead, int stream );
    bool audioReadStereoSamples( short *output, long samples, long& samplesRead, int stream );
    bool audioReadSamples( short *output, int channel, long samples, int stream );
    bool audioReReadSamples( short *output, int channel, long samples, int stream );
#else
    bool audioReadSamples( short *output, int channels, long samples, long& samplesRead, int stream );
#endif


    bool read();
    bool decode( short *output, long samples, long& samplesRead );
    void printID3Tags();


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
    LibMadPluginData *d;
    QString info;

};


#endif
