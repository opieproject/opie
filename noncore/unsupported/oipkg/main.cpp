
#include "mainwindow.h"

#include <qpe/qpeapplication.h>
#include <qstring.h>
#include <unistd.h>
#include <stdio.h>

int main( int argc, char ** argv )
{
	setpgid(0,0);
  printf("This is oipkg\n");
  printf("$Id: main.cpp,v 1.14.2.1 2002-08-04 15:18:54 tille Exp $\n");
  QPEApplication a( argc, argv );
  qDebug("creating mainwindow");
  MainWindow mw;
  qDebug(" showMainDocumentWidget");
  a.showMainDocumentWidget( &mw );
  qDebug(" a.exec()");
  return a.exec();
}
