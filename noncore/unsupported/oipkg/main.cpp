
#include "mainwindow.h"

#include <qpe/qpeapplication.h>
#include <qstring.h>
//#include <qmessagebox.h>
int debugLevel;

int main( int argc, char ** argv )
{
	setpgid(0,0);
  printf("This is oipkg\n");
  printf("$Id: main.cpp,v 1.13 2002-06-28 17:01:13 tille Exp $\n");
  debugLevel = 2;
  if (argc > 1)
  {
      debugLevel = QString ( argv[1] ).toInt();
    	qDebug("setting debug level to %i",debugLevel);
  }
  QPEApplication a( argc, argv );
  MainWindow mw;
//  if (argc > 2)
//		QMessageBox::information( &mw, "oipkg","While ipkg is working\noipkg is hanging.\nPlease be patient!");
  a.showMainDocumentWidget( &mw );
  return a.exec();
}
