
#include "mainwindow.h"

#include <qpe/qpeapplication.h>
#include <qstring.h>
#include <unistd.h>
#include <stdio.h>

int main( int argc, char ** argv )
{
	setpgid(0,0);
  printf("This is oipkg\n");
  printf("$Id: main.cpp,v 1.14.2.2 2002-08-04 19:16:07 tille Exp $\n");
  QPEApplication a( argc, argv );
  MainWindow mw;
  a.showMainDocumentWidget( &mw );
  return a.exec();
}
