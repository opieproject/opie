#include <qpe/qpeapplication.h>

#include "mainwindow.h"


int main( int argc, char* argv[] ){
    QPEApplication app( argc, argv );
    Viewer::MainWindow wnd;
    app.showMainWidget( &wnd );

    return app.exec();
};