#include "mediumwidget.h"
#include "mediumglobal.h"
#include "mainwindow.h"

#include <qpixmap.h>
#include <qpe/qpeapplication.h>

int main( int argc, char ** argv )
{
    QPEApplication a( argc, argv );

    MediumMountSetting::MainWindow mw;
    a.showMainWidget( &mw );

    return a.exec();
}
