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

#include <qimage.h>

#include <opie2/odebug.h>

ZCameraIO::ZCameraIO()
{
    _driver = open( "/dev/sharp_zdc", O_RDWR );
    if ( _driver == -1 )
        oerr << "Can't open camera driver: " << strerror(errno) << oendl;

};


ZCameraIO::~ZCameraIO()
{
    if ( _driver != -1 )
        close( _driver );
}


bool ZCameraIO::snapshot( QImage* img )
{
    char buf[76800];

    write( _driver, "M=13", 4 );
    write( _driver, "R=240,160,256,480", 17 );
    write( _driver, "M=12", 4 );

    int result = read( _driver, &buf, sizeof buf );

    return result == sizeof buf;
}
