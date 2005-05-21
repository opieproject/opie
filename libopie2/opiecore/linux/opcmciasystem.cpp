/*
                             This file is part of the Opie Project
              =.             (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
     ._= =}       :
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "opcmciasystem.h"
using namespace Opie::Core;

/* OPIE */
#include <opie2/odebug.h>

/* QT */
#include <qfile.h>
#include <qtextstream.h>

/* STD */
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

/*======================================================================================
 * OPcmciaSystem
 *======================================================================================*/

OPcmciaSystem* OPcmciaSystem::_instance = 0;

OPcmciaSystem::OPcmciaSystem()
{
    qDebug( "OPcmciaSystem::OPcmciaSystem()" );
    synchronize();
}

void OPcmciaSystem::synchronize()
{
    qDebug( "OPcmciaSystem::synchronize()" );
    _interfaces.clear();

    //FIXME: Use cardmgr subsystem ioctls

    QString fileName;
    if ( QFile::exists( "/var/run/stab" ) ) { fileName = "/var/run/stab"; }
    else if ( QFile::exists( "/var/state/pcmcia/stab" ) ) { fileName = "/var/state/pcmcia/stab"; }
    else { fileName = "/var/lib/pcmcia/stab"; }
    QFile cardinfofile( fileName );
    if ( !cardinfofile.exists() || !cardinfofile.open( IO_ReadOnly ) )
    {
        qWarning( "pcmcia info file not found or unaccessible" );
        return;
    }
    QTextStream cardinfo( &cardinfofile );
    while ( !cardinfo.atEnd() )
    {
        QString strSocket;
        int numSocket;
        char colon;
        QString cardName;
        cardinfo >> strSocket >> numSocket >> colon;
        cardName = cardinfo.readLine().stripWhiteSpace();
        qDebug( "strSocket = '%s', numSocket = '%d', colon = '%c', cardName = '%s'", (const char*) strSocket, numSocket, colon, ( const char*) cardName );
        if ( strSocket == "Socket" && colon == ':' )
        {
            _interfaces.append( new OPcmciaSocket( numSocket, this, (const char*) cardName ) );
        }
        else
        {
            continue;
        }
    }
}


int OPcmciaSystem::count() const
{
    return _interfaces.count();
}


int OPcmciaSystem::cardCount() const
{
    int nonEmpty = 0;
    OPcmciaSystem::CardIterator it = iterator();
    while ( it.current() )
    {
        if ( !it.current()->isEmpty() ) nonEmpty++;
        ++it;
    }
    return nonEmpty;
}


OPcmciaSocket* OPcmciaSystem::socket( unsigned int number )
{
    return _interfaces.at( number );
}


OPcmciaSystem* OPcmciaSystem::instance()
{
    if ( !_instance ) _instance = new OPcmciaSystem();
    return _instance;
}


OPcmciaSystem::CardIterator OPcmciaSystem::iterator() const
{
    return OPcmciaSystem::CardIterator( _interfaces );
}


/*======================================================================================
 * OPcmciaSocket
 *======================================================================================*/

OPcmciaSocket::OPcmciaSocket( int socket, QObject* parent, const char* name )
                 :QObject( parent, name ), _socket( socket )
{
    odebug << "OPcmciaSocket::OPcmciaSocket()" << oendl;
    init();
}


OPcmciaSocket::~OPcmciaSocket()
{
}


/* internal */ void OPcmciaSocket::init()
{
}

/* internal */ bool OPcmciaSocket::command( const QString& cmd )
{
    QString cmdline = QString().sprintf( "cardctl %s %d &", (const char*) cmd, _socket );
    ::system( (const char*) cmdline );
}

int OPcmciaSocket::number() const
{
    return _socket;
}


QString OPcmciaSocket::identity() const
{
    return ( strcmp( name(), "empty" ) == 0 ) ? "<Empty Socket>" : name();
}


bool OPcmciaSocket::isUnsupported() const
{
    return ( strcmp( name(), "unsupported card" ) == 0 );
}


bool OPcmciaSocket::isEmpty() const
{
    return ( strcmp( name(), "empty" ) == 0 );
}


bool OPcmciaSocket::isSuspended() const
{
    //FIXME
    return false;
}

bool OPcmciaSocket::eject()
{
    return command( "eject" );
}

bool OPcmciaSocket::insert()
{
    return command( "insert" );
}

bool OPcmciaSocket::suspend()
{
    return command( "suspend" );
}

bool OPcmciaSocket::resume()
{
    return command( "resume");
}

bool OPcmciaSocket::reset()
{
    return command( "reset");
}
