


#include <qpe/qpeapplication.h>
#include <qpe/qlibrary.h>
#include <qpe/config.h>
#include <qvaluelist.h>
#include <qobject.h>
#include <qdir.h>
#include <qpe/mediaplayerplugininterface.h>
#include "mediaplayerstate.h"



#ifdef QT_NO_COMPONENT
// Plugins which are compiled in when no plugin architecture available
#include "libmad/libmadpluginimpl.h"
#include "libmpeg3/libmpeg3pluginimpl.h"
#include "wavplugin/wavpluginimpl.h"
#endif


//#define MediaPlayerDebug(x) qDebug x
#define MediaPlayerDebug(x)


MediaPlayerState::MediaPlayerState( QObject *parent, const char *name )
        : QObject( parent, name ), decoder( NULL ), libmpeg3decoder( NULL ) {
    Config cfg( "OpiePlayer" );
    readConfig( cfg );
}


MediaPlayerState::~MediaPlayerState() {
    Config cfg( "OpiePlayer" );
    writeConfig( cfg );
}


void MediaPlayerState::readConfig( Config& cfg ) {
    cfg.setGroup("Options");
    isFullscreen = cfg.readBoolEntry( "FullScreen" );
    isScaled = cfg.readBoolEntry( "Scaling" );
    isLooping = cfg.readBoolEntry( "Looping" );
    isShuffled = cfg.readBoolEntry( "Shuffle" );
    usePlaylist = cfg.readBoolEntry( "UsePlayList" );
    usePlaylist = TRUE;
    isPlaying = FALSE;
    isPaused = FALSE;
    curPosition = 0;
    curLength = 0;
    curView = 'l';
}


void MediaPlayerState::writeConfig( Config& cfg ) const {
    cfg.setGroup("Options");
    cfg.writeEntry("FullScreen", isFullscreen );
    cfg.writeEntry("Scaling", isScaled );
    cfg.writeEntry("Looping", isLooping );
    cfg.writeEntry("Shuffle", isShuffled );
    cfg.writeEntry("UsePlayList", usePlaylist );
}


struct MediaPlayerPlugin {
#ifndef QT_NO_COMPONENT
    QLibrary *library;
#endif
    MediaPlayerPluginInterface *iface;
    MediaPlayerDecoder *decoder;
    MediaPlayerEncoder *encoder;
};


static QValueList<MediaPlayerPlugin> pluginList;


