
#include <qpe/qpeapplication.h>
#include "mediaplayerstate.h"
#include "playlistwidget.h"
#include "audiowidget.h"
#include "videowidget.h"
#include "mediaplayer.h"

MediaPlayerState *mediaPlayerState;
PlayListWidget *playList;
AudioWidget *audioUI;
VideoWidget *videoUI;

int main(int argc, char **argv) {
    QPEApplication a(argc,argv);

    MediaPlayerState st( 0, "mediaPlayerState" );
    mediaPlayerState = &st;
    PlayListWidget pl( 0, "playList" );
    playList = &pl;
    AudioWidget aw( 0, "audioUI" );
    audioUI = &aw;
    VideoWidget vw( 0, "videoUI" );
    videoUI = &vw;

    MediaPlayer mp( 0, "mediaPlayer" );

    a.showMainDocumentWidget(&pl);

    return a.exec();
}


