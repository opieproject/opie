
#include <qpe/qpeapplication.h>
#include "mediaplayerstate.h"
#include "playlistwidget.h"
#include "mediaplayer.h"

PlayListWidget *playList;

int main(int argc, char **argv) {
    QPEApplication a(argc,argv);

    MediaPlayerState st( 0, "mediaPlayerState" );
    PlayListWidget pl( st, 0, "playList" );
    playList = &pl;
    pl.showMaximized();
    MediaPlayer mp( st, 0, "mediaPlayer" );
    QObject::connect( &pl, SIGNAL( skinSelected() ),
                      &mp, SLOT( recreateAudioAndVideoWidgets() ) );

    a.showMainDocumentWidget(&pl);

    return a.exec();
}


