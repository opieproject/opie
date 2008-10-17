#include "mainwindow.h"

#include <qpe/qpeapplication.h>


int main( int argc, char* argv[] ) {
    QPEApplication app( argc, argv );

    Datebook::MainWindow mw;
    mw.setCaption( QObject::tr("Opie Datebook") );

    app.showMainWidget( &mw );

    return app.exec();
}
