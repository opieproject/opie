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
#ifndef MPEG3AUDIO_H
#define MPEG3AUDIO_H

#include "ac3.h"
#include "../bitstream.h"
typedef struct mpeg3_atrack_rec mpeg3_atrack_t;

#define MAXFRAMESIZE 1792
#define HDRCMPMASK 0xfffffd00
#define SBLIMIT 32
#define SSLIMIT 18
#define SCALE_BLOCK 12
#define MPEG3AUDIO_PADDING 1024

/* Values for mode */
#define MPG_MD_STEREO			0
#define MPG_MD_JOINT_STEREO 	1
#define MPG_MD_DUAL_CHANNEL 	2
#define MPG_MD_MONO 			3

/* IMDCT variables */
typedef struct
{
	mpeg3_real_t real;
	mpeg3_real_t imag;
} mpeg3_complex_t;

#define AC3_N 512

struct al_table 
{
	short bits;
	short d;
};

typedef struct
{
	struct mpeg3_rec* file;
	mpeg3_atrack_t* track;
	mpeg3_bits_t *astream;

/* In order of importance */
	int format;               /* format of audio */
	int layer;                /* layer if mpeg */
	int channels;
	long outscale;
	long framenum;
	long prev_framesize;
	long framesize;           /* For mp3 current framesize without header.  For AC3 current framesize with header. */
	int avg_framesize;      /* Includes the 4 byte header */
	mpeg3_real_t *pcm_sample;        /* Interlaced output from synthesizer in floats */
	int pcm_point;            /* Float offset in pcm_sample to write to */
	long pcm_position;        /* Sample start of pcm_samples in file */
	long pcm_size;            /* Number of pcm samples in the buffer */
	long pcm_allocated;       /* Allocated number of samples in pcm_samples */
	int sample_seek;
	double percentage_seek;
	unsigned long oldhead;
	unsigned long newhead;
	unsigned long firsthead;
	int bsnum;
	int lsf;
	int mpeg35;
	int sampling_frequency_code;
	int bitrate_index;
	int bitrate;
	int samples_per_frame;
	int padding;
	int extension;
	int mode;
	int mode_ext;
	int copyright;
	int original;
	int emphasis;
	int error_protection;

/* Back step buffers for mp3 */
	unsigned char bsspace[2][MAXFRAMESIZE + 512]; /* MAXFRAMESIZE */
	unsigned char *bsbuf, *bsbufold;
	long ssize;
	int init;
	int single;
    struct al_table *alloc;
    int II_sblimit;
    int jsbound;
	int bo;                      /* Static variable in synthesizer */

/* MP3 Static arrays here */
	mpeg3_real_t synth_stereo_buffs[2][2][0x110];
	mpeg3_real_t synth_mono_buff[64];
	unsigned int layer2_scfsi_buf[64];

	mpeg3_real_t mp3_block[2][2][SBLIMIT * SSLIMIT];
	int mp3_blc[2];

/* AC3 specific stuff.  AC3 also shares objects with MPEG */
	unsigned int ac3_framesize_code;
	mpeg3_ac3bsi_t ac3_bsi;
	mpeg3_ac3audblk_t ac3_audblk;
	mpeg3_ac3_bitallocation_t ac3_bit_allocation;
	mpeg3_ac3_mantissa_t ac3_mantissa;
	mpeg3_complex_t ac3_imdct_buf[AC3_N / 4];

/* Delay buffer for DCT interleaving */
	mpeg3_real_t ac3_delay[6][AC3_N / 2];
/* Twiddle factor LUT */
	mpeg3_complex_t *ac3_w[7];
#if !defined(USE_FIXED_POINT) || defined(PRINT_FIXED_POINT_TABLES)
	/* Just for allocated memory */
	mpeg3_complex_t ac3_w_1[1];
	mpeg3_complex_t ac3_w_2[2];
	mpeg3_complex_t ac3_w_4[4];
	mpeg3_complex_t ac3_w_8[8];
	mpeg3_complex_t ac3_w_16[16];
	mpeg3_complex_t ac3_w_32[32];
	mpeg3_complex_t ac3_w_64[64];
#endif
	int ac3_lfsr_state;
	unsigned char ac3_buffer[MAX_AC3_FRAMESIZE];
	mpeg3ac3_stream_samples_t ac3_samples;
} mpeg3audio_t;



#endif
