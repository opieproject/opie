#include <qpe/qpeapplication.h>

#include "mainwindow.h"

int main(int argc, char **argv) {
    QPEApplication app( argc, argv );

    MainWindow mw;
    mw.setCaption(QObject::tr("Opie console") );
    app.showMainWidget( &mw );

    return app.exec();
}
