
#include "mainwindow.h"

#include <qpe/qpeapplication.h>
#include <qstring.h>
//#include <qmessagebox.h>
int debugLevel;

int main( int argc, char ** argv )
{
  printf("This is oipkg\n");
  printf("$id$\n");
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
