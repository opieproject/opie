#include <qpe/qpeapplication.h>

#include "mainwindow.h"

int main(int argc, char **argv)
{
  QPEApplication application(argc, argv);

  MainWindow windowMain;
  windowMain.setHelpFile(application.qpeDir()+"/help/html/"+QString(argv[0])+".html");
  application.showMainDocumentWidget(&windowMain);

  return application.exec();
}
