
#include "mainwindow.h"

#include <qpe/qpeapplication.h>

int main( int argc, char ** argv )
{

  QPEApplication a( argc, argv );
  MainWindow mw;
  mw.showMaximized();
  a.showMainDocumentWidget( &mw );
  return a.exec();
}
