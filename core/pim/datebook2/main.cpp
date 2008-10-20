#include "mainwindow.h"

#include <qpe/qpeapplication.h>

using namespace Opie;

int main( int argc, char* argv[] ) {
    QPEApplication app( argc, argv );

    Datebook::MainWindow mw;

    app.showMainWidget( &mw );

    return app.exec();
}
