/**********************************************************************
** Copyright (C) 2004 Michael 'Mickey' Lauer <mickey@vanille.de>
** All rights reserved.
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

/* STD */
#include <pwd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main( int argc, char** argv )
{
    printf( "\nPyQuicklaunch Launcher (numargs=%d, arg[0]='%s', arg[1]='%s')\n", argc, argv[0], argv[1] );

    char fifoName[1024];
    sprintf( &fifoName[0], "/tmp/mickeys-quicklauncher-%s", ::getpwuid( ::getuid() )->pw_name );


    int fd = open( &fifoName[0], O_WRONLY | O_NONBLOCK );
    if ( fd == -1 )
    {
        perror( "Can't open fifo" );
        return -1;
    }

    char wdPath[1024];
    getcwd( &wdPath[0], sizeof( wdPath ) );

    char scriptPath[1024];
    if ( argv[1][0] == '/' )
    {
        strcpy( &scriptPath[0], argv[1] );
    }
    else
    {
        sprintf( &scriptPath[0], "%s/%s", wdPath, argv[1] );
    }

    printf( "\nInstructing the Quicklauncher to launch '%s'...", &scriptPath[0] );

    int res = write( fd, scriptPath, strlen( scriptPath ) );
    if ( res < 1 )
    {
        perror( "Can't write string to fifo" );
        return -1;
    }

    close( fd );

    return 0;
}
