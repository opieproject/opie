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

#include "avi.h"

#include <string.h>
#include <stdio.h>

int nframes;
int totalsize;
unsigned int* sizes;

void fprint_quartet(int fd, unsigned int i)
{
    char data[4];

    data[0] = (char) i%0x100;
    i /= 0x100;
    data[1] = (char) i%0x100;
    i /= 0x100;
    data[2] = (char) i%0x100;
    i /= 0x100;
    data[3] = (char) i%0x100;

    write( fd, &data, 4 );
}

// start writing an AVI file

void avi_start(int fd, int frames)
{
    int ofs = sizeof(struct riff_head)+
              sizeof(struct list_head)+
              sizeof(struct avi_head)+
              sizeof(struct list_head)+
              sizeof(struct stream_head)+
              sizeof(struct frame_head)+
              sizeof(struct list_head)+
              sizeof(struct dmlh_head)+
              sizeof(struct list_head);

    printf( "avi_start: frames = %d\n", frames );

    lseek(fd, ofs, SEEK_SET);

    nframes = 0;
    totalsize = 0;

    sizes = (unsigned int*) calloc( sizeof(unsigned int), frames );   // hold size of each frame
}

// add a jpeg frame to an AVI file
void avi_add(int fd, u8 *buf, int size)
{
    struct db_head db = {"00db", 0};

    printf( "avi_add: nframes = %d, totalsize = %d, size = %d\n", nframes, totalsize, size );

    // overwrite JFIF type with AVI1
    buf[6]='A';
    buf[7]='V';
    buf[8]='I';
    buf[9]='1';

    while( size%4 ) size++; // align 0 modulo 4*/
    db.size = size;

    write( fd, &db, sizeof(db) );
    write( fd, buf, size );

    sizes[nframes] = size;

    nframes++;
    totalsize += size;  // total frame size
}

// finish writing the AVI file - filling in the header
void avi_end(int fd, int width, int height, int fps)
{
    struct idx1_head idx = {"idx1", 16*nframes };
    struct db_head db = {"00db", 0};
    struct riff_head rh = { "RIFF", 0, "AVI "};
    struct list_head lh1 = {"LIST", 0, "hdrl"};
    struct avi_head ah;
    struct list_head lh2 = {"LIST", 0, "strl"};
    struct stream_head sh;
    struct frame_head fh;
    struct list_head lh3 = {"LIST", 0, "odml" };
    struct dmlh_head dh = {"dmlh", 4, nframes };
    struct list_head lh4 = {"LIST", 0, "movi"};
    int i;
    unsigned int offset = 4;

    printf( "avi_end: nframes = %d, fps = %d\n", nframes, fps );

    // write index

    write(fd, &idx, sizeof(idx));

    for ( i = 0; i < nframes; i++ )
    {
        write(fd, &db, 4 ); // only need the 00db
        fprint_quartet( fd, 18 );       // ???
        fprint_quartet( fd, offset );
        fprint_quartet( fd, sizes[i] );
        offset += sizes[i];
    }

    free( sizes );

    bzero( &ah, sizeof(ah) );
    strcpy(ah.avih, "avih");
    ah.time = 1000000 / fps;
    ah.maxbytespersec = 1000000.0*(totalsize/nframes)/ah.time;
    ah.numstreams = 1;
    ah.flags = AVIF_HASINDEX;
    ah.width = width;
    ah.height = height;

    bzero(&sh, sizeof(sh));
    strcpy(sh.strh, "strh");
    strcpy(sh.vids, "vids");
    strcpy(sh.codec, "MJPG");
    sh.scale = ah.time;
    sh.rate = 1000000;
    sh.length = nframes;

    bzero(&fh, sizeof(fh));
    strcpy(fh.strf, "strf");
    fh.width = width;
    fh.height = height;
    fh.planes = 1;
    fh.bitcount = 24;
    strcpy(fh.codec,"MJPG");
    fh.unpackedsize = 3*width*height;

    rh.size = sizeof(lh1)+sizeof(ah)+sizeof(lh2)+sizeof(sh)+
        sizeof(fh)+sizeof(lh3)+sizeof(dh)+sizeof(lh4)+
        nframes*sizeof(struct db_head)+
        totalsize + sizeof(struct idx1_head)+ (16*nframes) +4; // FIXME:16 bytes per nframe // the '4' - what for???

    lh1.size = 4+sizeof(ah)+sizeof(lh2)+sizeof(sh)+sizeof(fh)+sizeof(lh3)+sizeof(dh);
    ah.size = sizeof(ah)-8;
    lh2.size = 4+sizeof(sh)+sizeof(fh)+sizeof(lh3)+sizeof(dh);     //4+sizeof(sh)+sizeof(fh);
    sh.size = sizeof(sh)-8;
    fh.size = sizeof(fh)-8;
    fh.size2 = fh.size;
    lh3.size = 4+sizeof(dh);
    lh4.size = 4+ nframes*sizeof(struct db_head)+ totalsize;

    lseek(fd, 0, SEEK_SET);

    write(fd, &rh, sizeof(rh));
    write(fd, &lh1, sizeof(lh1));
    write(fd, &ah, sizeof(ah));
    write(fd, &lh2, sizeof(lh2));
    write(fd, &sh, sizeof(sh));
    write(fd, &fh, sizeof(fh));
    write(fd, &lh3, sizeof(lh3));
    write(fd, &dh, sizeof(dh));
    write(fd, &lh4, sizeof(lh4));
}


/* NOTE: This is not a general purpose routine - it is meant to only
   cope with AVIs saved using the other functions in this file
void avi_explode(char *fname)
{
	struct riff_head rh;
	struct list_head lh1;
	struct avi_head ah;
	struct list_head lh2;
	struct stream_head sh;
	struct frame_head fh;
	struct list_head lh3;
	int hsize, qsize;
	u16 *htables = jpeg_huffman_tables(&hsize);
	u16 *qtables = jpeg_quantisation_tables(&qsize, image_quality);
	int fd, i;

	fd = open(fname,O_RDONLY);
	if (fd == -1) {
		perror(fname);
		return;
	}

	read(fd, &rh, sizeof(rh));
	read(fd, &lh1, sizeof(lh1));
	read(fd, &ah, sizeof(ah));
	read(fd, &lh2, sizeof(lh2));
	read(fd, &sh, sizeof(sh));
	read(fd, &fh, sizeof(fh));
	read(fd, &lh3, sizeof(lh3));

	for (i=0; ; i++) {
		u8 buf[500*1024];
		struct db_head db;
		char fname[100];
		int fd2;

		if (read(fd, &db, sizeof(db)) != sizeof(db) ||
		    read(fd, buf, db.size) != db.size) break;

		snprintf(fname, sizeof(fname)-1,"frame.%d", i);

		fd2 = open(fname,O_WRONLY|O_CREAT, 0644);
		if (fd2 == -1) {
			perror(fname);
			continue;
		}
		write(fd2, buf, 2);
		write(fd2, qtables, qsize);
		write(fd2, htables, hsize);
		write(fd2, buf+2, db.size-2);
		close(fd2);
	}
	close(fd);
	printf("exploded %d frames\n", i);
}

*/

