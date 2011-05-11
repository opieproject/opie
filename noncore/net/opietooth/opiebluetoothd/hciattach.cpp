/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2011 Paul Eggleton <bluelightning@bluelightning.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this file; see the file COPYING.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "hciattach.h"

#include <qfile.h>
#include <qtextstream.h>

#include <opie2/odebug.h>
#include <opie2/oprocess.h>

using namespace Opie::Core;

OHciAttach::OHciAttach()
    : QObject()
{
    m_process = NULL;
    readConfig();
}

OHciAttach::~OHciAttach()
{
    stop();
}

void OHciAttach::readConfig()
{
    m_port = "";
    m_protocol = "";
    m_speed = 0;
    QFile cfg("/etc/sysconfig/bluetooth");
    if ( cfg.open( IO_ReadOnly ) ) {
        QTextStream stream( &cfg );
        QString streamIn = stream.read();
        QStringList list = QStringList::split( "\n", streamIn );
        cfg.close();
        for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
            QString line = *it;
            if( !line.startsWith("#") ) {
                QStringList sline = QStringList::split( "=", line );
                if( sline.count() == 2 ) {
                    if( sline[0] == "BLUETOOTH_PORT" ) {
                        m_port = sline[1];
                    }
                    else if ( sline[0] == "BLUETOOTH_PROTOCOL" ) {
                        m_protocol = sline[1];
                    }
                    else if ( sline[0] == "BLUETOOTH_SPEED" ) {
                        m_speed = sline[1].toUInt();
                    }
                }
            }
        }
    }
}

bool OHciAttach::isConfigured()
{
    return ( (!m_port.isEmpty()) && (!m_protocol.isEmpty()) && (m_speed > 0) );
}

void OHciAttach::start()
{
    if( !m_process ) {
        m_process = new OProcess();
        *m_process << "hciattach";
        *m_process << "-n";
        *m_process << m_port << m_protocol << m_speed;

        connect(m_process, SIGNAL( processExited(Opie::Core::OProcess*) ),
                this, SLOT( slotExited(Opie::Core::OProcess* ) ) );
        //connect(m_process, SIGNAL( receivedStdout(Opie::Core::OProcess*, char*, int) ),
        //        this, SLOT(slotStdOut(Opie::Core::OProcess*,char*,int ) ) );
        //connect(m_process, SIGNAL(receivedStderr(Opie::Core::OProcess*, char*, int ) ),
        //        this, SLOT(slotStdErr(Opie::Core::OProcess*,char*,int) ) );

        if( !m_process->start( OProcess::NotifyOnExit, OProcess::AllOutput ) ) {
            owarn << "Could not start hciattach" << oendl;
            delete m_process;
            m_process = NULL;
        }
    }
}

void OHciAttach::stop()
{
    if( m_process ) {
        m_process->kill();
    }
}

void OHciAttach::slotExited( OProcess *proc )
{
    if( proc == m_process ) {
        // FIXME implement
        delete m_process;
        m_process = NULL;
    }
}

/*
void OHciAttach::slotStdOut(OProcess *proc, char *chars, int len )
{
}

void OHciAttach::slotStdErr( OProcess *proc, char *chars, int len )
{
}
*/