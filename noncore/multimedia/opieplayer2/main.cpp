
#include "mediaplayer.h"



#include <opie2/oapplicationfactory.h>

using namespace Opie::Core;
OPIE_EXPORT_APP( OApplicationFactory<PlayListWidget> )

#if 0
using namespace Opie::Core;
int main(int argc, char **argv) {
    QPEApplication a(argc,argv);

    MediaPlayerState st( 0, "mediaPlayerState" );
    PlayListWidget pl( st, 0, "playList" );
    pl.showMaximized();
    MediaPlayer mp( pl, st, 0, "mediaPlayer" );
    QObject::connect( &pl, SIGNAL( skinSelected() ),
                      &mp, SLOT( reloadSkins() ) );

    a.showMainDocumentWidget(&pl);

    return a.exec();
}
#endif

