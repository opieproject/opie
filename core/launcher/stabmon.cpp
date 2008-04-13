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


#include "stabmon.h"

#ifdef QWS
#include <qtopia/qcopenvelope_qws.h>
#endif

#include <qfile.h>

SysFileMonitor::SysFileMonitor(QObject* parent) :
    QObject(parent)
{
    startTimer(2000);
}

void SysFileMonitor::timerEvent(QTimerEvent*)
{
    // st_size is no use, it's 0 for /proc/mounts too. Read it all.
    static int mtabSize = 0;
    QFile f( "/proc/mounts" );
    if ( f.open(IO_ReadOnly) ) {
        QString s;
        // QFile.readAll does not work correctly on sequential devices (eg. /proc files) so we need to read manually
        while( !f.atEnd() ) {
            QString tmp;
            f.readLine( tmp, 1024 );
            s += tmp;
        }
        if ( (int)s.length() != mtabSize ) {
            mtabSize = (int)s.length();
#ifndef QT_NO_COP
            QCopEnvelope("QPE/Card", "mtabChanged()" );
#endif
        }
    }
}

