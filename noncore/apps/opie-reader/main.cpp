#include <qpe/qpeapplication.h>
#include "QTReaderApp.h"

#include "signal.h"
#include "stdio.h"
#include "time.h"

QTReaderApp* app = NULL;

void handler(int signum)
{
    if (app != NULL) app->saveprefs();
    signal(signum, handler);
}

int main( int argc, char ** argv )
{
    signal(SIGCONT, handler);

    QPEApplication a( argc, argv );

    QTReaderApp m;

    a.showMainDocumentWidget( &m );

    app = &m;

    return a.exec();
}

