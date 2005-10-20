#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QPEApplication a(argc, argv);

    mainWindowWidget mainWindow;

    a.showMainWidget(&mainWindow);

    return a.exec();
}
