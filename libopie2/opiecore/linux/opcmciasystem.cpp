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
    buildInformation();
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
        QString filename = QString().sprintf( "/tmp/opcmciasystem-%d", ::getpid() );
        if ( ::mknod( (const char*) filename, ( S_IFCHR|S_IREAD|S_IWRITE ), dev ) == 0 )
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
            qWarning( "OPcmciaSocket::init() - can't create device node (%s)", strerror( errno ) );
        }
    }
}

/* internal */ void OPcmciaSocket::buildInformation()
{
    cistpl_vers_1_t *vers = &_ioctlarg.tuple_parse.parse.version_1;
    cistpl_manfid_t *manfid = &_ioctlarg.tuple_parse.parse.manfid;
    cistpl_funcid_t *funcid = &_ioctlarg.tuple_parse.parse.funcid;
    config_info_t config;

    if ( getTuple( CISTPL_VERS_1 ) )
    {
        for ( int i = 0; i < CISTPL_VERS_1_MAX_PROD_STRINGS; ++i )
        {
            qDebug( " PRODID = '%s'", vers->str+vers->ofs[i] );
            _productId += vers->str+vers->ofs[i];
        }
    }
        /*
        for (i = 0; i < 4; i++)
            printf("PRODID_%d=\"%s\"\n", i+1,
                (i < vers->ns) ? vers->str+vers->ofs[i] : "");
        *manfid = (cistpl_manfid_t) { 0, 0 };
        get_tuple(fd, CISTPL_MANFID, &arg);
        printf("MANFID=%04x,%04x\n", manfid->manf, manfid->card);
        *funcid = (cistpl_funcid_t) { 0xff, 0xff };
        get_tuple(fd, CISTPL_FUNCID, &arg);
        printf("FUNCID=%d\n", funcid->func);
        config.Function = config.ConfigBase = 0;
        */
}

/* internal */ void OPcmciaSocket::cleanup()
{
    // close control socket
}

/* internal */ bool OPcmciaSocket::getTuple( cisdata_t tuple )
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

    result = ::ioctl(_fd, DS_PARSE_TUPLE, &_ioctlarg);
    if ( result != 0 )
    {
        qWarning( "OPcmciaSocket::getTuple() - DS_PARSE_TUPLE failed (%s)", strerror( errno ) );
        return false;
    }

    return true;
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

const QStringList& OPcmciaSocket::productIdentity() const
{
    return _productId;
}

#if 0
const QPair& OPcmciaSocket::manufacturerIdentity() const
{
    return _manufId;
}
#endif

