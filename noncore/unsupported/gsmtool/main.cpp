#include "gsmtool.h"
#ifdef QWS
#include <qpe/qpeapplication.h>
#else
#include <qapplication.h>
#endif

int main( int argc, char ** argv )
{
#ifdef QWS
    QPEApplication a( argc, argv );

    GSMTool mw;
    a.showMainWidget( &mw );
#else
    QApplication a( argc, argv );

    GSMTool mw;
    a.setMainWidget( &mw );
    mw.show();
#endif
    return a.exec();
}
