/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
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

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include <qimage.h>

#include <opie2/odebug.h>

ZCameraIO* ZCameraIO::_instance = 0;

ZCameraIO* ZCameraIO::instance()
{
    if ( !ZCameraIO::_instance )
    {
        odebug << "Creating ZCameraIO::_instance" << oendl;
        ZCameraIO::_instance = new ZCameraIO();
    }
    return ZCameraIO::_instance;
}


ZCameraIO::ZCameraIO()
          :_height( 0 ), _width( 0 ), _zoom( 0 ), _rot( 0 ), _readlen( 0 )
{
    _driver = ::open( "/dev/sharp_zdc", O_RDWR );
    if ( _driver == -1 )
        oerr << "Can't open camera driver: " << strerror(errno) << oendl;
    else
        init();
}


void ZCameraIO::init()
{
    if ( ZCameraIO::_instance )
        ofatal << "Don't create more than one ZCameraIO instances." << oendl;
    else
    {
        setReadMode( STATUS );
    }
}


ZCameraIO::~ZCameraIO()
{
    if ( _driver != -1 )
        ::close( _driver );
}


inline bool ZCameraIO::isOpen() const
{
    return _driver != -1;
}


inline bool ZCameraIO::isShutterPressed()
{
    return _status[0] == 'S';
    clearShutterLatch();
}


inline bool ZCameraIO::isFinderReversed() const
{
    return _status[1] == 'M';
}


inline bool ZCameraIO::isCapturing() const
{
    return _status[2] == 'C';
}


inline bool ZCameraIO::isAvailable() const
{
    return _status[3] == 'A';
}


bool ZCameraIO::setCaptureFrame( int width, int height, int zoom, bool rot )
{
    char b[100];
    sprintf( b, "%c=%d,%d,%d,%d", rot ? 'R':'S', width, height, zoom, width*2 );
    if ( write( b ) )
    {
        _width = width;
        _height = _height;
        _zoom = zoom;
        _rot = rot;
        _readlen = 2 * _width * _height; // camera is fixed @ 16 bits per pixel
        return true;
    }
    return false;
}


void ZCameraIO::setReadMode( int mode )
{
    char b[4];
    sprintf( b, "M=%d", mode );
    write( b, mode <= 9 ? 3 : 4 );
    if ( mode & 1 ) // STATUS bit is set
        read( _status, 4 );
}


void ZCameraIO::clearShutterLatch()
{
    char b = 'B';
    write( &b, 1 );
}


bool ZCameraIO::read( char* b, int len )
{
    int rlen = ::read( _driver, b, len );
    odebug << "read " << rlen << " from driver." << oendl;
    return rlen == len;
}


bool ZCameraIO::write( char* buf, int len )
{
    if ( !len )
        len = strlen( buf );

    odebug << "writing '" << buf << "' to driver." << oendl;

    return ::write( _driver, buf, len ) == len;
}


bool ZCameraIO::snapshot( QImage* image )
{
    /*

    char buf[76800];

    write( _driver, "M=13", 4 );
    write( _driver, "R=240,160,256,480", 17 );
    write( _driver, "M=12", 4 );

    int result = read( _driver, &buf, sizeof buf );

    return result == sizeof buf;

    */

    unsigned char buf[76800];
    unsigned char* bp = buf;
    unsigned char* p;

    int fd = open( "/tmp/cam", O_RDONLY );
    if ( ::read( fd, buf, sizeof buf ) != sizeof buf )
        owarn << "Couldn't read image from /dev/sharp_zdc" << oendl;

    image->create( 240, 160, 16 );
    for ( int i = 0; i < 160; ++i )
    {
        p = image->scanLine( i );
        for ( int j = 0; j < 240; j++ )
        {
            *p = *bp;
            p++;
            bp++;
            *p = *bp;
            p++;
            bp++;
        }
    }

    return true;
}

bool ZCameraIO::snapshot( uchar* buf )
{
    int fd = open( "/tmp/cam", O_RDONLY );
    if ( ::read( fd, buf, 76800 ) != 76800 )
        owarn << "Couldn't read image from /dev/sharp_zdc" << oendl;

    return true;
}

