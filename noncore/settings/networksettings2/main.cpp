#include "networksettings.h"
#include <qpe/qpeapplication.h>

#include <opie/oapplicationfactory.h>

#ifdef GONE

OPIE_EXPORT_APP( OApplicationFactory<NetworkSettings> )

#else

#define ACT_GUI         0
#define ACT_REQUEST     1
#define ACT_REGEN       2

int main( int argc, char * argv[] ) {
        int rv = 0;
        int Action = ACT_GUI;
        // could be overruled by -qws
        QApplication::Type GuiType = QApplication::GuiClient; 

#ifdef _WS_QWS_
        QPEApplication * TheApp;
#else
        QApplication * TheApp;
#endif

        for ( int i = 1; i < argc; i ++ ) {
          int rmv;
          rmv = 0;
          if( strcmp( argv[i], "--regen" ) == 0 ) {
            Action = ACT_REGEN;
            GuiType = QApplication::Tty;
            rmv = 1;
          }
          if( rmv ) {
            memmove( argv+i, argv+i+rmv, 
                sizeof( char * ) * (argc-i-rmv) );
            i --;
            argc -= rmv;
          }
        }

        if( strstr( argv[0], "-request" ) ) {
          // called from system to request something
          GuiType = QApplication::Tty;
          Action = ACT_REQUEST;
        }

        // Start Qt
#ifdef _WS_QWS_
        // because QPEApplication does not handle GuiType well
        if( GuiType == QApplication::Tty ) {
          // this cast is NOT correct but we do not use
          // TheApp anymore ...
          TheApp = (QPEApplication *)new QApplication( argc, argv, GuiType );
        } else {
          TheApp = new QPEApplication( argc, argv, GuiType );
        }
#else
        TheApp = new QApplication( argc, argv, GuiType );
#endif

        // init qt with app widget
        if( GuiType != QApplication::Tty ) {
          QWidget * W = 0;
          W = new NetworkSettings(0);
          TheApp->setMainWidget( W ); 
          W->show();
#ifdef _WS_QWS_
          W->showMaximized();
#else
          W->resize( W->sizeHint() );
#endif
          rv = TheApp->exec();
          delete W;
        } else {
          switch( Action ) {
            case ACT_REQUEST :
              NetworkSettings::canStart( argv[1] );
              break;
            case ACT_REGEN :
              // regen returns 0 if OK
              rv = (NetworkSettings::regenerate()) ? 1 : 0;
              break;
          }
        }

        return rv;
}

#endif


// main.cpp

