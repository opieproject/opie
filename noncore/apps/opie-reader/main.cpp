#include "useqpe.h"
#ifdef USEQPE
#include <qpe/qpeapplication.h>
#else
#include <qapplication.h>
#endif
#include "QTReaderApp.h"

#include "signal.h"
#include "stdio.h"
#include "time.h"


#ifdef USEQPE
QTReaderApp* app = NULL;

void handler(int signum)
{
    if (app != NULL)
    {
	app->suspend();
	app->saveprefs();
    }
    signal(signum, handler);
}
#endif

int main( int argc, char ** argv )
{

#ifdef USEQPE
    signal(SIGCONT, handler);
    QPEApplication a( argc, argv );
    QTReaderApp m;
    a.showMainDocumentWidget( &m );
    app = &m;
#else
    QApplication a( argc, argv );
    QTReaderApp m;
    a.setMainWidget( &m );
	if (argc > 1)
	{
	    m.setDocument(argv[1]);
	}
#endif


    return a.exec();
}

