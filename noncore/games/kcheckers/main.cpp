
#include <qpe/qpeapplication.h>
#include <qfont.h>

#include "kcheckers.h"


int main(int argc, char *argv[])
{
  QPEApplication app(argc,argv);

  KCheckers kcheckers;
  app.setMainWidget(&kcheckers);
  kcheckers.showMaximized();

  return app.exec();
}

