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

#include <stdio.h>
#include <stdlib.h>

#include "process.h"

#ifndef QT_NO_PROCESS

#include "qapplication.h"


Process::Process( QObject *parent, const char *name )
    : QObject( parent, name ), ioRedirection( FALSE ), notifyOnExit( FALSE ),
    wroteToStdinConnected( FALSE )
{
    init();
}

Process::Process( const QString& arg0, QObject *parent, const char *name )
    : QObject( parent, name ), ioRedirection( FALSE ), notifyOnExit( FALSE ),
    wroteToStdinConnected( FALSE )
{
    init();
    addArgument( arg0 );
}

Process::Process( const QStringList& args, QObject *parent, const char *name )
    : QObject( parent, name ), ioRedirection( FALSE ), notifyOnExit( FALSE ),
    wroteToStdinConnected( FALSE )
{
    init();
    setArguments( args );
}


QStringList Process::arguments() const
{
    return _arguments;
}

void Process::setArguments( const QStringList& args )
{
    _arguments = args;
}

void Process::addArgument( const QString& arg )
{
    _arguments.append( arg );
}

bool Process::exec( const QString& in, QString& out, QStringList *env )
{
    QByteArray sout;
    QByteArray sin = in.local8Bit(); sin.resize(sin.size()-1); // cut nul
    bool r = exec( sin, sout, env );
    out = QString::fromLocal8Bit(sout);
    return r;
}

#endif
