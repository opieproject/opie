
#include "mainwindow.h"

#include <qpe/qpeapplication.h>
#include <qstring.h>
int debugLevel;

int main( int argc, char ** argv )
{
  debugLevel = 1;
  if (argc > 2)
    {
      debugLevel = QString ( argv[1] ).toInt();
    }
  QPEApplication a( argc, argv );
  MainWindow mw;
  a.showMainDocumentWidget( &mw );
  return a.exec();
}
