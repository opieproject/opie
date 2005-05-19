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
        QString line = cardinfo.readLine();
        if ( line.startsWith( "Socket" ) )
        {
            int mid = line.find( ':' );
            QString name = line.right( line.length() - mid - 1 );
            QString id = line.right( line.length() - mid + 1 );
            if ( mid ) _interfaces.insert( name, new OPcmciaCard( this, (const char*) id ) );
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


OPcmciaCard* OPcmciaSystem::card( const QString& iface ) const
{
    return _interfaces[iface];
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
 * OPcmciaCard
 *======================================================================================*/

OPcmciaCard::OPcmciaCard( QObject* parent, const char* name )
                 :QObject( parent, name )
{
    odebug << "OPcmciaCard::OPcmciaCard()" << oendl;
    init();
}


OPcmciaCard::~OPcmciaCard()
{
}


void OPcmciaCard::init()
{
}


