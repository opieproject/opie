
#include <stdlib.h>

#include <qdir.h>
#include <qpe/config.h>
#include <opie2/odebug.h>
using namespace Opie::Core;

#include "devicehandler.h"

using namespace OpieTooth;

DeviceHandler::DeviceHandler()
{
}

DeviceHandler::~DeviceHandler()
{
}

RemoteDevice::ValueList DeviceHandler::load()
{
    RemoteDevice::ValueList list;

    Config conf("bluetooth-devices");
    QStringList grps = conf.allGroups();

    if (!grps.isEmpty() ) {
        QStringList::Iterator it;
        for (it = grps.begin(); it != grps.end(); ++it ) {
            conf.setGroup(*it);
            QString name = conf.readEntry("name", "Unknown");
            RemoteDevice currentDevice( (*it), name  );
            list.append( currentDevice );
        }
    }
    return list;
}

void DeviceHandler::save( const RemoteDevice::ValueList& list)
{
    Config conf("bluetooth-devices");

    // Get the current list of groups
    QStringList grps = conf.allGroups();
    // Remove all current devices from list
    RemoteDevice::ValueList::ConstIterator it;
    for ( it = list.begin(); it != list.end(); ++it ) {
        grps.remove( (*it).mac() );
    }
    // If there are any remaining, they've been deleted, so delete them from the file
    if (!grps.isEmpty() ) {
        QStringList::Iterator it2;
        for (it2 = grps.begin(); it2 != grps.end(); ++it2 ) {
            conf.removeGroup(*it2);
        }
    }
    // Write out the new values
    for ( it = list.begin(); it != list.end(); ++it ) {
        conf.setGroup( (*it).mac() );
        conf.writeEntry( "name", (*it).name() );
    }
}
