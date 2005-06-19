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
#include <unistd.h>

#define PROC_DEVICES "/proc/devices"

// #define OPCMCIA_DEBUG 1

/*======================================================================================
 * OPcmciaSystem
 *======================================================================================*/

OPcmciaSystem* OPcmciaSystem::_instance = 0;

OPcmciaSystem::OPcmciaSystem()
              :_major( 0 )
{
    qDebug( "OPcmciaSystem::OPcmciaSystem()" );

    // get major node number out of /proc/devices
    QFile procfile( PROC_DEVICES );
    if ( procfile.exists() && procfile.open( IO_ReadOnly ) )
    {
        QTextStream devstream( &procfile );
        devstream.readLine(); // skip header
        while ( !devstream.atEnd() && !_major )
        {
            int nodenumber;
            QString driver;
            devstream >> nodenumber >> driver;
            if ( driver == "pcmcia" )
            {
                qDebug( "OPcmciaSystem::OPcmciaSystem(): gotcha! pcmcia node number = %d", nodenumber );
                _major = nodenumber;
                break;
            }
        }
    }
    else
    {
        qWarning( "OPcmciaSystem::OPcmciaSystem() - can't open /proc/devices - continuing with limited functionality." );
    }

    synchronize();
}

void OPcmciaSystem::synchronize()
{
    qDebug( "OPcmciaSystem::synchronize()" );
    _interfaces.clear();

    //NOTE: We _could_ use ioctl's here as well, however we want to know if
    //      the card is recognized by the cardmgr (hence has a valid binding)
    //      If it is not recognized yet, userland may want to provide a configuration dialog
    //TODO: Revise for pcmciautils

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
            _interfaces.append( new OPcmciaSocket( _major, numSocket, this, (const char*) cardName ) );
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

OPcmciaSocket::OPcmciaSocket( int major, int socket, QObject* parent, const char* name )
                 :QObject( parent, name ), _major( major ), _socket( socket )
{
    qDebug( "OPcmciaSocket::OPcmciaSocket()" );
    init();
}


OPcmciaSocket::~OPcmciaSocket()
{
    qDebug( "OPcmciaSocket::~OPcmciaSocket()" );
    cleanup();
}


/* internal */ void OPcmciaSocket::init()
{
    // open control socket and gather file descriptor
    if ( _major )
    {
        dev_t dev = makedev( _major, _socket );

#ifdef OPCMCIA_DEBUG
        QString filename = "/tmp/opcmciasystem-debug";
        if ( QFile::exists( filename ) )
#else
        QString filename = QString().sprintf( "/tmp/opcmciasystem-%d", ::getpid() );
        if ( ::mknod( (const char*) filename, ( S_IFCHR|S_IREAD|S_IWRITE ), dev ) == 0 )
#endif
        {
            _fd = ::open( (const char*) filename, O_RDONLY);
            if ( !_fd )
            {
                qWarning( "OPcmciaSocket::init() - can't open control socket (%s)", strerror( errno ) );
            }
            else
            {
                ::unlink( (const char*) filename );
            }
        }
        else
        {
            qWarning( "OPcmciaSocket::init() - can't create device node '%s' (%s)", (const char*) filename, strerror( errno ) );
        }
    }
}

/* internal */ void OPcmciaSocket::cleanup()
{
    // close control socket
}

/* internal */ bool OPcmciaSocket::getTuple( cisdata_t tuple ) const
{
    _ioctlarg.tuple.DesiredTuple = tuple;
    _ioctlarg.tuple.Attributes = TUPLE_RETURN_COMMON;
    _ioctlarg.tuple.TupleOffset = 0;

    int result;
    result = ::ioctl(_fd, DS_GET_FIRST_TUPLE, &_ioctlarg);
    if ( result != 0 )
    {
        qWarning( "OPcmciaSocket::getTuple() - DS_GET_FIRST_TUPLE failed (%s)", strerror( errno ) );
        return false;
    }

    result = ::ioctl(_fd, DS_GET_TUPLE_DATA, &_ioctlarg);
    if ( result != 0 )
    {
        qWarning( "OPcmciaSocket::getTuple() - DS_GET_TUPLE_DATA failed (%s)", strerror( errno ) );
        return false;
    }

    result = ::ioctl( _fd, DS_PARSE_TUPLE, &_ioctlarg );
    if ( result != 0 )
    {
        qWarning( "OPcmciaSocket::getTuple() - DS_PARSE_TUPLE failed (%s)", strerror( errno ) );
        return false;
    }

    return true;
}


int OPcmciaSocket::number() const
{
    return _socket;
}


QString OPcmciaSocket::identity() const
{
    return ( strcmp( name(), "empty" ) == 0 ) ? "<Empty Socket>" : name();
}


const OPcmciaSocket::OPcmciaSocketCardStatus OPcmciaSocket::status() const
{
    cs_status_t cs_status;
    cs_status.Function = 0;
    int result = ::ioctl( _fd, DS_GET_STATUS, &cs_status );
    if ( result != 0 )
    {
        qWarning( "OPcmciaSocket::status() - DS_GET_STATUS failed (%s)", strerror( errno ) );
        return Unknown;
    }
    else
    {
        qDebug( " card   status = 0x%08x", cs_status.CardState );
        qDebug( " socket status = 0x%08x", cs_status.SocketState );
        return (OPcmciaSocket::OPcmciaSocketCardStatus) (cs_status.CardState + cs_status.SocketState);
    }
}


bool OPcmciaSocket::isUnsupported() const
{
    return ( strcmp( name(), "unsupported card" ) == 0 );
}


bool OPcmciaSocket::isEmpty() const
{
    return ! status() && ( Occupied || OccupiedCardBus );
}


bool OPcmciaSocket::isSuspended() const
{
    return status() && Suspended;
}


bool OPcmciaSocket::eject()
{
    return ::ioctl( _fd, DS_EJECT_CARD ) != -1;
}


bool OPcmciaSocket::insert()
{
    return ::ioctl( _fd, DS_INSERT_CARD ) != -1;
}


bool OPcmciaSocket::suspend()
{
    return ::ioctl( _fd, DS_SUSPEND_CARD ) != -1;
}


bool OPcmciaSocket::resume()
{
    return ::ioctl( _fd, DS_RESUME_CARD ) != -1;
}


bool OPcmciaSocket::reset()
{
    return ::ioctl( _fd, DS_RESET_CARD ) != -1;
}


QStringList OPcmciaSocket::productIdentity() const
{
    QStringList list;
    cistpl_vers_1_t *vers = &_ioctlarg.tuple_parse.parse.version_1;
    if ( getTuple( CISTPL_VERS_1 ) )
    {
        for ( int i = 0; i < CISTPL_VERS_1_MAX_PROD_STRINGS; ++i )
        {
            qDebug( " PRODID = '%s'", vers->str+vers->ofs[i] );
            list += vers->str+vers->ofs[i];
        }
    }
    else
    {
        list += "<unknown>";
    }
    return list;
}


QString OPcmciaSocket::manufacturerIdentity() const
{
    cistpl_manfid_t *manfid = &_ioctlarg.tuple_parse.parse.manfid;
    if ( getTuple( CISTPL_MANFID ) )
    {
        return QString().sprintf( "0x%04x, 0x%04x", manfid->manf, manfid->card );
    }
    else
        return "<unknown>";
}


QString OPcmciaSocket::function() const
{
    cistpl_funcid_t *funcid = &_ioctlarg.tuple_parse.parse.funcid;
    if ( getTuple( CISTPL_FUNCID ) )
    {
        switch ( funcid->func )
        {
            case 0: return "Multifunction"; break;
            case 1: return "Memory"; break;
            case 2: return "Serial"; break;
            case 3: return "Parallel"; break;
            case 4: return "Fixed Disk"; break;
            case 5: return "Video"; break;
            case 6: return "Network"; break;
            case 7: return "AIMS"; break;
            case 8: return "SCSI"; break;
            default: return "<unknown>"; break;
        }
    }
    else
    {
        return "<unknown>";
    }
}
