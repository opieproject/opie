#include "tabmainwindow.h"
#include <qpe/qpeapplication.h>

int main( int argc, char **argv ){
  QPEApplication application( argc, argv );
  TabMainWindow mainWindow;
  application.showMainWidget(&mainWindow);
  return application.exec();
}

//main.cpp

