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
#ifndef MPEG3PRIVATE_H
#define MPEG3PRIVATE_H

#include "mpeg3atrack.h"
#include "mpeg3css.h"
#include "mpeg3io.h"
#include "mpeg3private.inc"
#include "mpeg3title.h"
#include "mpeg3vtrack.h"

struct mpeg3_rec
{
	mpeg3_fs_t *fs;      /* Store entry path here */
	mpeg3_demuxer_t *demuxer;        /* Master tables */

/* Media specific */
	int has_audio;
	int has_video;
	int total_astreams;
	int total_vstreams;
	mpeg3_atrack_t *atrack[MPEG3_MAX_STREAMS];
	mpeg3_vtrack_t *vtrack[MPEG3_MAX_STREAMS];

/* Only one of these is set to 1 to specify what kind of stream we have. */
	int is_transport_stream;
	int is_program_stream;
	int is_audio_stream;         /* Elemental stream */
	int is_video_stream;         /* Elemental stream */
	long packet_size;
/* Type and stream for getting current percentage */
	int last_type_read;  /* 1 - audio   2 - video */
	int last_stream_read;

	int program;  /* Number of program to play */
	int cpus;
	int have_mmx;
};

typedef struct mpeg3_rec mpeg3_t;



#endif
