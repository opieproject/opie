#include <opie2/multiauthmainwindow.h>
#include <opie2/multiauthcommon.h>

#include <opie2/oapplication.h>

/// main() function of the opie-multiauth-standalone demonstration application
/**
 * Calls each plugin one after the other, and asks them to perform one authentication.
 * It loads the plugins one after another through the MultiauthPluginInterface,
 * then calls MultiauthPluginObject::authenticate().
 * \em Note: calls are not parsed by doxygen since they are done via a QInterfacePtr:
 * \see http://dudu.dyn.2-h.org/nist/doxydoc/allOpie//classQInterfacePtr.html
 */
int main( int argc, char ** argv )
{
    Opie::Core::OApplication app(argc, argv, "Multi-authentication demo");

    /*
     * Run authentication and retursn if successfull
     * This uses the global settings
     */
    Opie::Security::MultiauthPassword::authenticate();
}
