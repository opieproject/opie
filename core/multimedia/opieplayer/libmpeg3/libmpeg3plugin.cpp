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
#include "libmpeg3plugin.h"


#ifdef OLD_MEDIAPLAYER_API


bool LibMpeg3Plugin::audioReadSamples( short *output, int channel, long samples, int stream ) {
    return file ? mpeg3_read_audio( file, 0, output, 0, channel, samples, stream ) == 1 : FALSE; 
}


bool LibMpeg3Plugin::audioReReadSamples( short *output, int channel, long samples, int stream ) {
    return file ? mpeg3_reread_audio( file, 0, output, 0, channel, samples, stream ) == 1 : FALSE; 
}


bool LibMpeg3Plugin::audioReadMonoSamples( short *output, long samples, long& samplesRead, int stream ) {
    samplesRead = samples;
    return file ? mpeg3_read_audio( file, 0, output, 0, 0, samples, stream ) == 1 : FALSE; 
}


bool LibMpeg3Plugin::audioReadStereoSamples( short *output, long samples, long& samplesRead, int stream ) {
    bool err = FALSE;
    if ( file ) {
#if 1
	err = mpeg3_read_audio  ( file, 0,  output, 1, 0, samples, stream ) == 1;
	if ( err == FALSE ) {
	    err = mpeg3_reread_audio( file, 0, output + 1, 1, 1, samples, stream ) == 1;
#else
	short left[samples];
	short right[samples];
	err = mpeg3_read_audio  ( file, 0,  left, 0, samples, stream ) == 1;
	if ( !err )
	    err = mpeg3_reread_audio( file, 0, right, 1, samples, stream ) == 1;
	for ( int j = 0; j < samples; j++ ) {
	    output[j*2+0] =  left[j];
	    output[j*2+1] = right[j];
#endif
	}
    }
    samplesRead = samples;
    return err;
}


#else


bool LibMpeg3Plugin::audioReadSamples( short *output, int channels, long samples, long& samplesRead, int stream ) {
    samplesRead = samples;
    switch ( channels ) {
	case 1:
	    return file ? mpeg3_read_audio( file, 0, output, 0, 0, samples, stream ) == 1 : FALSE;
	case 2:
	    if ( ( file ) && ( mpeg3_read_audio( file, 0, output, 1, 0, samples, stream ) != 1 ) &&
		    ( mpeg3_reread_audio( file, 0, output + 1, 1, 1, samples, stream ) != 1 ) )
		return TRUE;
	    return FALSE;
    }
    return FALSE;
}


#endif


bool LibMpeg3Plugin::videoReadFrame( unsigned char **output_rows, int in_x, int in_y, int in_w, int in_h, ColorFormat color_model, int stream ) {
    int format = MPEG3_RGB565;
    switch ( color_model ) {
	case RGB565:	format = MPEG3_RGB565;   break;
	case BGR565:    /*format = MPEG3_BGR565;*/   break;
	case RGBA8888:	format = MPEG3_RGBA8888; break;
	case BGRA8888:	format = MPEG3_BGRA8888; break;
    }
    return file ? mpeg3_read_frame( file, output_rows, in_x, in_y, in_w, in_h, in_w, in_h, format, stream ) == 1 : FALSE;
}


bool LibMpeg3Plugin::videoReadScaledFrame( unsigned char **output_rows, int in_x, int in_y, int in_w, int in_h, int out_w, int out_h, ColorFormat color_model, int stream ) {
    int format = MPEG3_RGB565;
    switch ( color_model ) {
	case RGB565:	format = MPEG3_RGB565;   break;
	case BGR565:	/*format = MPEG3_BGR565;*/   break;
	case RGBA8888:	format = MPEG3_RGBA8888; break;
	case BGRA8888:	format = MPEG3_BGRA8888; break;
    }
    return file ? mpeg3_read_frame( file, output_rows, in_x, in_y, in_w, in_h, out_w, out_h, format, stream ) == 1 : FALSE;
}


bool LibMpeg3Plugin::videoReadYUVFrame( char *y_output, char *u_output, char *v_output, int in_x, int in_y, int in_w, int in_h, int stream ) {
    return file ? mpeg3_read_yuvframe( file, y_output, u_output, v_output, in_x, in_y, in_w, in_h, stream ) == 1 : FALSE;
}


