/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define QUICKEXEC "/tmp/.quickexec"

int quickexecv( const char *path, const char *argv[] )
{
    int fd = open( QUICKEXEC, O_WRONLY );
    if ( fd == -1 ) {
	perror( "quickexec pipe" );
	return -1;
    }
    write( fd, path, strlen( path )+1 );
    const char **s = argv;
    while( *s ) {
	write( fd, *s, strlen( *s )+1 );
	++s;
    }
    close(fd);
    return 0;
}

int quickexec( const char *path, const char *, ...)
{
    int fd = open( QUICKEXEC, O_WRONLY );
    if ( fd == -1 ) {
	perror( "quickexec pipe" );
	return -1;
    }
    const char** s = &path;
    do {
	write( fd, *s, strlen( *s )+1 );
    } while ( *(++s) );

    close( fd );

    return 0;
}
