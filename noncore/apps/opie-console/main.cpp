#include <qpe/qpeapplication.h>

#include <mainwindow.h>

int main(int argc, char **argv) {
    QPEApplication app( argc, argv );
    MainWindow* mw = new MainWindow();
    app.setMainWidget( mw );
    
    app.exec();
    
    delete mw;
    return 0;
}
