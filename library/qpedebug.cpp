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

#include "qpedebug.h"
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

struct timeval qpe_debuglast;

void qpe_debugTime( const char *file, int line, const char *desc )
{
    struct timeval tv;
    gettimeofday( &tv, 0 );

    int tdiff = tv.tv_usec - qpe_debuglast.tv_usec;
    tdiff += (tv.tv_sec - qpe_debuglast.tv_sec) * 1000000;

    fprintf( stderr, "%s:%d Time: %ld.%06ld", file, line, tv.tv_sec, tv.tv_usec);

    static int pid = getpid();

    if ( qpe_debuglast.tv_sec )
	fprintf( stderr, " (pid %d delta %dus)", pid, tdiff );

    if ( desc )
	fprintf( stderr, " (%s)", desc );

    fprintf( stderr, "\n" );

    qpe_debuglast = tv;
}


