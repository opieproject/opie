/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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
#ifndef MEDIA_PLAYER_PLUGIN_INTERFACE_H
#define MEDIA_PLAYER_PLUGIN_INTERFACE_H

#include <qpe/qcom.h>

#ifndef QT_NO_COMPONENT
// {c0093632-b44c-4cf7-a279-d82fe8a8890c}
# ifndef IID_MediaPlayerPlugin
#  define IID_MediaPlayerPlugin QUuid( 0xc0093632, 0xb44c, 0x4cf7, 0xa2, 0x79, 0xd8, 0x2f, 0xe8, 0xa8, 0x89, 0x0c )
# endif
#endif


enum ColorFormat {
    RGB565,
    BGR565,
    RGBA8888,
    BGRA8888
};


class MediaPlayerDecoder {

public:
    virtual ~MediaPlayerDecoder() { };
   
    // About Plugin 
    virtual const char *pluginName() = 0;
    virtual const char *pluginComment() = 0;
    virtual double pluginVersion() = 0;

    virtual bool isFileSupported( const QString& file ) = 0;
    virtual bool open( const QString& file ) = 0;
    virtual bool close() = 0;
    virtual bool isOpen() = 0;
    virtual const QString &fileInfo() = 0;

    // If decoder doesn't support audio then return 0 here
    virtual int audioStreams() = 0;
    virtual int audioChannels( int stream ) = 0;
    virtual int audioFrequency( int stream ) = 0;
    virtual int audioSamples( int stream ) = 0;
    virtual bool audioSetSample( long sample, int stream ) = 0;
    virtual long audioGetSample( int stream ) = 0;
    virtual bool audioReadSamples( short *samples, int channels, long samples, long& samplesRead, int stream ) = 0;

    // If decoder doesn't support video then return 0 here
    virtual int videoStreams() = 0;
    virtual int videoWidth( int stream ) = 0;
    virtual int videoHeight( int stream ) = 0;
    virtual double videoFrameRate( int stream ) = 0; // frames per second (this may change to frames/1000secs)
    virtual int videoFrames( int stream ) = 0;
    virtual bool videoSetFrame( long sample, int stream ) = 0;
    virtual long videoGetFrame( int stream ) = 0;
    virtual bool videoReadFrame( unsigned char **output_rows, int in_x, int in_y, int in_w, int in_h, ColorFormat color_model, int stream ) = 0;
    virtual bool videoReadScaledFrame( unsigned char **output_rows, int in_x, int in_y, int in_w, int in_h, int out_w, int out_h, ColorFormat color_model, int stream ) = 0;
    virtual bool videoReadYUVFrame( char *y_output, char *u_output, char *v_output, int in_x, int in_y, int in_w, int in_h, int stream ) = 0;

    // Profiling
    virtual double getTime() = 0;

    // Ignore if these aren't supported
    virtual bool setSMP( int cpus ) = 0;
    virtual bool setMMX( bool useMMX ) = 0;

    // Capabilities
    virtual bool supportsAudio() = 0;
    virtual bool supportsVideo() = 0;
    virtual bool supportsYUV() = 0;
    virtual bool supportsMMX() = 0;
    virtual bool supportsSMP() = 0;
    virtual bool supportsStereo() = 0;
    virtual bool supportsScaling() = 0;

    // File Properies
    virtual long getPlayTime() { return -1; }
};


class MediaPlayerEncoder;


struct MediaPlayerPluginInterface : public QUnknownInterface
{
    virtual MediaPlayerDecoder *decoder() = 0;
    virtual MediaPlayerEncoder *encoder() = 0;
};


#endif

