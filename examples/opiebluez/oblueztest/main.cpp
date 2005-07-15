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
        odebug << "APP: Interface is " << ( it.current()->isUp() ? "UP" : "DOWN" ) << "." << oendl;
        //odebug << "APP: Trying to toggle state..." << oendl;
        //it.current()->setUp( !it.current()->isUp() );
        //odebug << "APP: Interface is " << ( it.current()->isUp() ? "UP" : "DOWN" ) << "." << oendl;

        odebug << "APP: scanning..." << oendl;

        OBluetoothInterface::DeviceIterator devit = it.current()->neighbourhood();
        while( devit.current() )
        {
            odebug << "APP: Neighbourhood '" << devit.current()->name() << "' has MAC '" << devit.current()->macAddress() << "'" << oendl;
            odebug << "APP: Neighbourhood '" << devit.current()->name() << "' has class '" << devit.current()->deviceClass() << "'" << oendl;
            ++devit;
        }

        ++it;
    }
    return 0;
}

