/*
                             This file is part of the Opie Project
              =.             (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
            .=l.             (C) 2009 Paul Eggleton <bluelightning@bluelightning.org>
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

Note: portions copied from pcmcia-utils (file src/pccardctl.c)
      (C) 2004-2005  Dominik Brodowski <linux@brodo.de>

*/

#include "opcmciasystem.h"
using namespace Opie::Core;

/* OPIE */
#include <opie2/odebug.h>

/* QT */
#include <qfile.h>
#include <qtextstream.h>
#include <qdir.h>
#include <qmap.h>

/* STD */
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


// Functions borrowed from pccardctl.c

static int pccardctl_echo_one(unsigned long socket_no, const char *in_file)
{
    int ret;
    char file[SYSFS_PATH_MAX];
    struct sysfs_attribute *attr;

    if (!in_file)
        return -EINVAL;

    snprintf(file, SYSFS_PATH_MAX, "/sys/class/pcmcia_socket/pcmcia_socket%lu/%s",
                socket_no, in_file);

    attr = sysfs_open_attribute(file);
    if (!attr)
            return -ENODEV;

    ret = sysfs_write_attribute(attr, "42", 2);

    sysfs_close_attribute(attr);

    return (ret);
}

static int pccardctl_power_socket(unsigned long socket_no, unsigned int power)
{
    int ret;
    char file[SYSFS_PATH_MAX];
        struct sysfs_attribute *attr;

    snprintf(file, SYSFS_PATH_MAX,
         "/sys/class/pcmcia_socket/pcmcia_socket%lu/card_pm_state",
         socket_no);

        attr = sysfs_open_attribute(file);
        if (!attr)
                return -ENODEV;

        ret = sysfs_write_attribute(attr, power ? "off" : "on", power ? 3 : 2);

        sysfs_close_attribute(attr);

    return (ret);
}

static int read_out_file(char * file, char **output)
{
    struct sysfs_attribute *attr = sysfs_open_attribute(file);
    int ret;
    char *result = NULL;

    *output = NULL;

    if (!attr)
        return -EIO;
    ret = sysfs_read_attribute(attr);

    if (ret || !attr->value || !attr->len || (attr->len > SYSFS_PATH_MAX))
        goto close_out;

    result = (char *)malloc(attr->len + 1);
    if (result) {
        memcpy(result, attr->value, attr->len);
        result[attr->len] = '\0';
        if (result[attr->len - 1] == '\n')
            result[attr->len - 1] = '\0';
        *output = result;
    } else
        ret = -ENOMEM;

 close_out:
    sysfs_close_attribute(attr);
    return ret;
}

static int pccardctl_get_string_socket(unsigned long socket_no, const char *in_file, char **output)
{
    char file[SYSFS_PATH_MAX];

    snprintf(file, SYSFS_PATH_MAX, "/sys/class/pcmcia_socket/pcmcia_socket%lu/%s",
         socket_no, in_file);

    return read_out_file(file, output);
}

static int pccardctl_get_string(unsigned long socket_no, const char *in_file, char **output)
{
    char file[SYSFS_PATH_MAX];

    snprintf(file, SYSFS_PATH_MAX, "/sys/bus/pcmcia/devices/%lu.0/%s",
         socket_no, in_file);

    return read_out_file(file, output);
}

static int pccardctl_get_one_f(unsigned long socket_no, unsigned int dev, const char *in_file, unsigned int *result)
{
    char *value;
    char file[SYSFS_PATH_MAX];
    int ret;

    snprintf(file, SYSFS_PATH_MAX, "/sys/bus/pcmcia/devices/%lu.%u/%s",
         socket_no, dev, in_file);
    ret = read_out_file(file, &value);
    if (ret || !value)
        return -EINVAL;

    if (sscanf(value, "0x%X", result) != 1)
        return -EIO;
    return 0;
}

static int pccardctl_get_one(unsigned long socket_no, const char *in_file, unsigned int *result)
{
    return pccardctl_get_one_f(socket_no, 0, in_file, result);
}

static int pccardctl_get_power_device(unsigned long socket_no, unsigned int func)
{
    char * value;
    char file[SYSFS_PATH_MAX];

    snprintf(file, SYSFS_PATH_MAX, "/sys/bus/pcmcia/devices/%lu.%u/pm_state",
         socket_no, func);
    read_out_file(file, &value);
    if (value) {
        if (!strncmp(value, "off", 3))
            return 1;
        return 0;
    }

    return -ENODEV;
}

static int pccardctl_get_power_socket(unsigned long socket_no)
{
    char * value;
    char file[SYSFS_PATH_MAX];

    snprintf(file, SYSFS_PATH_MAX, "/sys/class/pcmcia_socket/pcmcia_socket%lu/card_pm_state",
         socket_no);
    read_out_file(file, &value);
    if (value) {
        if (!strncmp(value, "off", 3))
            return 1;
        return 0;
    }

    return -ENODEV;
}

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

    //NOTE: We _could_ use ioctl's here as well, however we want to know if
    //      the card is recognized by the cardmgr (hence has a valid binding)
    //      If it is not recognized yet, userland may want to provide a configuration dialog

    // First, get available sockets
    QDir sktdir("/sys/class/pcmcia_socket");
    sktdir.setFilter( QDir::Dirs );

    QMap<int,QString> sockets;
    for ( unsigned int i=0; i<sktdir.count(); i++ ) {
        QString devid = sktdir[i];
        if(devid[0] == '.') continue;
        devid = devid.replace(QRegExp("^[^0123456789]*"), "");
        int numSocket = devid.toInt();
        sockets[numSocket] = QObject::tr("(no card)");
    }

    // Now, check if there are any cards in the sockets
    // FIXME This may not be reliable with multi-function cards

    QDir carddir("/sys/bus/pcmcia/devices");
    carddir.setFilter( QDir::Dirs );

    for ( unsigned int i=0; i<carddir.count(); i++ ) {
        QString devid = carddir[i];
        if(devid[0] == '.') continue;

        QString cardName;
        {
            QFile f("/sys/bus/pcmcia/devices/" + devid + "/prod_id1");
            if ( f.open(IO_ReadOnly) ) {
                QTextStream t( &f );
                cardName += t.readLine();
            }
            f.close();
        }
        {
            QFile f("/sys/bus/pcmcia/devices/" + devid + "/prod_id2");
            if ( f.open(IO_ReadOnly) ) {
                QTextStream t( &f );
                cardName += " " + t.readLine();
            }
            f.close();
        }
        cardName = cardName.stripWhiteSpace();

        // Now we need to split out the socket number
        devid = QStringList::split('.', devid)[0];
        int numSocket = devid.toInt();

        sockets[numSocket] = cardName;
    }

    QMap<int,QString>::Iterator it;
    for( it = sockets.begin(); it != sockets.end(); ++it )
        _interfaces.append( new OPcmciaSocket( it.key(), this, (const char*) it.data() ) );
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


void OPcmciaSystem::restart()
{
    //FIXME Use OProcess or something that allows more control
    ::system( "/etc/init.d/pcmcia restart" );
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
    // no longer needs to create/open PCMCIA device node
}

/* internal */ void OPcmciaSocket::cleanup()
{
    // close control socket
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
    // DS_GET_STATUS is no longer supported
    // FIXME remove this function
    return Unknown;
}


bool OPcmciaSocket::isUnsupported() const
{
    // FIXME this is no longer going to work
    return ( strcmp( name(), "unsupported card" ) == 0 );
}


bool OPcmciaSocket::isEmpty() const
{
    char *card_type = 0;
    pccardctl_get_string_socket(_socket, "card_type", &card_type);
    return (!card_type);
}


bool OPcmciaSocket::isSuspended() const
{
    return pccardctl_get_power_socket(_socket);
}


bool OPcmciaSocket::eject()
{
    return (pccardctl_echo_one(_socket, "card_eject") == 0);
}


bool OPcmciaSocket::insert()
{
    return (pccardctl_echo_one(_socket, "card_insert") == 0);
}


bool OPcmciaSocket::suspend()
{
    return (pccardctl_power_socket(_socket, 1) == 0);
}


bool OPcmciaSocket::resume()
{
    return (pccardctl_power_socket(_socket, 0) == 0);
}


bool OPcmciaSocket::reset()
{
    int ret = pccardctl_power_socket(_socket, 1);
    if (ret)
        return false;
    return (pccardctl_power_socket(_socket, 0) == 0);
}


QStringList OPcmciaSocket::productIdentityVector() const
{
    QStringList list;

    char *prod_id[4];
    int valid_prod_id = 0;
    int i;

    for (i=1; i<=4; i++) {
        char file[SYSFS_PATH_MAX];
        snprintf(file, SYSFS_PATH_MAX, "prod_id%u", i);

        pccardctl_get_string(_socket, file, &prod_id[i-1]);
        if (prod_id[i-1])
            valid_prod_id++;
    }

    if (valid_prod_id) {
        for (i=0;i<4;i++) {
            if (prod_id[i]) {
                qDebug( " PRODID = '%s'", prod_id[i] );
                list += prod_id[i];
                free(prod_id[i]);
            }
            else
                list += "";
        }
    }
    else
        list += "<unknown>";

    return list;
}


QString OPcmciaSocket::productIdentity() const
{
    return productIdentityVector().join( " " ).stripWhiteSpace();
}


QString OPcmciaSocket::manufacturerIdentity() const
{
    unsigned int manf_id, card_id;
    if (!pccardctl_get_one(_socket, "manf_id", &manf_id))
        if (!pccardctl_get_one(_socket, "card_id", &card_id))
            return QString().sprintf( "0x%04x, 0x%04x", manf_id, card_id );

    return "<unknown>";
}


QString OPcmciaSocket::function() const
{
    unsigned int func_id;
    if (!pccardctl_get_one(_socket, "func_id", &func_id))
    {
        switch ( func_id )
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
