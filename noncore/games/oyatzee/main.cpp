#include "wordgame.h"

#include <qpe/qpeapplication.h>

int main( int argc, char ** argv )
{
    QPEApplication a( argc, argv );

    OYatzee mw;
    a.showMainWidget(&mw);

    return a.exec();
}
