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
#ifndef LIBFLASH_PLUGIN_H 
#define LIBFLASH_PLUGIN_H


#include <qstring.h>
#include <qapplication.h>
#include "flash.h"
//#include <qpe/mediaplayerplugininterface.h>
#include "../mediaplayerplugininterface.h"


class LibFlashPlugin : public MediaPlayerDecoder {

public:
    LibFlashPlugin(); 
    ~LibFlashPlugin() { close(); }
 
    const char *pluginName() { return "LibFlashPlugin: " PLUGIN_NAME " " FLASH_VERSION_STRING; }
    const char *pluginComment() { return "This is the libflash library: " PLUGIN_NAME " " FLASH_VERSION_STRING; }
    double pluginVersion() { return 1.0; }

    bool isFileSupported( const QString& fileName ) { return fileName.right(4) == ".swf"; } 
    bool open( const QString& fileName );
    bool close() { FlashClose( file ); file = NULL; return TRUE; }
    bool isOpen() { return file != NULL; }
    const QString &fileInfo() { return strInfo = qApp->translate( "MediaPlayer", "No Information Available", "media plugin text" ); }

    // If decoder doesn't support audio then return 0 here
    int audioStreams() { return 1; }
    int audioChannels( int /*stream*/ ) { return 2; }
    int audioFrequency( int /*stream*/ ) { return 44100; }
    int audioSamples( int /*stream*/ ) { return 1000000; }
int audioBitsPerSample(int) { return 0;}
    bool audioSetSample( long sample, int stream );
    long audioGetSample( int stream );
    //bool audioReadMonoSamples( short *output, long samples, long& samplesRead, int stream );
    //bool audioReadStereoSamples( short *output, long samples, long& samplesRead, int stream );
    bool audioReadSamples( short *output, int channels, long samples, long& samplesRead, int stream );
    //bool audioReadSamples( short *output, int channel, long samples, int stream );
    //bool audioReReadSamples( short *output, int channel, long samples, int stream );

    // If decoder doesn't support video then return 0 here
    int videoStreams();
    int videoWidth( int stream );
    int videoHeight( int stream );
    double videoFrameRate( int stream );
    int videoFrames( int stream );
    bool videoSetFrame( long frame, int stream );
    long videoGetFrame( int stream );
    bool videoReadFrame( unsigned char **output_rows, int in_x, int in_y, int in_w, int in_h, ColorFormat color_model, int stream );
    bool videoReadScaledFrame( unsigned char **output_rows, int in_x, int in_y, int in_w, int in_h, int out_w, int out_h, ColorFormat color_model, int stream );
    bool videoReadYUVFrame( char *y_output, char *u_output, char *v_output, int in_x, int in_y, int in_w, int in_h, int stream );

    // Profiling
    double getTime();

    // Ignore if these aren't supported
    bool setSMP( int cpus );
    bool setMMX( bool useMMX );

    // Capabilities
    bool supportsAudio() { return TRUE; }
    bool supportsVideo() { return TRUE; }
    bool supportsYUV() { return TRUE; }
    bool supportsMMX() { return TRUE; }
    bool supportsSMP() { return TRUE; }
    bool supportsStereo() { return TRUE; }
    bool supportsScaling() { return TRUE; }

private:
    FlashHandle file;
    FlashDisplay *fd;
    QString strInfo;

};


#endif

