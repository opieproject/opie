#include "gsmtool.h"
#include <qpe/qpeapplication.h>

int main( int argc, char ** argv )
{
    QPEApplication a( argc, argv );

    GSMTool mw;
    a.showMainWidget( &mw );

    return a.exec();
}
