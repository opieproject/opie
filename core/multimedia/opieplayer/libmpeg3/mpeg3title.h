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
#ifndef MPEG3TITLE_H
#define MPEG3TITLE_H

#include "mpeg3io.h"

typedef struct
{
	long start_byte;
	double start_time;
	double absolute_start_time;
	double absolute_end_time;
	long end_byte;
	double end_time;
	int program;
} mpeg3demux_timecode_t;

typedef struct
{
	struct mpeg3_rec *file;
	mpeg3_fs_t *fs;
	long total_bytes;     /* Total bytes in file.  Critical for seeking and length. */
/* Timecode table */
	mpeg3demux_timecode_t *timecode_table;
	long timecode_table_size;    /* Number of entries */
	long timecode_table_allocation;    /* Number of available slots */
} mpeg3_title_t;

#endif
