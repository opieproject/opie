
#include "mainwindow.h"

#include <qpe/qpeapplication.h>
#include <stdio.h>
int main( int argc, char ** argv )
{
  printf("This is confedit\n");
  printf("$Id: main.cpp,v 1.1 2002-06-29 20:15:06 tille Exp $\n");

  QPEApplication a( argc, argv );
  MainWindow mw;
  a.showMainDocumentWidget( &mw );
  return a.exec();
}
