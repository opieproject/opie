/**********************************************************************
** Copyright (C) 2003 Michael 'Mickey' Lauer.  All rights reserved.
** Based on work from Andrew Tridgell and the jpegtoavi project
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef AVI_H
#define AVI_H

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned u32;

// header flags

const u32 AVIF_HASINDEX=0x00000010;    /* index at end of file */
const u32 AVIF_MUSTUSEINDEX=0x00000020;
const u32 AVIF_ISINTERLEAVED=0x00000100;
const u32 AVIF_TRUSTCKTYPE=0x00000800;
const u32 AVIF_WASCAPTUREFILE=0x00010000;
const u32 AVIF_COPYRIGHTED=0x00020000;

// function prototypes

void avi_start(int fd, int frame);
void avi_add(int fd, u8 *buf, int size);
void avi_end(int fd, int width, int height, int fps);
void fprint_quartet(int fd, unsigned int i);

// the following structures are ordered as they appear in a typical AVI

struct riff_head {
    char riff[4];               // chunk type = "RIFF"
    u32 size;                   // chunk size
    char avistr[4];             // avi magic = "AVI "
};

// the avih chunk contains a number of list chunks

struct avi_head {
    char avih[4];               // chunk type = "avih"
    u32 size;                   // chunk size
    u32 time;                   // microsec per frame == 1e6 / fps
    u32 maxbytespersec;         // = 1e6*(total size/frames)/per_usec)
    u32 pad;                    // pad = 0
    u32 flags;                  // e.g. AVIF_HASINDEX
    u32 nframes;                // total number of frames
    u32 initialframes;          // = 0
    u32 numstreams;             // = 1 for now (later = 2 because of audio)
    u32 suggested_bufsize;      // = 0 (no suggestion)
    u32 width;                  // width
    u32 height;                 // height
    u32 reserved[4];            // reserved for future use = 0
};


// the LIST chunk contains a number (==#numstreams) of stream chunks

struct list_head {
    char list[4];               // chunk type = "LIST"
    u32 size;
    char type[4];
};


struct dmlh_head {
    char dmlh[4];               // chunk type dmlh
    u32 size;                   // 4
    u32 nframes;                // number of frames
};


struct stream_head {
    char strh[4];               // chunk type = "strh"
    u32 size;                   // chunk size
    char vids[4];               // stream type = "vids"
    char codec[4];              // codec name (for us, = "MJPG")
    u32 flags;                  // contains AVIT_F* flags
    u16 priority;               // = 0
    u16 language;               // = 0
    u32 initialframes;          // = 0
    u32 scale;                  // = usec per frame
    u32 rate;                   // 1e6
    u32 start;                  // = 0
    u32 length;                 // number of frames
    u32 suggested_bufsize;      // = 0
    u32 quality;                // = 0 ?
    u32 samplesize;             // = 0 ?
};


struct db_head {
    char db[4];                 // "00db"
    u32 size;
};

// a frame chunk contains one JPEG image

struct frame_head {
    char strf[4];               // chunk type = "strf"
    u32 size;                   // sizeof chunk (big endian)    ?
    u32 size2;                  // sizeof chunk (little endian) ?
    u32 width;
    u32 height;
    u16 planes;                 // 1 bitplane
    u16 bitcount;               // 24 bpl
    char codec[4];              // MJPG (for us)
    u32 unpackedsize;           // = 3*w*h
    u32 r1;                     // reserved
    u32 r2;                     // reserved
    u32 clr_used;               // reserved
    u32 clr_important;          // reserved
};

struct idx1_head {
    char idx1[4];               // chunk type = "idx1"
    u32 size;                   // chunk size
};

#ifdef __cplusplus
}
#endif

#endif
