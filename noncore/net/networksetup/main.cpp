#include "mainwindowimp.h"
#include <qpe/qpeapplication.h>

int main(int argc, char **argv)
{
  QPEApplication app(argc, argv);
  MainWindowImp window;
  window.showMaximized();
  return app.exec();
}

// main.cpp

