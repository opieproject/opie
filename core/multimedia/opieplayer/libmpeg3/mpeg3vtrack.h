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
#ifndef MPEG3_VTRACK_H
#define MPEG3_VTRACK_H

#include "mpeg3demux.h"
#include "video/mpeg3video.h"

struct mpeg3_vtrack_rec
{
	int width;
	int height;
	float frame_rate;
	mpeg3_demuxer_t *demuxer;
	mpeg3video_t *video;
	long current_position;  /* Number of next frame to be played */
	long total_frames;     /* Total frames in the file */
};

typedef struct mpeg3_vtrack_rec mpeg3_vtrack_t;

#endif
