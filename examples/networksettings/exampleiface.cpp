#include "exampleiface.h"

#include <opie2/odebug.h>
using namespace Opie::Core;

VirtualInterface::VirtualInterface( QObject* parent,
                                    const char* name,
                                    bool status )
    : Interface(parent, name, status )
{
}

VirtualInterface::~VirtualInterface() {
}

bool VirtualInterface::refresh() {
/* we do VPN over ppp
 * so replace the interfaceName with
 * something actual existing
 * I take wlan0 in my case
 */
    QString old = getInterfaceName();
    odebug << "Interface name was " << old << oendl;
    setInterfaceName( "wlan0" );

    bool b =Interface::refresh();
    setInterfaceName( old );

/*  new and old interface name */
    emit updateInterface(this);
    return b;
}


void VirtualInterface::start() {
// call pptp
    setStatus(true);
    refresh();
    emit updateMessage("VPN started");
}

void VirtualInterface::stop() {
    setStatus(false );
    refresh();
    emit updateMessage("VPN halted");
}
