#include  "gif_slave.h"

#include "thumbnailtool.h"

#include <qimage.h>
#include <qobject.h>
#include <qfile.h>
#include <qpixmap.h>


PHUNK_VIEW_INTERFACE( "Gif", GifSlave );


namespace {
/*
** $Id: gif_slave.cpp,v 1.1 2004-03-22 23:34:31 zecke Exp $
**
** Minimal GIF parser, for use in extracting and setting metadata.
** Modified for standalone & KDE calling by Bryce Nesbitt
**
**  TODO:
**      Support gif comments that span more than one comment block.
**      Verify that Unicode utf-8 is fully unmolested by this code.
**      Implement gif structure verifier.
**
** Based on: GIFtrans v1.12.2
** Copyright (C) 24.2.94 by Andreas Ley <ley@rz.uni-karlsruhe.de>
**
*******************************************************************************
**
** Original distribution site is
**      ftp://ftp.rz.uni-karlsruhe.de/pub/net/www/tools/giftrans/giftrans.c
** A man-page by knordlun@fltxa.helsinki.fi (Kai Nordlund) is at
**      ftp://ftp.rz.uni-karlsruhe.de/pub/net/www/tools/giftrans/giftrans.1
** An online version by taylor@intuitive.com (Dave Taylor) is at
**         http://www.intuitive.com/coolweb/Addons/giftrans-doc.html
** To compile for MS-DOS or OS/2, you need getopt:
**      ftp://ftp.rz.uni-karlsruhe.de/pub/net/www/tools/giftrans/getopt.c
** MS-DOS executable can be found at
**      ftp://ftp.rz.uni-karlsruhe.de/pub/net/www/tools/giftrans/giftrans.exe
** OS/2 executable can be found at
**      ftp://ftp.rz.uni-karlsruhe.de/pub/net/www/tools/giftrans/giftrans.os2.exe
** A template rgb.txt for use with the MS-DOS version can be found at
**      ftp://ftp.rz.uni-karlsruhe.de/pub/net/www/tools/giftrans/rgb.txt
** Additional info can be found on
**      http://melmac.corp.harris.com/transparent_images.html
**
** The GIF file format is documented in
**      ftp://ftp.uu.net/doc/literary/obi/Standards/Graphics/Formats/gif89a.doc.Z
** A good quick reference is at:
**      http://www.goice.co.jp/member/mo/formats/gif.html
**
*******************************************************************************
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
*/
#define SUCCESS (0)
#define FAILURE (1)

#define READ_BINARY     "r"
#define WRITE_BINARY    "w"


static long int pos;
static char     skipcomment,verbose;
char     *global_comment;

#define readword(buffer)        ((buffer)[0]+256*(buffer)[1])
#define readflag(buffer)        ((buffer)?true:false)
#define hex(c)                  ('a'<=(c)&&(c)<='z'?(c)-'a'+10:'A'<=(c)&&(c)<='Z'?(c)-'A'+10:(c)-'0')

static bool debug = false;
static bool output= false;

void dump(long int, unsigned char *, size_t) {
}

void skipdata(FILE *src)
{
        unsigned char   size,buffer[256];

        do {
                pos=ftell(src);
                (void)fread((void *)&size,1,1,src);
                if (debug)
                        dump(pos,&size,1);
                if (debug) {
                        pos=ftell(src);
                        (void)fread((void *)buffer,(size_t)size,1,src);
                        dump(pos,buffer,(size_t)size);
                }
                else
                        (void)fseek(src,(long int)size,SEEK_CUR);
        } while (!feof(src)&&size>0);
}


void transdata(FILE *src, FILE *dest)
{
        unsigned char   size,buffer[256];

        do {
                pos=ftell(src);
                (void)fread((void *)&size,1,1,src);
                pos=ftell(src);
                (void)fread((void *)buffer,(size_t)size,1,src);
                if (debug)
                        dump(pos,buffer,(size_t)size);
                if (output)
                        (void)fwrite((void *)buffer,(size_t)size,1,dest);
        } while (!feof(src)&&size>0);
}

void transblock(FILE *src, FILE* dest)
{
        unsigned char   size,buffer[256];

        pos=ftell(src);
        (void)fread((void *)&size,1,1,src);
        if (debug)
                dump(pos,&size,1);
        if (output)
                (void)fwrite((void *)&size,1,1,dest);
        pos=ftell(src);
        (void)fread((void *)buffer,(size_t)size,1,src);
        if (debug)
                dump(pos,buffer,(size_t)size);
        if (output)
                (void)fwrite((void *)buffer,(size_t)size,1,dest);
}

void dumpcomment(FILE *src, QCString& str)
{
        unsigned char   size;

        pos=ftell(src);
        (void)fread((void *)&size,1,1,src);
        if (debug)
                dump(pos,&size,1);
        str.resize( size+1 );
        (void)fread((void *)str.data(),size,1,src);
        (void)fseek(src,(long int)pos,SEEK_SET);
}




int giftrans(FILE *src, FILE* dest, QString& str, bool full)
{
        unsigned char   buffer[3*256],lsd[7],gct[3*256];
        unsigned int    size,gct_size;

        /* Header */
        pos=ftell(src);
        (void)fread((void *)buffer,6,1,src);
        if (strncmp((char *)buffer,"GIF",3)) {
		str = QObject::tr("Not a GIF file");
                (void)fprintf(stderr,"Not GIF file!\n");
                return(1);
        }

        /* Logical Screen Descriptor */
        pos=ftell(src);
        (void)fread((void *)lsd,7,1,src);
                //(void)fprintf(stderr,"Logical Screen Descriptor:\n");
            str += QObject::tr("Dimensions: %1x%2\n").arg( readword(lsd) ).arg( readword(lsd+2 ) );
                //(void)fprintf(stderr,"Global Color Table Flag: %s\n",readflag(lsd[4]&0x80));
	    str += QObject::tr("Depth: %1 bits\n").arg( (lsd[4]&0x70>>4 )+1);
                //(void)fprintf(stderr,"Depth  : %d bits\n",(lsd[4]&0x70>>4)+1);
                if (lsd[4]&0x80 && full) {
                    str += QObject::tr("Sort Flag: %1\n" ).arg(readflag(lsd[4]&0x8) );
                    str += QObject::tr("Size of Global Color Table: %1 colors\n" ).arg( 2<<(lsd[4]&0x7));
                    str += QObject::tr("Background Color Index: %1\n" ).arg(lsd[5]);
                }
                if (lsd[6] && full)
                    str += QObject::tr("Pixel Aspect Ratio: %1 (Aspect Ratio %2)\n" ).arg( lsd[6] ).
                           arg( ((double)lsd[6]+15)/64 );

        /* Global Color Table */
        if (lsd[4]&0x80) {
                gct_size=2<<(lsd[4]&0x7);
                pos=ftell(src);
                (void)fread((void *)gct,gct_size,3,src);
        }

        do {
                pos=ftell(src);
                (void)fread((void *)buffer,1,1,src);
                switch (buffer[0]) {
                case 0x2c:      /* Image Descriptor */
                        (void)fread((void *)(buffer+1),9,1,src);
                        /* Local Color Table */
                        if (buffer[8]&0x80) {
                                size=2<<(buffer[8]&0x7);
                                pos=ftell(src);
                                (void)fread((void *)buffer,size,3,src);
                        }
                        /* Table Based Image Data */
                        pos=ftell(src);
                        (void)fread((void *)buffer,1,1,src);
                        transdata(src,dest);
                        break;
                case 0x21:      /* Extension */
                        (void)fread((void *)(buffer+1),1,1,src);
                        switch (buffer[1]) {
                        case 0xfe:      /* Comment Extension */
                                if (true)
                                {
                                    QCString st;
                                    dumpcomment(src, st);
                                    str += QObject::tr("Comment: %1\n" ).arg( st );
                                }
                                if (skipcomment)
                                        skipdata(src);
                                else {
                                        transdata(src,dest);
                                }
                                break;
                        case 0x01:      /* Plain Text Extension */
                        case 0xf9:      /* Graphic Control Extension */
                        case 0xff:      /* Application Extension */
                        default:
                                transblock(src,dest);
                                transdata(src,dest);
                                break;
                        }
                        break;
                case 0x3b:      /* Trailer (write comment just before here) */
                        break;
                default:
                        (void)fprintf(stderr,"0x%08lx: Error, unknown block 0x%02x!\n",ftell(src)-1,buffer[0]);
                        return(1);
                }
        } while (buffer[0]!=0x3b&&!feof(src));
        return(buffer[0]==0x3b?SUCCESS:FAILURE);
}


/****************************************************************************/
extern void get_gif_info( const char * original_filename, QString& str,
                          bool full =false)
{
FILE    * infile;

    if ((infile = fopen(original_filename, READ_BINARY)) == NULL) {
        fprintf(stderr, "can't open gif image '%s'\n", original_filename);
        return ;
        }

    output      = FALSE;
    verbose     = TRUE;
    debug       = FALSE;
    skipcomment = FALSE;
    giftrans( infile, NULL, str, full );
    fclose( infile );
}

}


GifSlave::GifSlave()
    : SlaveInterface(QStringList("gif"))
{}

GifSlave::~GifSlave() {

}

QString GifSlave::iconViewName(const QString& str) {
    QString st;
    get_gif_info(QFile::encodeName( str ).data(), st );
    return st;
}

QString GifSlave::fullImageInfo( const QString& str) {
    QString st;
    get_gif_info(QFile::encodeName( str ).data(), st, true );
    return st;
}

QPixmap GifSlave::pixmap(const QString& path, int width, int height ) {
    static QImage img;
    img.load( path );
    if ( img.isNull() ) {
        QPixmap pix;
        return pix;
    }

    return ThumbNailTool::scaleImage( img, width,height );
}


