#include "powerchord.h"
#include <qpe/qpeapplication.h>

int main( int argc, char ** argv )
{
    QPEApplication a( argc, argv );

    PowerchordBase p;
    a.showMainWidget( &p );

    return a.exec();
}
