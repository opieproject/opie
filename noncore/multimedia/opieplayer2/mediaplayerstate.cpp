#include <qpe/qpeapplication.h>
#include <qpe/qlibrary.h>
#include <qpe/config.h>
#include <qvaluelist.h>
#include <qobject.h>
#include <qdir.h>
#include "mediaplayerstate.h"



//#define MediaPlayerDebug(x) qDebug x
#define MediaPlayerDebug(x)


MediaPlayerState::MediaPlayerState( QObject *parent, const char *name )
        : QObject( parent, name ) {
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




// public stuff


bool MediaPlayerState::streaming() {
    return isStreaming;
}

bool MediaPlayerState::fullscreen() {
    return isFullscreen;
}

bool MediaPlayerState::scaled() {
    return isScaled;
}

bool MediaPlayerState::looping() {
    return isLooping;
}

bool MediaPlayerState::shuffled() {
    return isShuffled;
}


bool MediaPlayerState:: playlist() {
    return usePlaylist;
}

bool MediaPlayerState::paused() {
    return isPaused;
}

bool MediaPlayerState::playing() {
    return isPlaying;
}

bool MediaPlayerState::stop() {
    return isStoped;
}

long MediaPlayerState::position() {
    return curPosition;
}

long MediaPlayerState::length() {
    return curLength;
}

char MediaPlayerState::view() {
    return curView;
}

// slots
void MediaPlayerState::setIsStreaming( bool b ) {

    if ( isStreaming == b ) {
        return;
    }
    isStreaming = b;
}


void MediaPlayerState::setFullscreen( bool b ) {
    if ( isFullscreen == b ) {
        return;
    }
    isFullscreen = b;
    emit fullscreenToggled(b);
}


void MediaPlayerState::setScaled( bool b ) {
    if ( isScaled == b ) {
        return;
    }
    isScaled = b;
    emit scaledToggled(b);
}

void MediaPlayerState::setLooping( bool b ) {
    if ( isLooping    == b ) {
        return;
    }
    isLooping = b;
    emit loopingToggled(b);
}

void MediaPlayerState::setShuffled( bool b ) {
    if ( isShuffled   == b ) {
        return;
    }
    isShuffled = b;
    emit shuffledToggled(b);
}

void MediaPlayerState::setPlaylist( bool b ) {
    if ( usePlaylist  == b ) {
        return;
    }
    usePlaylist = b;
    emit playlistToggled(b);
}

void MediaPlayerState::setPaused( bool b ) {
    if ( isPaused  == b ) {
        return;
    }
    isPaused = b;
    emit pausedToggled(b);
}

void MediaPlayerState::setPlaying( bool b ) {
    if ( isPlaying  == b ) {
        return;
    }
    isPlaying = b;
    isStoped = !b;
    emit playingToggled(b);
}

void MediaPlayerState::setStop( bool b ) {
    if ( isStoped  == b ) {
        return;
    }
    isStoped = b;
    emit stopToggled(b);
}

void MediaPlayerState::setPosition( long p ) {
    if ( curPosition == p ) {
        return;
    }
    curPosition = p;
    emit positionChanged(p);
}

void MediaPlayerState::updatePosition( long p ){
    if ( curPosition  == p ) {
        return;
    }
    curPosition = p;
    emit positionUpdated(p);
}

void MediaPlayerState::setLength( long l ) {
    if ( curLength  == l ) {
        return;
    }
    curLength = l;
    emit lengthChanged(l);
}

void MediaPlayerState::setView( char v ) {
    if ( curView  == v ) {
        return;
    }
    curView = v;
    emit viewChanged(v);
}

void MediaPlayerState::setPrev(){
    emit prev();
}

void MediaPlayerState::setNext() {
    emit next();
}

void MediaPlayerState::setList() {
    setPlaying( FALSE );
    setView('l');
}

void MediaPlayerState::setVideo() {
    setView('v');
}

void MediaPlayerState::setAudio() {
    setView('a');
}

void MediaPlayerState::toggleFullscreen() {
    setFullscreen( !isFullscreen );
}

void MediaPlayerState::toggleScaled() {
    setScaled( !isScaled);
}

void MediaPlayerState::toggleLooping() {
    setLooping( !isLooping);
}

void MediaPlayerState::toggleShuffled() {
    setShuffled( !isShuffled);
}

void MediaPlayerState::togglePlaylist() {
    setPlaylist( !usePlaylist);
}

void MediaPlayerState::togglePaused() {
    setPaused( !isPaused);
}

void MediaPlayerState::togglePlaying() {
    setPlaying( !isPlaying);
}




