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

#include "capture.h"

#include "zcameraio.h"
#include "imageio.h"
#include "avi.h"

#include <opie2/oapplication.h>
#include <opie2/odebug.h>

#include <qimage.h>

using namespace Opie::Core;
using namespace Opie::Core;
Capturer::Capturer()
         :QFrame( 0 ), height( 320 ), width( 240 ), zoom( 1 ), quality( 90 ),
         flip( "A" ), format( "JPEG" ), name( "Untitled" )
{
}


Capturer::~Capturer()
{
}


void Capturer::checkSettings()
{
    if ( width > height )
    {
        if ( 0 != width % 16 || width < 16 || width > 640 )
        {
            printf( "Warning: Corrected X resolution to 320 px\n" );
            width = 320;
        }
        if ( 0 != height % 16 || height < 16 || height > 480 )
        {
            printf( "Warning: Corrected Y resolution to 240 px\n" );
            height = 240;
        }
    }
    else
    {
        if ( 0 != width % 16 || width < 16 || width > 480 )
        {
            printf( "Warning: Corrected X resolution to 240 px\n" );
            width = 240;
        }
        if ( 0 != height % 16 || height < 16 || height > 640 )
        {
            printf( "Warning: Corrected Y resolution to 320 px\n" );
            height = 320;
        }
    }

    if ( quality > 100 || quality < 10 )
    {
        printf( "Warning: Corrected quality to 75%%\n" );
        quality = 75;
    }

    if ( zoom > 2 || zoom < 1 )
    {
        printf( "Warning: Corrected zoom to x1\n" );
        zoom = 1;
    }

    if ( format != "JPEG" && format != "PNG" && format != "BMP" )
    {
        printf( "Warning: Corrected format to 'JPEG'\n" );
        format = "JPEG";
    }
}


void Capturer::capture()
{
    if ( flip == "A" )
        ZCameraIO::instance()->setFlip( ZCameraIO::AUTOMATICFLIP );
    else if ( flip == "0" )
        ZCameraIO::instance()->setFlip( ZCameraIO::XNOFLIP | ZCameraIO::YNOFLIP );
    else if ( flip == "X" )
        ZCameraIO::instance()->setFlip( ZCameraIO::XFLIP );
    else if ( flip == "Y" )
        ZCameraIO::instance()->setFlip( ZCameraIO::YFLIP );
    else if ( flip == "*" )
        ZCameraIO::instance()->setFlip( ZCameraIO::XFLIP | ZCameraIO::YFLIP );

    ZCameraIO::instance()->captureFrame( width, height, zoom, &image );
    QImage im = image.convertDepth( 32 );
    bool result = im.save( name, format, quality );
    if ( !result )
    {
        printf( "QImageio-Problem while writing.\n" );
    }
    else
    {
        printf( "Ok.\n" );
    }
}


void usage()
{
    printf( "Usage: ./capture [options] filename\n\n" );
    printf( "  -x xresolution (dividable by 16) [default=240]\n" );
    printf( "  -y xresolution (dividable by 16) [default=320]\n" );
    printf( "  -q quality (10-100) [default=75]\n" );
    printf( "  -f flip (A=auto, 0, X, Y, *=both) [default=Auto]\n" );
    printf( "  -o output format (JPEG,BMP,PNG) [default=JPEG]\n" );
    printf( "  -z zoom (1-2) [default=1]\n" );
}

int main( int argc, char** argv )
{
    OApplication* a = new OApplication( argc, argv, "Capture" );
    Capturer* c = new Capturer();

    if ( argc < 2 )
    {
        usage();
        return -1;
    }

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
                c->name = argv[i];
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
                case 'x': c->width = QString( argv[i] ).toInt(); break;
                case 'y': c->height = QString( argv[i] ).toInt(); break;
                case 'z': c->zoom = QString( argv[i] ).toInt(); break;
                case 'o': c->format = QString( argv[i] ); break;
                case 'q': c->quality = QString( argv[i] ).toInt(); break;
                case 'f': c->flip = QString( argv[i] )[0]; break;
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

    c->checkSettings();
    c->capture();
    return 0;
}

