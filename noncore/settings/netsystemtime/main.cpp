//#include "settime.h"
#include "ntp.h"
#include <qpe/qpeapplication.h>

int main( int argc, char ** argv )
{
    QPEApplication a( argc, argv );

  //  SetDateTime mw;
    Ntp mw;
    a.showMainWidget( &mw );

    return a.exec();
}
