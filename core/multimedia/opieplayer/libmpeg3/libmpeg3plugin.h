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
#ifndef LIBMPEG3_PLUGIN_H 
#define LIBMPEG3_PLUGIN_H

#include <qstring.h>
#include <qapplication.h>
#include "libmpeg3.h"
#include "mpeg3protos.h"
//#include <qpe/mediaplayerplugininterface.h>
#include "../mediaplayerplugininterface.h"


// #define OLD_MEDIAPLAYER_API


class LibMpeg3Plugin : public MediaPlayerDecoder {

public:
    LibMpeg3Plugin() { file = NULL; }
    ~LibMpeg3Plugin() { close(); }

    const char *pluginName() { return "LibMpeg3Plugin"; }
    const char *pluginComment() { return "This is the libmpeg3 library writen by ... which has been modified by trolltech to use fixed point maths"; }
    double pluginVersion() { return 1.0; }

    bool isFileSupported( const QString& fileName ) { return mpeg3_check_sig( (char *)fileName.latin1() ) == 1; }
    bool open( const QString& fileName ) { file = mpeg3_open( (char *)fileName.latin1() ); return file != NULL; }
    bool close() { if ( file ) { int r = mpeg3_close( file ); file = NULL; return r == 1; } return FALSE; }
    bool isOpen() { return file != NULL; }
    const QString &fileInfo() { return strInfo = ""; }

    // If decoder doesn't support audio then return 0 here
    int audioStreams() { return file ? mpeg3_total_astreams( file ) : 0; }
    int audioChannels( int stream ) { return file ? mpeg3_audio_channels( file, stream ) : 0; }
    int audioFrequency( int stream ) { return file ? mpeg3_sample_rate( file, stream ) : 0; }
    int audioBitsPerSample(int) { return 0;}
    int audioSamples( int stream ) { return file ? mpeg3_audio_samples( file, stream ) : 0; } 
    bool audioSetSample( long sample, int stream ) { return file ? mpeg3_set_sample( file, sample, stream) == 1 : FALSE; }
    long audioGetSample( int stream ) { return file ? mpeg3_get_sample( file, stream ) : 0; }
#ifdef OLD_MEDIAPLAYER_API
    bool audioReadMonoSamples( short *output, long samples, long& samplesRead, int stream );
    bool audioReadStereoSamples( short *output, long samples, long& samplesRead, int stream );
    bool audioReadSamples( short *output, int channel, long samples, int stream );
    bool audioReReadSamples( short *output, int channel, long samples, int stream );
#else
    bool audioReadSamples( short *output, int channels, long samples, long& samplesRead, int stream );
#endif

    // If decoder doesn't support video then return 0 here
    int videoStreams() { return file ? mpeg3_total_vstreams( file ) : 0; }
    int videoWidth( int stream ) { return file ? mpeg3_video_width( file, stream ) : 0; }
    int videoHeight( int stream ) { return file ? mpeg3_video_height( file, stream ) : 0; }
    double videoFrameRate( int stream ) { return file ? mpeg3_frame_rate( file, stream ) : 0.0; }
    int videoFrames( int stream )
{ return file ? mpeg3_video_frames( file, stream ) : 0; }
/*
{
    if ( file ) {
	int frames = mpeg3_video_frames( file, stream );
	if ( frames == 1 ) {
	    int res = mpeg3_seek_percentage( file, 0.99 );
	    printf("res: %i\n", res );
	    mpeg3video_seek( file->vtrack[stream]->video );
	    frames = mpeg3_get_frame( file, stream );
	    mpeg3_seek_percentage( file, 0.0 );
	}
	return frames;
    }
    return 0;
}
*/
    bool videoSetFrame( long frame, int stream ) { return file ? mpeg3_set_frame( file, frame, stream) == 1 : FALSE; }
    long videoGetFrame( int stream ) { return file ? mpeg3_get_frame( file, stream ) : 0; }
    bool videoReadFrame( unsigned char **output_rows, int in_x, int in_y, int in_w, int in_h, ColorFormat color_model, int stream );
    bool videoReadScaledFrame( unsigned char **output_rows, int in_x, int in_y, int in_w, int in_h, int out_w, int out_h, ColorFormat color_model, int stream );
    bool videoReadYUVFrame( char *y_output, char *u_output, char *v_output, int in_x, int in_y, int in_w, int in_h, int stream );

    // Profiling
    double getTime() { return file ? mpeg3_get_time( file ) : 0.0; }

    // Ignore if these aren't supported
    bool setSMP( int cpus ) { return file ? mpeg3_set_cpus( file, cpus ) == 1 : FALSE; }
    bool setMMX( bool useMMX ) { return file ? mpeg3_set_mmx( file, useMMX ) == 1 : FALSE; }

    // Capabilities
    bool supportsAudio() { return TRUE; }
    bool supportsVideo() { return TRUE; }
    bool supportsYUV() { return TRUE; }
    bool supportsMMX() { return TRUE; }
    bool supportsSMP() { return TRUE; }
    bool supportsStereo() { return TRUE; }
    bool supportsScaling() { return TRUE; }

    long getPlayTime() { return -1; }

private:
    mpeg3_t *file;
    QString strInfo;

};


#endif

