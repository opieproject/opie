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
#include "libflashplugin.h"

#if 0

bool LibFlashPlugin::audioReadSamples( short *output, int channel, long samples, int stream ) {
}


bool LibFlashPlugin::audioReReadSamples( short *output, int channel, long samples, int stream ) {
}


bool LibFlashPlugin::audioReadMonoSamples( short *output, long samples, long& samplesRead, int stream ) {
    samplesRead = samples;
}


bool LibFlashPlugin::audioReadStereoSamples( short *output, long samples, long& samplesRead, int stream ) {
}


bool LibFlashPlugin::videoReadFrame( unsigned char **output_rows, int in_x, int in_y, int in_w, int in_h, ColorFormat color_model, int stream ) {
}


bool LibFlashPlugin::videoReadScaledFrame( unsigned char **output_rows, int in_x, int in_y, int in_w, int in_h, int out_w, int out_h, ColorFormat color_model, int stream ) {
/*
    int format = MPEG3_RGB565;
    switch ( color_model ) {
	case RGB565:	format = MPEG3_RGB565;   break;
	case RGBA8888:	format = MPEG3_RGBA8888; break;
	case BGRA8888:	format = MPEG3_BGRA8888; break;
    }
*/
}


bool LibFlashPlugin::videoReadYUVFrame( char *y_output, char *u_output, char *v_output, int in_x, int in_y, int in_w, int in_h, int stream ) {
}


FlashHandle file;
FlashDisplay *fd;

#endif


LibFlashPlugin::LibFlashPlugin() {
    file = NULL;
    fd = 0;
}
#include <stdio.h>
#include <stdlib.h>
static int readFile(const char *filename, char **buffer, long *size)
{
    FILE *in;
    char *buf;
    long length;

    printf("read files\n");

    in = fopen(filename,"r");
    if (in == 0) {
	perror(filename);
	return -1;
    }
    fseek(in,0,SEEK_END);
    length = ftell(in);
    rewind(in);
    buf = (char *)malloc(length);
    fread(buf,length,1,in);
    fclose(in);

    *size = length;
    *buffer = buf;

    return length;
}

static void showUrl(char *url, char * /*target*/, void * /*client_data*/) {
    printf("get url\n");
    printf("GetURL : %s\n", url);
}

static void getSwf(char *url, int level, void *client_data) {
    FlashHandle flashHandle = (FlashHandle) client_data;
    char *buffer;
    long size;

    printf("get swf\n");

    printf("LoadMovie: %s @ %d\n", url, level);
    if (readFile(url, &buffer, &size) > 0) {
	FlashParse(flashHandle, level, buffer, size);
    }
}

bool LibFlashPlugin::open( const QString& fileName ) {

    printf("opening file\n");

    delete fd;
    fd = new FlashDisplay;
    fd->pixels = new int[320*240*4];
    fd->width = 200;
    fd->bpl = 320*2;
    fd->height = 300;
    fd->depth = 16;
    fd->bpp = 2;
    fd->flash_refresh = 25;
    fd->clip_x = 0;
    fd->clip_y = 0;
    fd->clip_width = 0;
    fd->clip_height = 0;

    char *buffer;
    long size;
    int status;
    struct FlashInfo fi;

    if (readFile(fileName.latin1(), &buffer, &size) < 0)
	    exit(2);

    if (!(file = FlashNew()))
	    exit(1);

    do
	status = FlashParse(file, 0, buffer, size);
    while (status & FLASH_PARSE_NEED_DATA);

    free(buffer);
    FlashGetInfo(file, &fi);
    //FlashSettings(flashHandle, PLAYER_LOOP);
    FlashGraphicInit(file, fd);
    FlashSoundInit(file, "/dev/dsp");
    FlashSetGetUrlMethod(file, showUrl, 0);
    FlashSetGetSwfMethod(file, getSwf, (void*)file);

    printf("opened file\n");
}

// If decoder doesn't support audio then return 0 here
bool LibFlashPlugin::audioSetSample( long sample, int stream ) { return TRUE; }
long LibFlashPlugin::audioGetSample( int stream ) { return 0; }
//bool LibFlashPlugin::audioReadMonoSamples( short *output, long samples, long& samplesRead, int stream ) { return TRUE; }
//bool LibFlashPlugin::audioReadStereoSamples( short *output, long samples, long& samplesRead, int stream ) { return FALSE; }
bool LibFlashPlugin::audioReadSamples( short *output, int channels, long samples, long& samplesRead, int stream ) { return FALSE; }
//bool LibFlashPlugin::audioReadSamples( short *output, int channel, long samples, int stream ) { return TRUE; }
//bool LibFlashPlugin::audioReReadSamples( short *output, int channel, long samples, int stream ) { return TRUE; }

// If decoder doesn't support video then return 0 here
int LibFlashPlugin::videoStreams() { return 1; }
int LibFlashPlugin::videoWidth( int stream ) { return 300; }
int LibFlashPlugin::videoHeight( int stream ) { return 200; }
double LibFlashPlugin::videoFrameRate( int stream ) { return 25.0; }
int LibFlashPlugin::videoFrames( int stream ) { return 1000000; }
bool LibFlashPlugin::videoSetFrame( long frame, int stream ) { return TRUE; }
long LibFlashPlugin::videoGetFrame( int stream ) { return 0; }
bool LibFlashPlugin::videoReadFrame( unsigned char **output_rows, int in_x, int in_y, int in_w, int in_h, ColorFormat color_model, int stream ) { return TRUE; }
#include <time.h>
bool LibFlashPlugin::videoReadScaledFrame( unsigned char **output_rows, int in_x, int in_y, int in_w, int in_h, int out_w, int out_h, ColorFormat color_model, int stream ) {
        struct timeval wd;
        FlashEvent fe;

/*
    delete fd;
    fd = new FlashDisplay;
    fd->pixels = output_rows[0];
    fd->width = 300; // out_w;
    fd->bpl = 640; // out_w*2;
    fd->height = 200;//out_h;
    fd->depth = 16;
    fd->bpp = 2;
    fd->flash_refresh = 50;
    fd->clip_x = 0;//in_x;
    fd->clip_y = 0;//in_y;
    fd->clip_width = 300;//in_w;
    fd->clip_height = 200;//in_h;
    FlashGraphicInit(file, fd);
*/

        long cmd = FLASH_WAKEUP;
        FlashExec(file, cmd, 0, &wd);

	fe.type = FeRefresh;
	cmd = FLASH_EVENT;
        FlashExec(file, cmd, &fe, &wd);
/*
	for (int i = 0; i < out_h; i++)
	    memcpy( output_rows[i], (char*)fd->pixels + i*fd->bpl, QMIN( fd->width * fd->bpp, out_w * fd->bpp ) );
*/	
	memcpy( output_rows[0], (char*)fd->pixels, out_w * out_h * 2 );
}

bool LibFlashPlugin::videoReadYUVFrame( char *y_output, char *u_output, char *v_output, int in_x, int in_y, int in_w, int in_h, int stream ) { return TRUE; }

// Profiling
double LibFlashPlugin::getTime() { return 0.0; }

// Ignore if these aren't supported
bool LibFlashPlugin::setSMP( int cpus ) { return TRUE; }
bool LibFlashPlugin::setMMX( bool useMMX ) { return TRUE; }


