#include <opie2/oapplication.h>
#include <qwsevent_qws.h>

#include "mainwindow.h"

class OConsoleApplication : public Opie::Core::OApplication {
public:
    OConsoleApplication(int argc, char* argv[]);
protected:
    bool qwsEventFilter( QWSEvent * );    
};

OConsoleApplication::OConsoleApplication(int argc, char* argv[] )
    : Opie::Core::OApplication(argc, argv, "opie-console")
{}

bool OConsoleApplication::qwsEventFilter( QWSEvent *e) {
    if( e->type == QWSEvent::Key ) {
	QWSKeyEvent *key = static_cast<QWSKeyEvent*>(e);
	
	/* prevent QPEApplication to filter the key */
	if(key->simpleData.keycode == Qt::Key_Escape) 
	    return QApplication::qwsEventFilter(e);
	
    }
    
    return Opie::Core::OApplication::qwsEventFilter( e);
}


int main(int argc, char* argv[]) {
    OConsoleApplication app(argc, argv);
    
    MainWindow win;
    app.showMainWidget(&win);
    
    return app.exec();
}

