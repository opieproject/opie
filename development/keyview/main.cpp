#include "keyview.h"
#include <qpe/qpeapplication.h>

int main( int argc, char ** argv )
{
    QPEApplication a( argc, argv );

    Keyview kv;
    a.showMainWidget( &kv );

    return a.exec();
}
