

#include <qurl.h>
#include <qpe/qpeapplication.h>
#include "kvnc.h"

int main( int argc, char **argv )
{
  QPEApplication app( argc, argv );
  KVNC *view = new KVNC( "Keypebble" );
  app.showMainWidget( view );

  return app.exec();
}

