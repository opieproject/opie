#include "example.h"
#include <qpe/qpeapplication.h>

int main( int argc, char ** argv )
{
    QPEApplication a( argc, argv );

    Example mw;
    a.showMainWidget( &mw );

    return a.exec();
}
