#include <opie2/odebug.h>
#include <opie2/obluetooth.h>

using namespace Opie::Core;
using namespace Opie::Bluez;

int main( int argc, char** argv )
{
    OBluetooth* sys = OBluetooth::instance();

    OBluetooth::InterfaceIterator it = sys->iterator();

    while( it.current() )
    {
        odebug << "APP: Bluetooth host controller interface '" << it.current()->name() << "' has MAC '" << it.current()->macAddress() << "'" << oendl;
        ++it;
    }
    return 0;
}

