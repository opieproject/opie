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
#ifndef MPEG3ATRACK_H
#define MPEG3ATRACK_H

#include "mpeg3demux.h"
#include "audio/mpeg3audio.h"

struct mpeg3_atrack_rec
{
	int channels;
	int sample_rate;
	mpeg3_demuxer_t *demuxer;
	mpeg3audio_t *audio;
	long current_position;
	long total_samples;
};

#endif
