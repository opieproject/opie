#include <qpe/qpeapplication.h>
#include "mainwindow.h"

int main(int argc, char **argv) {
    QPEApplication a(argc, argv);
    MainWindow mw;
    a.showMainWidget(&mw);
    return a.exec();
}

