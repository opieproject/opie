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

    /* Constructs the main window, which displays messages and blocks
     * access to the desktop
     */
    Opie::Security::MultiauthMainWindow win;

    app.showMainWidget(&win);

    // resize the QDialog object so it fills all the screen
    QRect desk = qApp->desktop()->geometry();
    win.setGeometry( 0, 0, desk.width(), desk.height() );

    // the authentication has already succeeded (without win interactions)
    if ( win.isAlreadyDone() )
        return 0;

    return app.exec();
}
