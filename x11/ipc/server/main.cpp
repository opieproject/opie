#include <qapplication.h>
#include "ocopserver.h"


int main( int argc, char* argv[] ) {
    QApplication app(argc, argv );

    /* get the server started */
    OCopServer server;

    /* enter the event loop */
    return app.exec();
};
