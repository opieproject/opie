/**********************************************************************
** Copyright (C) 2003 Michael 'Mickey' Lauer.  All rights reserved.
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

#include "zcameraio.h"
#include "imageio.h"
#include "avi.h"

#include <qfile.h>
#include <qimage.h>
#include <opie2/oapplication.h>

#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

#define CAPTUREFILE "/tmp/capture.dat"
#define OUTPUTFILE  "/tmp/output.avi"

int captureX = 240;
int captureY = 320;
int quality = 75;
QString flip = "A";
int zoom = 1;
QString format = "JPG";
QString name;

int performCapture();

void usage()
{
    printf( "Usage: ./capture [options] filename\n\n" );
    printf( "  -x xresolution (dividable by 16) [default=240]\n" );
    printf( "  -y xresolution (dividable by 16) [default=320]\n" );
    printf( "  -q quality (10-100) [default=75]\n" );
    printf( "  -f flip (A=auto, X, Y, *=both) [default=Auto]\n" );
    printf( "  -o output format (JPG,BMP,PNG) [default=JPG]\n" );
    printf( "  -z zoom (1-2) [default=1]\n" );
}

int main( int argc, char** argv )
{
    OApplication* a = new OApplication( argc, argv, "opie-camera" );

    if ( argc < 2 )
    {
        usage();
        return -1;
    }

    int captureX = 240;
    int captureY = 320;
    QString flip = "A";

    #define I_HATE_WRITING_HARDCODED_PARSES

    int i = 1;
    while ( i < argc )
    {
        // check for filename
        if ( argv[i][0] != '-' )
        {
            if ( argc != i+1 )
            {
                usage();
                return -1;
            }
            else
            {
                name = argv[i];
                break;
            }
        }
        else
        {
            i++;
            if ( argc == i )
            {
                usage();
                return -1;
            }
            switch ( argv[i-1][1] )
            {
                case 'x': captureX = QString( argv[i] ).toInt(); break;
                case 'y': captureY = QString( argv[i] ).toInt(); break;
                case 'z': zoom = QString( argv[i] ).toInt(); break;
                case 'o': format = QString( argv[i] ); break;
                case 'q': quality = QString( argv[i] ).toInt(); break;
                case 'f': flip = QString( argv[i] )[0]; break;
                default: usage(); return -1;
            }
            i++;
        }

    #undef I_HATE_WRITING_HARDCODED_PARSES
    }

    if ( !ZCameraIO::instance()->isOpen() )
    {
        printf( "Error: Can't detect your camera. Exiting.\n" );
        return -1;
    }
    return performCapture();
}


int performCapture()
{
   printf( "Capturing %dx%d Image [%s] q%d z%d --> %s\n", captureX,
                                                          captureY,
                                                          (const char*) format,
                                                          quality,
                                                          zoom,
                                                          (const char*) name );

    QImage i;
    ZCameraIO::instance()->captureFrame( captureX, captureY, zoom, &i );
    QImage im = i.convertDepth( 32 );
    bool result = im.save( name, format, quality );
    if ( !result )
    {
        printf( "QImageio-Problem while writing.\n" );
        return -1;
    }
    else
    {
        printf( "Ok.\n" );
    }
}

