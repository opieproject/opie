#include "nsdata.h"
#include "activateprofile.h"
#include "activatevpn.h"
#include "networksettings.h"

#include <opie2/odebug.h>
#include <qpe/qpeapplication.h>

#include <opie2/oapplicationfactory.h>
using namespace Opie::Core;

#ifdef GONE

OPIE_EXPORT_APP( OApplicationFactory<NetworkSettings> )

#else

// just standard GUI
#define ACT_GUI         0
// used by interfaces to request for allow of up/down
#define ACT_REQUEST     1
// regenerate config files
#define ACT_REGEN       2
// used by interfaces to request user prompt
#define ACT_PROMPT      3
// used by interfaces to trigger VPN prompting
#define ACT_TRIGGERVPN  4
// activate opietooth
#define ACT_OT          5
// prompt for VPN networks
#define ACT_PROMPTVPN   6

// include Opietooth GUI
#include <opietooth2/Opietooth.h>
using namespace Opietooth2;

#include <qpushbutton.h>
#include <qlayout.h>
#include <qframe.h>
#include <qlabel.h>

int main( int argc, char * argv[] ) {
        int rv = 0;
        int Action = ACT_GUI;
        // could be overruled by -qws
        QApplication::Type GuiType = QApplication::GuiClient;

        QPEApplication * TheApp;

        for ( int i = 1; i < argc; i ++ ) {
          int rmv;
          rmv = 0;
          if( strcmp( argv[i], "--regen" ) == 0 ) {
            Action = ACT_REGEN;
            GuiType = QApplication::Tty;
            rmv = 1;
          } else if( strcmp( argv[i], "--prompt" ) == 0 ) {
            Action = ACT_PROMPT;
            rmv = 1;
          } else if( strcmp( argv[i], "--triggervpn" ) == 0 ) {
            GuiType = QApplication::Tty;
            Action = ACT_TRIGGERVPN;
            rmv = 1;
          } else if( strcmp( argv[i], "--promptvpn" ) == 0 ) {
            Action = ACT_PROMPTVPN;
            rmv = 1;
          } else if( strcmp( argv[i], "--opietooth" ) == 0 ) {
            Action = ACT_OT;
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
          Log(("Request : %s\n", argv[1] ));
        } else if( strstr( argv[0], "-opietooth" ) ) {
          Action = ACT_OT;
        }

        // Start Qt
        // because QPEApplication does not handle GuiType well
        if( GuiType == QApplication::Tty ) {
          // this cast is NOT correct but we do not use
          // TheApp anymore ...
          TheApp = (QPEApplication *)new QApplication( argc, argv, GuiType );
        } else {
          TheApp = new QPEApplication( argc, argv, GuiType );
        }

        // init qt with app widget

        switch( Action ) {
          case ACT_REQUEST :
            { NetworkSettingsData NS;
              if( NS.canStart( argv[1] ) ) {
                QStringList SL;
                SL << QPEApplication::qpeDir() + "bin/networksettings2"
                   << "--prompt"
                   << argv[1];
                // exec synchronous -> blocks
                NSResources->system().execAsUser( SL, 1 );
              }
            }
            break;
          case ACT_TRIGGERVPN :
            { NetworkSettingsData NS;
              if( NS.couldBeTriggered( argv[1] ) ) {
                // there are VPNS that can be triggered
                QStringList S;
                S << QPEApplication::qpeDir() + "bin/networksettings2";
                S << "--promptvpn";
                S << argv[1];
                NSResources->system().execAsUser( S );
              }
            }
            break;
          case ACT_REGEN :
            { NetworkSettingsData NS;
              QString S= NS.generateSettings();
              // regen returns 0 if OK
              rv = ( S.isEmpty() ) ? 0 : 1;
            }
            break;
          case ACT_PROMPT :
            { ActivateProfile AP(argv[1]);
              if( AP.exec() == QDialog::Accepted ) {
                Log(("allow profile %ld for %s\n",
                    AP.selectedProfile(), argv[1] ));
                printf( "A%ld%s\n", AP.selectedProfile(), argv[1] );
              } else {
                Log(("disallow %s\n", argv[1] ));
                printf( "D-%s\n", argv[1] );
              }
            }
            break;
          case ACT_PROMPTVPN :
            { ActivateVPN AVPN( argv[1] );
              Log(("Trigger vpns on interface %s\n", argv[1] ));
              AVPN.exec();
            }
            break;
          case ACT_GUI :
          case ACT_OT :
            { QWidget * W;

              if( Action == ACT_OT ) {
                W = new OTMain( 0 );
              } else {
                W = new NetworkSettings(0);
              }
              TheApp->setMainWidget( W );

              W->show();
              W->showMaximized();
              rv = TheApp->exec();

              delete W;
            }
            break;
        }

        LogClose();

        return rv;
}

#endif


// main.cpp

