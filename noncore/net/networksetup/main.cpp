#include "mainwindowimp.h"
#include <qpe/qpeapplication.h>

int main(int argc, char **argv)
{
  QPEApplication app(argc, argv);
  MainWindowImp window;
  app.showMainWidget(&window);
  return app.exec();
}

// main.cpp

