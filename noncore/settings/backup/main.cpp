#include "backuprestore.h"
#include "qnetworkprotocol.h"
#include <qpe/qpeapplication.h>

int main(int argc, char *argv[]) {
  QPEApplication a( argc, argv );
  
  BackupAndRestore app(0, "mainwindow");
  app.show();
  return a.exec();
}

// main.cpp

