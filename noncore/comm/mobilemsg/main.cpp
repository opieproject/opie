#include "mobilemsg.h"
#include <qpe/qpeapplication.h>

int main( int argc, char ** argv )
{
    QPEApplication a( argc, argv );

    MobileMsg mw;
    a.showMainWidget( &mw );

    return a.exec();
}
