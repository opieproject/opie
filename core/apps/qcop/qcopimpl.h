/**********************************************************************
** Copyright (C) 2000-2005 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
** 
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2 of the License, or (at your
** option) any later version.
** 
** A copy of the GNU GPL license version 2 is included in this package as 
** LICENSE.GPL.
**
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
** See the GNU General Public License for more details.
**
** In addition, as a special exception Trolltech gives permission to link
** the code of this program with Qtopia applications copyrighted, developed
** and distributed by Trolltech under the terms of the Qtopia Personal Use
** License Agreement. You must comply with the GNU General Public License
** in all respects for all of the code used other than the applications
** licensed under the Qtopia Personal Use License Agreement. If you modify
** this file, you may extend this exception to your version of the file,
** but you are not obligated to do so. If you do not wish to do so, delete
** this exception statement from your version.
** 
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QCOPIMPL_H
#define QCOPIMPL_H

#ifdef QWS
#include <qtopia/qcopenvelope_qws.h>
#endif

#include <qtopia/qpeapplication.h>
#include <qstringlist.h>
#include <qdatastream.h>
#include <qtimer.h>

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#ifndef Q_OS_WIN32
#include <pwd.h>
#include <unistd.h>
#include <grp.h>
#endif

// No tr() anywhere in this file

void doqcopusage();
void doqcopsyntax( const QString &where, const QString &what );
int doqcopimpl (int argc, char *argv[]);

class QCopWatcher : public QObject
{
    Q_OBJECT
public:
    QCopWatcher( QObject *parent, const QString& msg );
    ~QCopWatcher();

signals:
    void done();

public slots:
    void received( const QCString& msg, const QByteArray& data );
    void timeout();

private:
    QCString msg;
};

#endif
