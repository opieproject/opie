
#include "mainwindow.h"

#include <opie2/oapplication.h>

int main( int argc, char ** argv )
{
  OApplication a( argc, argv, "confedit" );
  MainWindow mw;
  mw.showMaximized();
  a.showMainDocumentWidget( &mw );
  return a.exec();
}
